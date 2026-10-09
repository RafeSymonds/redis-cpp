#include "redis_command.hpp"

auto parseCommand(const std::string &s) -> Command {
    if (s == "PING")
        return Command::PING;

    if (s == "ECHO")
        return Command::ECHO;

    if (s == "GET")
        return Command::GET;

    if (s == "SET")
        return Command::SET;

    return Command::UNKNOWN;
}
