#pragma once

#include <concepts>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "resp/stream_reader.hpp"

namespace resp {

inline auto endsWithCRLF(const std::string &s) -> bool { return s.ends_with("\r\n"); }

// Anything with Socket's read interface: Socket, StreamReader.
template <typename T>
concept Reader = requires(T reader, void *buffer, size_t size, std::function<bool(const std::string &)> pred) {
    { reader.recvByte() } -> std::convertible_to<char>;
    reader.recvBytes(buffer, size);
    { reader.recvUntil(pred) } -> std::convertible_to<std::string>;
};

// Reads one RESP value. Arrays come back as their elements, scalars as a single element.
template <Reader R> auto parse(R &reader) -> std::vector<std::string> {
    char type = reader.recvByte();

    switch (type) {
        case '*':
            // read integer until \r\n
            // parse that many RESP values
            {
                auto value = reader.recvUntil(endsWithCRLF);
                value.resize(value.size() - 2);

                std::vector<std::string> output;

                auto numToParse = std::stoll(value);

                while (numToParse-- > 0) {
                    auto command = parse(reader);

                    if (command.size() != 1) {
                        throw std::runtime_error("unexpected command");
                    }
                    output.push_back(std::move(command[0]));
                }

                return output;
            }

        case '$': {
            // read length until \r\n
            // then read exactly length bytes
            // then consume \r\n
            auto length = std::stoll(reader.recvUntil(endsWithCRLF));

            std::string buffer(length, '\0');

            auto n = reader.recvBytes(buffer.data(), length);

            char junk[2];
            reader.recvBytes(junk, 2);

            if (n != length) {
                throw std::runtime_error("did not receive what we expected");
            }
            return {std::move(buffer)};
        }

        case '+':
            // simple string, read until \r\n
        case '-':
            // error, read until \r\n
        case ':':
            // integer, read until \r\n
            {
                auto value = reader.recvUntil(endsWithCRLF);
                value.resize(value.size() - 2);
                return {std::move(value)};
            }
    }

    throw std::runtime_error("command did not match");
}

inline auto parse(const std::string &s) -> std::vector<std::string> {
    std::istringstream stream(s);
    StreamReader reader(stream);
    return parse(reader);
}

} // namespace resp
