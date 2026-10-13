#include <format>

#include "resp/encode.hpp"

namespace resp {

auto simpleString(std::string_view s) -> std::string { return std::format("+{}\r\n", s); }

auto bulkString(std::string_view s) -> std::string { return std::format("${}\r\n{}\r\n", s.size(), s); }

auto integer(int64_t value) -> std::string { return std::format(":{}\r\n", value); }

auto array(const std::vector<std::string> &elements) -> std::string {
    auto output = std::format("*{}\r\n", elements.size());

    for (const auto &element : elements) {
        output += bulkString(element);
    }

    return output;
}

} // namespace resp
