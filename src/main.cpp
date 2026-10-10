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

#include "kv_store/kv_store.hpp"
#include "net/socket_handle.hpp"
#include "redis_command.hpp"
#include "utils/string_utils.hpp"
#include "utils/time_utils.hpp"

auto clientHandler(KVStore &kvStore, Socket &&client) {
    while (true) {

        auto commandData = parseRedisCommandFromNetwork(client);

        auto &command = commandData[0];

        switch (parseCommand(command)) {
            case Command::PING:
                client.sendString("+PONG\r\n");
                break;
            case Command::ECHO: {
                auto &echoArg = commandData[1];
                client.sendString(convertToRESPBulkString(echoArg));
                break;
            }
            case Command::SET: {
                auto &key = commandData[1];
                auto &value = commandData[2];

                auto expireTime = TimePoint::max();

                if (commandData.size() >= 5) {
                    auto &expireType = commandData[3];
                    auto delta = std::stoll(commandData[4]);

                    expireTime = Clock::now() + std::chrono::milliseconds(expireType == "EX" ? delta * 1000 : delta);
                }

                kvStore.updatedKeyValueStore(key, value, expireTime);

                client.sendString(convertToSimpleString("OK"));
                break;
            }
            case Command::GET: {
                auto &key = commandData[1];
                auto value = kvStore.getValue(key);

                if (value.has_value()) {
                    client.sendString(convertToRESPBulkString(*value));
                }
                else {
                    client.sendString(NULL_BULK_STRING);
                }

                break;
            }
            case Command::UNKNOWN:
                throw "unknown command";
        }
    }
}

auto createKVStore(cxxopts::ParseResult result) -> KVStore {
    if (result.contains("dir")) {
        auto dir = result["dir"].as<std::string>();
        auto dbfilename = result["dbfilename"].as<std::string>();

        return {dir, dbfilename};
    }
    else {
        return {"", ""};
    }
}

int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    cxxopts::Options options("test", "A brief description");

    options.add_options()("dir", "the path to the directory where the RDB file is stored (example: /tmp/redis-data)",
                          cxxopts::value<std::string>())("dbfilename", "the name of the RDB file (example: rdbfile)",
                                                         cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    auto kvStore = createKVStore(result);

    constexpr uint32_t port = 6379;
    auto server = Socket::createServer(port);

    while (true) {
        auto client = server.acceptClient();
        std::cout << "Client connected\n";
        std::thread t(clientHandler, std::ref(kvStore), std::move(client));
        t.detach();
    }

    server.close();

    return 0;
}
