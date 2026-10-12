#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <stdexcept>

#include "rdb/rdb.hpp"
#include "rdb/rdb_file_manager.hpp"
#include "utils/time_utils.hpp"

namespace {

auto readBytes(std::ifstream &file, std::size_t count) -> std::string {
    std::string buffer(count, '\0');
    file.read(buffer.data(), static_cast<std::streamsize>(count));

    if (static_cast<std::size_t>(file.gcount()) != count) {
        throw std::runtime_error(
            std::format("unexpected end of RDB file: wanted {} bytes, got {}", count, file.gcount()));
    }

    return buffer;
}

auto readByte(std::ifstream &file) -> std::uint8_t { return static_cast<std::uint8_t>(readBytes(file, 1)[0]); }

// Integers stored as raw little-endian bytes (expiry timestamps, special string ints).
auto readLittleEndian(std::ifstream &file, std::size_t size) -> std::uint64_t {
    auto bytes = readBytes(file, size);

    std::uint64_t value = 0;
    for (std::size_t i = 0; i < size; i++) {
        value |= static_cast<std::uint64_t>(static_cast<std::uint8_t>(bytes[i])) << (8 * i);
    }

    return value;
}

struct Length {
    std::uint32_t value;
    bool isSpecial; // true means value is a special string encoding, not a length
};

auto readLength(std::ifstream &file) -> Length {
    auto first = readByte(file);
    std::uint32_t low = first & RDB::LENGTH_VALUE_MASK;

    switch (first & RDB::LENGTH_TYPE_MASK) {
        case RDB::LENGTH_6_BIT:
            return {.value = low, .isSpecial = false};
        case RDB::LENGTH_14_BIT:
            return {.value = (low << 8) | readByte(file), .isSpecial = false};
        case RDB::LENGTH_32_BIT: {
            // 32-bit lengths are big-endian
            auto bytes = readBytes(file, 4);
            std::uint32_t value = 0;
            for (char b : bytes) {
                value = (value << 8) | static_cast<std::uint8_t>(b);
            }
            return {.value = value, .isSpecial = false};
        }
        default:
            return {.value = low, .isSpecial = true};
    }
}

auto readString(std::ifstream &file) -> std::string {
    auto [value, isSpecial] = readLength(file);

    if (!isSpecial) {
        return readBytes(file, value);
    }

    switch (value) {
        case RDB::STRING_INT8:
            return std::to_string(static_cast<std::int8_t>(readLittleEndian(file, 1)));
        case RDB::STRING_INT16:
            return std::to_string(static_cast<std::int16_t>(readLittleEndian(file, 2)));
        case RDB::STRING_INT32:
            return std::to_string(static_cast<std::int32_t>(readLittleEndian(file, 4)));
        default:
            throw std::runtime_error(std::format("unsupported RDB string encoding {}", value));
    }
}

} // namespace

auto RDBFileManager::readDBFile() -> KVStore {
    if (dbFilename == "") {
        return {};
    }

    std::ifstream file(dbFolderPath + "/" + dbFilename, std::ios::binary);
    if (!file.is_open()) {
        std::println("No DB file found.");
        std::cout.flush();
        return {};
    }

    auto header = readBytes(file, RDB::HEADER.size());

    if (header != RDB::HEADER) {
        throw std::runtime_error(std::format("invalid RDB header {}", header));
    }

    KVStore kvStore;
    auto expireTime = TimePoint::max();

    while (true) {
        auto opcode = readByte(file);

        switch (opcode) {
            case RDB::METADATA:
                readString(file); // name
                readString(file); // value
                break;
            case RDB::DATABASE:
                readLength(file); // database index
                break;
            case RDB::HASH_TABLE_SIZE:
                readLength(file); // key count
                readLength(file); // expiring key count
                break;
            case RDB::EXPIRE_SECONDS:
                expireTime = TimePoint(std::chrono::seconds(readLittleEndian(file, RDB::EXPIRE_SECONDS_SIZE)));
                break;
            case RDB::EXPIRE_MS:
                expireTime = TimePoint(std::chrono::milliseconds(readLittleEndian(file, RDB::EXPIRE_MS_SIZE)));
                break;
            case RDB::STRING: {
                auto key = readString(file);
                auto value = readString(file);
                kvStore.updatedKeyValueStore(key, value, expireTime);
                expireTime = TimePoint::max();
                break;
            }
            case RDB::END_OF_FILE:
                return kvStore;
            default:
                throw std::runtime_error(std::format("unsupported RDB opcode 0x{:02X}", opcode));
        }
    }
}
