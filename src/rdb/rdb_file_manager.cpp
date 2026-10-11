#include <cassert>
#include <fstream>

#include "rdb/rdb.hpp"
#include "rdb/rdb_file_manager.hpp"
#include "utils/time_utils.hpp"

auto RDBFileManager::readDBFile() -> KVStore {
    std::ifstream file(dbFolderPath + dbFilename, std::ios::binary);
    if (!file.is_open()) {
        return KVStore{};
    }

    std::string header(RDB::HEADER.size(), '\0');

    file >> header;

    if (header != "REDIS0011") {
        throw "invalid file";
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
            throw "unsupported type";
        }

        std::string key;
        std::string value;
        file >> key >> value;

        kvStore.updatedKeyValueStore(key, value, expireTime);
    }

    return std::move(kvStore);
}
