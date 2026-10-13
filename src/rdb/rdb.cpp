#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <stdexcept>

#include "rdb/rdb.hpp"
#include "rdb/rdb_format.hpp"
#include "utils/time_utils.hpp"

namespace rdb {

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
    std::uint32_t low = first & LENGTH_VALUE_MASK;

    switch (first & LENGTH_TYPE_MASK) {
        case LENGTH_6_BIT:
            return {.value = low, .isSpecial = false};
        case LENGTH_14_BIT:
            return {.value = (low << 8) | readByte(file), .isSpecial = false};
        case LENGTH_32_BIT: {
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
        case STRING_INT8:
            return std::to_string(static_cast<std::int8_t>(readLittleEndian(file, 1)));
        case STRING_INT16:
            return std::to_string(static_cast<std::int16_t>(readLittleEndian(file, 2)));
        case STRING_INT32:
            return std::to_string(static_cast<std::int32_t>(readLittleEndian(file, 4)));
        default:
            throw std::runtime_error(std::format("unsupported RDB string encoding {}", value));
    }
}

auto writeLittleEndian(std::string &output, std::uint64_t value, std::size_t size) -> void {
    for (std::size_t i = 0; i < size; i++) {
        output.push_back(static_cast<char>((value >> (8 * i)) & 0xFF));
    }
}

auto writeLength(std::string &output, std::uint32_t length) -> void {
    if (length < (1U << 6)) {
        output.push_back(static_cast<char>(LENGTH_6_BIT | length));
    }
    else if (length < (1U << 14)) {
        output.push_back(static_cast<char>(LENGTH_14_BIT | (length >> 8)));
        output.push_back(static_cast<char>(length & 0xFF));
    }
    else {
        // 32-bit lengths are big-endian
        output.push_back(static_cast<char>(LENGTH_32_BIT));
        for (int shift = 24; shift >= 0; shift -= 8) {
            output.push_back(static_cast<char>((length >> shift) & 0xFF));
        }
    }
}

auto writeString(std::string &output, std::string_view s) -> void {
    writeLength(output, static_cast<std::uint32_t>(s.size()));
    output.append(s);
}

} // namespace

auto load(const std::string &path, KVStore &store) -> void {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::println("No DB file found.");
        std::cout.flush();
        return;
    }

    auto header = readBytes(file, HEADER.size());

    if (header != HEADER) {
        throw std::runtime_error(std::format("invalid RDB header {}", header));
    }

    auto expireTime = TimePoint::max();

    while (true) {
        auto opcode = readByte(file);

        switch (opcode) {
            case METADATA:
                readString(file); // name
                readString(file); // value
                break;
            case DATABASE:
                readLength(file); // database index
                break;
            case HASH_TABLE_SIZE:
                readLength(file); // key count
                readLength(file); // expiring key count
                break;
            case EXPIRE_SECONDS:
                expireTime = TimePoint(std::chrono::seconds(readLittleEndian(file, EXPIRE_SECONDS_SIZE)));
                break;
            case EXPIRE_MS:
                expireTime = TimePoint(std::chrono::milliseconds(readLittleEndian(file, EXPIRE_MS_SIZE)));
                break;
            case STRING: {
                auto key = readString(file);
                auto value = readString(file);
                store.set(key, value, expireTime);
                expireTime = TimePoint::max();
                break;
            }
            case END_OF_FILE:
                return;
            default:
                throw std::runtime_error(std::format("unsupported RDB opcode 0x{:02X}", opcode));
        }
    }
}

auto serialize(const KVStore &store) -> std::string {
    std::string output(HEADER);

    // metadata
    output.push_back(static_cast<char>(METADATA));
    writeString(output, "redis-ver");
    writeString(output, "7.2.0");

    auto currentTime = Clock::now();
    auto items = store.items();
    std::erase_if(items, [&](const auto &entry) { return entry.second.IsExpired(currentTime); });

    std::uint32_t expiringKeys = 0;
    for (const auto &[key, item] : items) {
        if (item.expireTime != TimePoint::max()) {
            expiringKeys++;
        }
    }

    // database 0
    output.push_back(static_cast<char>(DATABASE));
    writeLength(output, 0);
    output.push_back(static_cast<char>(HASH_TABLE_SIZE));
    writeLength(output, static_cast<std::uint32_t>(items.size()));
    writeLength(output, expiringKeys);

    for (const auto &[key, item] : items) {
        if (item.expireTime != TimePoint::max()) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(item.expireTime.time_since_epoch());
            output.push_back(static_cast<char>(EXPIRE_MS));
            writeLittleEndian(output, static_cast<std::uint64_t>(ms.count()), EXPIRE_MS_SIZE);
        }

        output.push_back(static_cast<char>(STRING));
        writeString(output, key);
        writeString(output, item.value);
    }

    output.push_back(static_cast<char>(END_OF_FILE));

    // zero checksum tells readers to skip verification
    output.append(CHECKSUM_SIZE, '\0');

    return output;
}

} // namespace rdb
