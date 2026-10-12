#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <stdexcept>

#include <aof/aof_file_manager.hpp>
#include <utils/string_utils.hpp>

auto AOFFileManager::readCommands() const -> std::generator<std::vector<std::string>> {
    if (!config.appendonly || !std::filesystem::exists(manifestFullFilePath())) {
        co_return;
    }

    auto filePath = dirPath() + "/" + readManifestFileName();

    std::println("replaying append only file {}", filePath);
    std::cout.flush();

    std::ifstream file(filePath);
    StreamReader reader(file);

    while (!reader.atEnd()) {
        co_yield parseRedisCommand(reader);
    }
}

auto AOFFileManager::open() -> void {
    if (!config.appendonly) {
        return;
    }

    if (std::filesystem::exists(manifestFullFilePath())) {
        fileHandle.emplace(dirPath() + "/" + readManifestFileName(), std::ios::app);
        return;
    }

    std::filesystem::create_directories(dirPath());
    std::ofstream file(fullFilePath());

    if (!file) {
        throw std::runtime_error("failed to create file");
    }

    std::ofstream manifestFile(manifestFullFilePath());

    if (!manifestFile) {
        throw std::runtime_error("failed to create file");
    }

    manifestFile << "file " << config.appendfilename << ".1.incr.aof seq 1 type i";

    fileHandle = std::move(file);
}

auto AOFFileManager::readManifestFileName() const -> std::string {
    std::ifstream manifest(manifestFullFilePath());
    std::string keyword;
    std::string filename;

    if (!(manifest >> keyword >> filename) || keyword != "file") {
        throw std::runtime_error("invalid manifest");
    }

    return filename;
}

auto AOFFileManager::persistCommand(const std::string &command) -> void {
    if (!config.appendonly) {
        return;
    }

    *fileHandle << command;

    switch (config.appendfsync) {
        case AppendFSync::ALWAYS:
            fileHandle->flush();
        case AppendFSync::EVERYSEC:
        case AppendFSync::NO:
            return;
    }
}
