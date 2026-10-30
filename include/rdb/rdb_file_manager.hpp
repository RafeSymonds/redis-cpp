#include <string>
#include <utility>

#include "kv_store/kv_store.hpp"
#include "rdb/rdb_config.hpp"

class RDBFileManager {

    std::string dir;
    RDBConfig config;

  public:
    RDBFileManager(std::string dir, RDBConfig config) : dir(std::move(dir)), config(std::move(config)) {}

    auto readDBFile() -> KVStore;

    [[nodiscard]] auto getDir() const -> const std::string & { return dir; }
    [[nodiscard]] auto getConfig() const -> const RDBConfig & { return config; }

    [[nodiscard]] static auto writeDBFile(KVStore &kvStore) -> std::string;
};
