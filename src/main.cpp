#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "cxxopts.hpp"

#include "net/socket_handle.hpp"
#include "redis/redis.hpp"
#include "utils/string_utils.hpp"

auto clientHandler(Redis &redis, Socket &&client) {
    try {
        while (true) {
            auto commandData = parseRedisCommand(client);
            client.sendString(redis.processCommand(commandData));
        }
    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}

static auto parseConfig(const cxxopts::ParseResult &result) -> RedisConfig {
    RedisConfig config;

    if (result.contains("dir")) {
        config.dir = result["dir"].as<std::string>();
    }
    if (result.contains("dbfilename")) {
        config.rdb.dbfilename = result["dbfilename"].as<std::string>();
    }
    if (result.contains("appendonly")) {
        config.aof.appendonly = result["appendonly"].as<std::string>() == "yes";
    }
    if (result.contains("appenddirname")) {
        config.aof.appenddirname = result["appenddirname"].as<std::string>();
    }
    if (result.contains("appendfilename")) {
        config.aof.appendfilename = result["appendfilename"].as<std::string>();
    }
    if (result.contains("appendfsync")) {
        config.aof.appendfsync = parseAppendFSync(result["appendfsync"].as<std::string>());
    }

    return config;
}

int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    cxxopts::Options options("test", "A brief description");

    auto addOption = options.add_options();
    addOption("dir", "the path to the directory where the RDB file is stored (example: /tmp/redis-data)",
              cxxopts::value<std::string>());
    addOption("dbfilename", "the name of the RDB file (example: rdbfile)", cxxopts::value<std::string>());
    addOption("appendonly", "enable the append-only file (yes/no)", cxxopts::value<std::string>());
    addOption("appenddirname", "the directory where AOF files are stored", cxxopts::value<std::string>());
    addOption("appendfilename", "the base name of the AOF file", cxxopts::value<std::string>());
    addOption("appendfsync", "AOF fsync policy (always/everysec/no)", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    try {
        Redis redis(parseConfig(result));

        constexpr uint32_t port = 6379;
        auto server = Socket::createServer(port);

        while (true) {
            auto client = server.acceptClient();
            std::cout << "Client connected\n";
            std::thread t(clientHandler, std::ref(redis), std::move(client));
            t.detach();
        }

        server.close();
    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }

    return 0;
}
