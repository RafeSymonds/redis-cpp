#include <ranges>

#include "utils/string_utils.hpp"

auto convertToSimpleString(const std::string &s) -> std::string { return '+' + s + "\r\n"; }

auto endsWithCRLF(const std::string &s) -> bool { return s.ends_with("\r\n"); }

auto convertToRESPBulkString(const std::string &s) -> std::string {
    return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
}

auto convertToRESPArray(const std::vector<std::string> elements) -> std::string {
    auto output = "*" + std::to_string(elements.size()) + "\r\n";

    for (auto &ele : elements) {
        output += convertToRESPBulkString(ele);
    }

    return output;
}

auto split(const std::string &str, char delim) -> std::vector<std::string> {
    std::vector<std::string> result;

    for (auto part : str | std::views::split(delim)) {
        result.emplace_back(part.begin(), part.end());
    }

    return result;
}
