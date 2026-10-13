#include <format>
#include <stdexcept>

#include "config/config.hpp"

auto parseAppendFSync(const std::string &s) -> AppendFSync {
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

auto toString(AppendFSync appendfsync) -> std::string {
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

auto configGet(const RedisConfig &config, std::string_view name) -> std::optional<std::string> {
    if (name == "dir") {
        return config.dir;
    }
    if (name == "dbfilename") {
        return config.rdb.dbfilename;
    }
    if (name == "appendonly") {
        return config.aof.appendonly ? "yes" : "no";
    }
    if (name == "appenddirname") {
        return config.aof.appenddirname;
    }
    if (name == "appendfilename") {
        return config.aof.appendfilename;
    }
    if (name == "appendfsync") {
        return toString(config.aof.appendfsync);
    }

    return std::nullopt;
}
