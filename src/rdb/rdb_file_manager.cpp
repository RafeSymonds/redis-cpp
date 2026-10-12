#include <cassert>
#include <fstream>
#include <iostream>
#include <print>

#include "rdb/rdb.hpp"
#include "rdb/rdb_file_manager.hpp"
#include "utils/time_utils.hpp"

auto RDBFileManager::readDBFile() -> KVStore {
    if (dbFilename == "") {
        return {};
    }

    std::ifstream file(dbFolderPath + "/" + dbFilename);
    if (!file.is_open()) {
        std::println("No DB file found.");
        std::cout.flush();
        return {};
    }

    std::string header;

    file >> header;

    if (header != "REDIS0011") {
        throw std::runtime_error(
            std::format("invalid file since header is wrong {} with size={}", header, header.size()));
    }

    uint8_t byte = 0;

    do {
        file >> byte;

    } while (byte != RDB::DATABASE);

    uint8_t databaseIndex = 0;
    file >> databaseIndex;

    // read hash table size
    file >> byte;

    uint8_t numKeys = 0;
    uint8_t numExpires = 0;
    file >> numKeys >> numExpires;

    KVStore kvStore;

    std::println("number of keys reading from disk {}", numKeys);
    std::cout.flush();

    while (numKeys-- > 0) {
        file.read(reinterpret_cast<char *>(&byte), 1);

        TimePoint expireTime = TimePoint::max();

        if (byte == RDB::EXPIRE_SECONDS) {
            uint32_t time = 0;
            file >> time;

            expireTime = Clock::now() + std::chrono::seconds(time);
        }
        else if (byte == RDB::EXPIRE_MS) {
            uint64_t time = 0;
            file >> time;

            expireTime = Clock::now() + std::chrono::milliseconds(time);
        }

        file.read(reinterpret_cast<char *>(&byte), 1);

        if (byte != 0) {
            throw std::runtime_error("unsupported type");
        }

        std::string key;
        std::string value;
        file >> key >> value;

        kvStore.updatedKeyValueStore(key, value, expireTime);
    }

    return std::move(kvStore);
}
