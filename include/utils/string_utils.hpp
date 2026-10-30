#pragma once

#include <string>
#include <vector>

namespace utils {
auto split(const std::string &str, char delim) -> std::vector<std::string>;
}
