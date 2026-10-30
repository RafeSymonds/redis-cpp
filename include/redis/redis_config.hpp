#include <filesystem>
#include <string>

#include "aof/aof_manager.hpp"
#include "rdb/rdb_file_manager.hpp"

#include "replication/role.hpp"

static constexpr uint16_t DEFAULT_PORT = 6379;

struct RedisConfig {
    std::string dir = std::filesystem::current_path();
    RDBConfig rdb;
    AOFConfig aof;
    uint16_t port = DEFAULT_PORT;

    std::optional<std::string> masterHostname;
    std::optional<uint32_t> masterPort;
};
