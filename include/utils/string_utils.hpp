#pragma once

#include <string>
#include <vector>

#include "net/socket_handle.hpp"

auto createSimpleString(const std::string &s) -> std::string;

auto endsWithCRLF(const std::string &s) -> bool;

auto parseRedisCommandFromNetwork(Socket &socket) -> std::vector<std::string>;

auto convertToRESPBulkString(const std::string &s) -> std::string;
