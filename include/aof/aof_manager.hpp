#include <cstdint>
#include <format>
#include <fstream>
#include <generator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "aof/aof_config.hpp"

class AOFManager {
    std::string dir;
    AOFConfig config;

    uint32_t fileIncrement = 1;

    std::optional<std::ofstream> fileHandle;

    [[nodiscard]] auto fileIncrementName() const -> std::string { return std::format(".{}.incr.aof", fileIncrement); }

    [[nodiscard]] auto dirPath() const -> std::string { return dir + "/" + config.appenddirname; }
    [[nodiscard]] auto fullFilePath() const -> std::string {
        return dirPath() + "/" + config.appendfilename + fileIncrementName();
    }

    [[nodiscard]] auto manifestFileName() const -> std::string { return config.appendfilename + ".manifest"; }
    [[nodiscard]] auto manifestFullFilePath() const -> std::string { return dirPath() + "/" + manifestFileName(); }

    [[nodiscard]] auto readManifestFileName() const -> std::string;

  public:
    AOFManager(std::string dir, AOFConfig config) : dir(std::move(dir)), config(std::move(config)) {}

    [[nodiscard]] auto getConfig() const -> const AOFConfig & { return config; }
    [[nodiscard]] auto readCommands() const -> std::generator<std::vector<std::string>>;

    auto persistCommand(const std::string &command) -> void;
    auto open() -> void;
};
