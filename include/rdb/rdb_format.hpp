#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rdb {

constexpr std::string_view HEADER = "REDIS0011";

// Section / opcode markers
constexpr std::uint8_t METADATA = 0xFA;
constexpr std::uint8_t DATABASE = 0xFE;
constexpr std::uint8_t HASH_TABLE_SIZE = 0xFB;
constexpr std::uint8_t EXPIRE_MS = 0xFC;
constexpr std::uint8_t EXPIRE_SECONDS = 0xFD;
constexpr std::uint8_t END_OF_FILE = 0xFF;

// Value types
constexpr std::uint8_t STRING = 0x00;

// Length encoding
constexpr std::uint8_t LENGTH_TYPE_MASK = 0b11000000;
constexpr std::uint8_t LENGTH_VALUE_MASK = 0b00111111;

constexpr std::uint8_t LENGTH_6_BIT = 0b00000000;
constexpr std::uint8_t LENGTH_14_BIT = 0b01000000;
constexpr std::uint8_t LENGTH_32_BIT = 0b10000000;
constexpr std::uint8_t SPECIAL_STRING = 0b11000000;

// Special string encodings
constexpr std::uint8_t STRING_INT8 = 0;
constexpr std::uint8_t STRING_INT16 = 1;
constexpr std::uint8_t STRING_INT32 = 2;
constexpr std::uint8_t STRING_LZF = 3;

// Fixed byte sizes
constexpr std::size_t EXPIRE_SECONDS_SIZE = 4;
constexpr std::size_t EXPIRE_MS_SIZE = 8;
constexpr std::size_t CHECKSUM_SIZE = 8;

} // namespace rdb
