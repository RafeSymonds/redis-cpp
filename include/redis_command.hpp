#pragma once

#include <string>

constexpr std::string PING_COMAMND = "PING";
constexpr std::string ECHO_COMAMND = "ECHO";
constexpr std::string GET_COMAMND = "GET";
constexpr std::string SET_COMAMND = "SET";

enum class Command { PING, ECHO, GET, SET, CONFIG, UNKNOWN };

auto parseCommand(const std::string &s) -> Command;
