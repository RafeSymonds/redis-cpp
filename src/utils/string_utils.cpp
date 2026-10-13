#include <ranges>

#include "utils/string_utils.hpp"

auto split(const std::string &str, char delim) -> std::vector<std::string> {
    std::vector<std::string> result;

    for (auto part : str | std::views::split(delim)) {
        result.emplace_back(part.begin(), part.end());
    }

    return result;
}
