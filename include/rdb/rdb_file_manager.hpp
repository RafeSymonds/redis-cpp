#include <string>

#include "kv_store/kv_store.hpp"

class RDBFileManager {

    std::string dbFolderPath;
    std::string dbFilename;

  public:
    RDBFileManager(const std::string &dbFolderPath, const std::string &dbFilename)
        : dbFolderPath(dbFolderPath), dbFilename(dbFilename) {}

    auto readDBFile() -> KVStore;

    auto getDBFolderPath() const -> std::string { return dbFolderPath; }
    auto getDBFilename() const -> std::string { return dbFilename; }
};
