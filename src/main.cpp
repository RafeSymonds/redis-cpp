#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <ranges>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "cxxopts.hpp"

#include "kv_store/kv_store.hpp"
#include "net/socket_handle.hpp"
#include "redis/redis.hpp"
#include "redis_command.hpp"
#include "utils/string_utils.hpp"
#include "utils/time_utils.hpp"

auto clientHandler(Redis &redis, Socket &&client) {
    try {

        while (true) {

            auto commandData = parseRedisCommandFromNetwork(client);

            auto &command = commandData[0];

            redis.aofFileManager.persistCommand(convertToRESPArray(commandData));

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

                        expireTime =
                            Clock::now() + std::chrono::milliseconds(expireType == "EX" ? delta * 1000 : delta);
                    }

                    redis.kvStore.updatedKeyValueStore(key, value, expireTime);

                    client.sendString(convertToSimpleString("OK"));
                    break;
                }
                case Command::GET: {
                    auto &key = commandData[1];
                    auto value = redis.kvStore.getValue(key);

                    if (value.has_value()) {
                        client.sendString(convertToRESPBulkString(*value));
                    }
                    else {
                        client.sendString(NULL_BULK_STRING);
                    }

                    break;
                }
                case Command::CONFIG: {

                    auto &type = commandData[1];

                    std::vector<std::string> output;

                    if (type == "GET") {
                        for (auto &&setting : commandData | std::views::drop(2)) {
                            if (setting == "dir") {
                                output.emplace_back("dir");
                                output.push_back(redis.dbFileManager.getDir());
                            }
                            else if (setting == "dbfilename") {
                                output.emplace_back("dbfilename");
                                output.push_back(redis.dbFileManager.getConfig().dbfilename);
                            }
                            else if (setting == "appendonly") {
                                output.emplace_back("appendonly");
                                output.push_back(redis.aofFileManager.getConfig().appendonlyString());
                            }
                            else if (setting == "appenddirname") {
                                output.emplace_back("appenddirname");
                                output.push_back(redis.aofFileManager.getConfig().appenddirname);
                            }
                            else if (setting == "appendfilename") {
                                output.emplace_back("appendfilename");
                                output.push_back(redis.aofFileManager.getConfig().appendfilename);
                            }
                            else if (setting == "appendfsync") {
                                output.emplace_back("appendfsync");
                                output.push_back(toString(redis.aofFileManager.getConfig().appendfsync));
                            }
                        }
                    }

                    client.sendString(convertToRESPArray(output));
                    break;
                }
                case Command::KEYS: {

                    auto &pattern = commandData[1];
                    auto values = redis.kvStore.getValues(pattern);

                    client.sendString(convertToRESPArray(values));

                    break;
                }
                case Command::UNKNOWN:
                    throw "unknown command";
            }
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
