#include <iostream>
#include <print>
#include <stdexcept>
#include <thread>

#include "rdb/rdb.hpp"
#include "redis/master_link.hpp"
#include "redis/redis.hpp"
#include "redis/session.hpp"
#include "resp/encode.hpp"
#include "utils/string_utils.hpp"

namespace {

auto makeRole(const RedisConfig &config) -> Role {
    // In place: MasterRole owns a mutex and cannot be moved.
    if (config.replicaof.has_value()) {
        return Role(std::in_place_type<ReplicaRole>, *config.replicaof);
    }
    return Role(std::in_place_type<MasterRole>);
}

} // namespace

Redis::Redis(RedisConfig config)
    : config(std::move(config)), aof(this->config.dir, this->config.aof), role(makeRole(this->config)) {}

auto Redis::loadState() -> void {
    if (!config.rdb.dbfilename.empty()) {
        rdb::load(config.dir + "/" + config.rdb.dbfilename, store);
    }

    for (auto &&args : aof.replay()) {
        execute(args, Origin::AOF_REPLAY);
    }

    aof.open();
}

auto Redis::execute(const std::vector<std::string> &args, Origin origin, Session *session) -> Reply {
    const auto *command = registry.find(toUpper(args[0]));

    if (command == nullptr) {
        throw std::runtime_error("unknown command");
    }

    std::unique_lock guard(lock);

    CommandContext ctx{
        .args = args, .store = store, .config = config, .role = role, .session = session, .lock = guard};
    auto reply = command->handler(ctx);

    if (command->write && origin != Origin::AOF_REPLAY) {
        auto encoded = resp::array(args);
        aof.append(encoded);

        if (auto *master = std::get_if<MasterRole>(&role)) {
            master->propagate(encoded);
        }
    }

    if (origin == Origin::MASTER) {
        auto *replica = std::get_if<ReplicaRole>(&role);

        if (replica == nullptr) {
            throw std::runtime_error("Master has recieved message from master.");
        }

        auto encoded = resp::array(args);
        replica->processCommand(encoded);
    }

    return reply;
}

auto Redis::run() -> void {
    try {
        loadState();

        std::println("Starting server with port {}", config.port);
        std::cout.flush();

        auto server = Socket::createServer(config.port);

        if (auto *replica = std::get_if<ReplicaRole>(&role)) {
            std::thread([this, master = replica->getMaster()]() {
                try {
                    MasterLink(master, config.port, *this).run();
                } catch (const std::exception &e) {
                    std::println(std::cerr, "Could not connect to master: {}", e.what());
                    std::cerr.flush();
                }
            }).detach();
        }

        while (true) {
            auto client = server.acceptClient();

            std::cout << "Client connected\n";
            std::cout.flush();

            std::thread([this, client = std::move(client)]() mutable {
                Session(std::move(client), *this).run();
            }).detach();
        }

    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}
