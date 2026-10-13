#include <iostream>
#include <print>
#include <ranges>
#include <thread>

#include "redis/redis.hpp"
#include "utils/string_utils.hpp"
#include <redis_command.hpp>

auto Redis::setup() -> void {
    for (auto &&commandData : aofFileManager.readCommands()) {
        processCommand(commandData, false);
    }

    aofFileManager.open();
}

auto Redis::processCommand(const std::vector<std::string> &commandData, bool persistCommand) -> std::string {
    auto &command = commandData[0];

    switch (parseCommand(command)) {
        case Command::PING:
            return "+PONG\r\n";
        case Command::ECHO: {
            auto &echoArg = commandData[1];
            return convertToRESPBulkString(echoArg);
        }
        case Command::SET: {
            if (persistCommand) {
                aofFileManager.persistCommand(convertToRESPArray(commandData));
            }

            auto &key = commandData[1];
            auto &value = commandData[2];

            auto expireTime = TimePoint::max();

            if (commandData.size() >= 5) {
                auto &expireType = commandData[3];
                auto delta = std::stoll(commandData[4]);

                expireTime = Clock::now() + std::chrono::milliseconds(expireType == "EX" ? delta * 1000 : delta);
            }

            kvStore.updatedKeyValueStore(key, value, expireTime);

            return convertToSimpleString("OK");
        }
        case Command::GET: {
            auto &key = commandData[1];
            auto value = kvStore.getValue(key);

            if (value.has_value()) {
                return convertToRESPBulkString(*value);
            }
            return NULL_BULK_STRING;
        }
        case Command::CONFIG: {

            auto &type = commandData[1];

            std::vector<std::string> output;

            if (type == "GET") {
                for (auto &&setting : commandData | std::views::drop(2)) {
                    if (setting == "dir") {
                        output.emplace_back("dir");
                        output.push_back(dbFileManager.getDir());
                    }
                    else if (setting == "dbfilename") {
                        output.emplace_back("dbfilename");
                        output.push_back(dbFileManager.getConfig().dbfilename);
                    }
                    else if (setting == "appendonly") {
                        output.emplace_back("appendonly");
                        output.push_back(aofFileManager.getConfig().appendonlyString());
                    }
                    else if (setting == "appenddirname") {
                        output.emplace_back("appenddirname");
                        output.push_back(aofFileManager.getConfig().appenddirname);
                    }
                    else if (setting == "appendfilename") {
                        output.emplace_back("appendfilename");
                        output.push_back(aofFileManager.getConfig().appendfilename);
                    }
                    else if (setting == "appendfsync") {
                        output.emplace_back("appendfsync");
                        output.push_back(toString(aofFileManager.getConfig().appendfsync));
                    }
                }
            }

            return convertToRESPArray(output);
        }
        case Command::KEYS: {
            auto &pattern = commandData[1];
            auto values = kvStore.getValues(pattern);

            return convertToRESPArray(values);
        }
        case Command::INFO: {

            std::string result = std::format("# Replication\nrole:{}", IsMaster() ? "master" : "slave");

            return convertToRESPBulkString(result);
        }
        case Command::REPLCONF: {
            return OK_SIMPLE_STRING;
        }
        case Command::PSYNC: {
            auto result = convertToSimpleString(std::format("FULLRESYNC {} 0", replicaManager->getReplicationId()));
            return result;
        }
        case Command::UNKNOWN:
            throw std::runtime_error("unknown command");
    }
}

auto Redis::clientHandler(Socket client) -> void {
    try {
        while (true) {
            auto commandData = parseRedisCommand(client);
            client.sendString(processCommand(commandData));
        }
    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}

auto Redis::run() -> void {
    try {
        std::println("Starting server with port {}", port);
        std::cout.flush();
        auto server = Socket::createServer(port);

        while (true) {
            auto client = server.acceptClient();

            std::cout << "Client connected\n";
            std::cout.flush();

            std::thread t([this](Socket client) { clientHandler(client); }, client);
            t.detach();
        }

        server.close();

    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}
