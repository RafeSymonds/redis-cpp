#include <filesystem>
#include <string>

#include "aof/aof_file_manager.hpp"
#include "kv_store/kv_store.hpp"
#include "rdb/rdb_file_manager.hpp"
#include <net/socket_handle.hpp>

constexpr uint32_t DEFAULT_PORT = 6379;

struct RedisConfig {
    std::string dir = std::filesystem::current_path();
    RDBConfig rdb;
    AOFConfig aof;
    uint32_t port = DEFAULT_PORT;
};

class Redis {

    auto setup() -> void;

  public:
    RDBFileManager dbFileManager;
    AOFFileManager aofFileManager;
    KVStore kvStore;

    uint32_t port;

    explicit Redis(const RedisConfig &config)
        : dbFileManager(config.dir, config.rdb), aofFileManager(config.dir, config.aof),
          kvStore(dbFileManager.readDBFile()), port(config.port) {
        setup();
    }

    auto processCommand(const std::vector<std::string> &commandData, bool persistCommand = true) -> std::string;

    auto clientHandler(Socket client) -> void;

    auto run() -> void;
};
