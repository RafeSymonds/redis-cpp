#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <aof/aof_file_manager.hpp>

auto AOFFileManager::setup() -> void {
    if (!config.appendonly) {
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
}
