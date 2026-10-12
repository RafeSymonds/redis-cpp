#include <filesystem>
#include <string>

#include "aof/aof_file_manager.hpp"
#include "kv_store/kv_store.hpp"
#include "rdb/rdb_file_manager.hpp"

struct RedisConfig {
    std::string dir = std::filesystem::current_path();
    RDBConfig rdb;
    AOFConfig aof;
};

class Redis {

    auto setup() -> void;

  public:
    RDBFileManager dbFileManager;
    AOFFileManager aofFileManager;
    KVStore kvStore;

    explicit Redis(const RedisConfig &config)
        : dbFileManager(config.dir, config.rdb), aofFileManager(config.dir, config.aof),
          kvStore(dbFileManager.readDBFile()) {
        setup();
    }

    auto processCommand(const std::vector<std::string> &commandData, bool persistCommand = true) -> std::string;
};
