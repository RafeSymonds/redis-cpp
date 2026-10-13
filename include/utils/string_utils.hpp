#pragma once

#include <concepts>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "utils/stream_reader.hpp"

auto convertToSimpleString(const std::string &s) -> std::string;

auto endsWithCRLF(const std::string &s) -> bool;

// Anything with Socket's read interface: Socket, StreamReader.
template <typename T>
concept RESPReader = requires(T reader, void *buffer, size_t size, std::function<bool(const std::string &)> pred) {
    { reader.recvByte() } -> std::convertible_to<char>;
    reader.recvBytes(buffer, size);
    { reader.recvUntil(pred) } -> std::convertible_to<std::string>;
};

template <RESPReader Reader> auto parseRedisCommand(Reader &reader) -> std::vector<std::string> {
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
                    auto command = parseRedisCommand(reader);

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

inline auto parseRedisCommand(const std::string &s) -> std::vector<std::string> {
    std::istringstream stream(s);
    StreamReader reader(stream);
    return parseRedisCommand(reader);
}

auto convertToRESPBulkString(const std::string &s) -> std::string;

auto convertToRESPArray(const std::vector<std::string> elements) -> std::string;

constexpr std::string NULL_BULK_STRING = "$-1\r\n";
constexpr std::string OK_SIMPLE_STRING = "+OK\r\n";

auto split(const std::string &str, char delim) -> std::vector<std::string>;
