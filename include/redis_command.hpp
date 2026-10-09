#pragma once

#include <string>
#include <variant>

struct PingCommand {};

struct GetCommand {
    std::string key;
};

struct SetCommand {
    std::string key;
    std::string value;
};

using RedisCommand = std::variant<PingCommand, GetCommand, SetCommand>;
