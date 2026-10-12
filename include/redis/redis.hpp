#include "aof/aof_file_manager.hpp"
#include "kv_store/kv_store.hpp"
#include "rdb/rdb_file_manager.hpp"

struct RedisConfig {
    RDBConfig rdb;
    AOFConfig aof;
};

class Redis {
  public:
    RDBFileManager dbFileManager;
    AOFFileManager aofFileManager;
    KVStore kvStore;

    explicit Redis(const RedisConfig &config)
        : dbFileManager(config.rdb), aofFileManager(config.aof), kvStore(dbFileManager.readDBFile()) {}
};
