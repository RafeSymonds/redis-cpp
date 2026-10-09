#pragma once

#include <string>
#include <vector>

#include "net/socket_handle.hpp"

auto convertToSimpleString(const std::string &s) -> std::string;

auto endsWithCRLF(const std::string &s) -> bool;

auto parseRedisCommandFromNetwork(Socket &socket) -> std::vector<std::string>;

auto convertToRESPBulkString(const std::string &s) -> std::string;

constexpr std::string NULL_BULK_STRING = "$-1\r\n";
