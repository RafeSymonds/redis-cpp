#pragma once

#include <cstdint>
#include <format>
#include <fstream>
#include <generator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "config/config.hpp"

class AOF {
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
    AOF(std::string dir, AOFConfig config) : dir(std::move(dir)), config(std::move(config)) {}

    // Commands recorded in the existing AOF, oldest first. Empty when AOF is off or absent.
    [[nodiscard]] auto replay() const -> std::generator<std::vector<std::string>>;

    // Opens the AOF for appending, creating the directory, file and manifest if needed.
    auto open() -> void;

    // Appends an already RESP-encoded command.
    auto append(const std::string &command) -> void;
};
