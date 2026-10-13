#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace resp {

constexpr std::string_view NULL_BULK_STRING = "$-1\r\n";
constexpr std::string_view OK = "+OK\r\n";

auto simpleString(std::string_view s) -> std::string;

auto bulkString(std::string_view s) -> std::string;

auto array(const std::vector<std::string> &elements) -> std::string;

} // namespace resp
