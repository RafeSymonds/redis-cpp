#include <string>

#include "kv_store/kv_store.hpp"
#include "rdb/rdb_file_manager.hpp"

class Redis {
  public:
    RDBFileManager fileManager;
    KVStore kvStore;

    Redis(const std::string &dbFolderPath, const std::string &dbFilename)
        : fileManager(dbFolderPath, dbFilename), kvStore(fileManager.readDBFile()) {}
};
