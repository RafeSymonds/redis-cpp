#pragma once

#include <string>

#include "aof/aof_manager.hpp"
#include "kv_store/kv_store.hpp"
#include "net/socket_handle.hpp"
#include "rdb/rdb_file_manager.hpp"
#include "redis/redis_config.hpp"
#include "replication/role.hpp"

class Redis {
    auto setup() -> void;

    auto processCommand(const std::vector<std::string> &commandData, bool persistCommand = true) -> std::string;

    auto clientHandler(Socket client) -> void;

    auto IsMaster() const -> bool { return !masterHostname.has_value(); }

  public:
    RDBFileManager dbFileManager;
    AOFManager aofFileManager;
    KVStore kvStore;

    Role role;

    uint32_t port;
    std::optional<std::string> masterHostname;
    std::optional<uint32_t> masterPort;

    explicit Redis(const RedisConfig &config)
        : dbFileManager(config.dir, config.rdb), aofFileManager(config.dir, config.aof),
          kvStore(dbFileManager.readDBFile()), port(config.port), masterHostname(config.masterHostname),
          masterPort(config.masterPort) {
        setup();
    }

    auto run() -> void;
};
