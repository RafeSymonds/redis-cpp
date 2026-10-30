#include <cstdint>
#include <format>
#include <string>

enum class AppendFSync : uint8_t { ALWAYS, EVERYSEC, NO };

inline auto parseAppendFSync(const std::string &s) -> AppendFSync {
    if (s == "always") {
        return AppendFSync::ALWAYS;
    }

    if (s == "everysec") {
        return AppendFSync::EVERYSEC;
    }

    if (s == "no") {
        return AppendFSync::NO;
    }

    throw std::invalid_argument(std::format("invalid appendfsync value: {}", s));
}

inline auto toString(AppendFSync appendfsync) -> std::string {
    switch (appendfsync) {
        case AppendFSync::ALWAYS:
            return "always";
        case AppendFSync::EVERYSEC:
            return "everysec";
        case AppendFSync::NO:
            return "no";
    }

    throw std::invalid_argument("invalid appendfsync value");
}
