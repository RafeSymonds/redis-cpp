#include <cstdint>
#include <string_view>

namespace rdb {

static constexpr std::string_view HEADER = "REDIS0011";

// Section / opcode markers
static constexpr std::uint8_t METADATA = 0xFA;
static constexpr std::uint8_t DATABASE = 0xFE;
static constexpr std::uint8_t HASH_TABLE_SIZE = 0xFB;
static constexpr std::uint8_t EXPIRE_MS = 0xFC;
static constexpr std::uint8_t EXPIRE_SECONDS = 0xFD;
static constexpr std::uint8_t END_OF_FILE = 0xFF;

// Value types
static constexpr std::uint8_t STRING = 0x00;

// Length encoding
static constexpr std::uint8_t LENGTH_TYPE_MASK = 0b11000000;
static constexpr std::uint8_t LENGTH_VALUE_MASK = 0b00111111;

static constexpr std::uint8_t LENGTH_6_BIT = 0b00000000;
static constexpr std::uint8_t LENGTH_14_BIT = 0b01000000;
static constexpr std::uint8_t LENGTH_32_BIT = 0b10000000;
static constexpr std::uint8_t SPECIAL_STRING = 0b11000000;

// Special string encodings
static constexpr std::uint8_t STRING_INT8 = 0;
static constexpr std::uint8_t STRING_INT16 = 1;
static constexpr std::uint8_t STRING_INT32 = 2;
static constexpr std::uint8_t STRING_LZF = 3;

// Fixed byte sizes
static constexpr std::size_t EXPIRE_SECONDS_SIZE = 4;
static constexpr std::size_t EXPIRE_MS_SIZE = 8;
static constexpr std::size_t CHECKSUM_SIZE = 8;

} // namespace rdb
