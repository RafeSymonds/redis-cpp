#include <string>
#include <utility>

#include "kv_store/kv_store.hpp"

struct RDBConfig {
    std::string dbfilename;
};

class RDBFileManager {

    std::string dir;
    RDBConfig config;

  public:
    RDBFileManager(std::string dir, RDBConfig config) : dir(std::move(dir)), config(std::move(config)) {}

    auto readDBFile() -> KVStore;

    [[nodiscard]] auto getDir() const -> const std::string & { return dir; }
    [[nodiscard]] auto getConfig() const -> const RDBConfig & { return config; }
};
