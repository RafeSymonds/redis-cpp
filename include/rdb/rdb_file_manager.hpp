#include <filesystem>
#include <string>
#include <utility>

#include "kv_store/kv_store.hpp"

struct RDBConfig {
    std::string dir;
    std::string dbfilename;

    RDBConfig() : dir(std::filesystem::current_path()) {}
    RDBConfig(std::string dir, std::string dbfilename) : dir(std::move(dir)), dbfilename(std::move(dbfilename)) {}
};

class RDBFileManager {

    RDBConfig config;

  public:
    explicit RDBFileManager(RDBConfig config) : config(std::move(config)) {}

    auto readDBFile() -> KVStore;

    [[nodiscard]] auto getConfig() const -> const RDBConfig & { return config; }
};
