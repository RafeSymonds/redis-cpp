#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "cxxopts.hpp"

#include "redis/redis.hpp"
#include "utils/string_utils.hpp"

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
    if (result.contains("port")) {
        config.port = result["port"].as<uint32_t>();
    }
    if (result.contains("replicaof")) {
        auto info = result["replicaof"].as<std::string>();

        auto result = split(info, ' ');

        if (result.size() != 2) {
            throw std::runtime_error(
                std::format("not the correct number of inputs for repliace master. Input was {}", info));
        }

        config.masterHostname = result[0];
        config.masterPort = std::stoull(result[1]);
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
    addOption("port", "passes the port number to the Redis server", cxxopts::value<uint32_t>());
    addOption("replicaof", "instance should be replica", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    Redis redis(parseConfig(result));
    redis.run();
}
