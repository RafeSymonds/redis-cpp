#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

constexpr uint16_t DEFAULT_PORT = 6379;

enum class AppendFSync : uint8_t { ALWAYS, EVERYSEC, NO };

auto parseAppendFSync(const std::string &s) -> AppendFSync;
auto toString(AppendFSync appendfsync) -> std::string;

struct RDBConfig {
    std::string dbfilename;
};

struct AOFConfig {
    bool appendonly = false;
    std::string appenddirname = "appendonlydir";
    std::string appendfilename = "appendonly.aof";
    AppendFSync appendfsync = AppendFSync::EVERYSEC;
};

struct ReplicaOf {
    std::string hostname;
    uint16_t port;
};

// The single source of server configuration. Built once in main, owned by Redis.
struct RedisConfig {
    std::string dir = std::filesystem::current_path();
    RDBConfig rdb;
    AOFConfig aof;
    uint16_t port = DEFAULT_PORT;

    // Set when this instance is a replica.
    std::optional<ReplicaOf> replicaof;
};

// Value of a CONFIG GET parameter, or nullopt for unknown parameters.
auto configGet(const RedisConfig &config, std::string_view name) -> std::optional<std::string>;
