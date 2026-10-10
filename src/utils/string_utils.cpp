#include "utils/string_utils.hpp"
#include "net/socket_handle.hpp"

auto convertToSimpleString(const std::string &s) -> std::string { return '+' + s + "\r\n"; }

auto endsWithCRLF(const std::string &s) -> bool { return s.ends_with("\r\n"); }

auto parseRedisCommandFromNetwork(Socket &socket) -> std::vector<std::string> {
    char type = socket.recvByte();

    switch (type) {
        case '*':
            // read integer until \r\n
            // parse that many RESP values
            {
                auto value = socket.recvUntil(endsWithCRLF);
                value.resize(value.size() - 2);

                std::vector<std::string> output;

                auto numToParse = std::stoll(value);

                while (numToParse-- > 0) {
                    auto command = parseRedisCommandFromNetwork(socket);

                    if (command.size() != 1) {
                        throw "unexpected command";
                    }
                    output.push_back(std::move(command[0]));
                }

                return output;
            }

        case '$': {
            // read length until \r\n
            // then read exactly length bytes
            // then consume \r\n
            auto length = std::stoll(socket.recvUntil(endsWithCRLF));

            std::string buffer(length, '\0');

            auto n = socket.recvBytes(buffer.data(), length);

            char junk[2];
            socket.recvBytes(junk, 2);

            if (n != length) {
                throw "did not receive what we expected";
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
                auto value = socket.recvUntil(endsWithCRLF);
                value.resize(value.size() - 2);
                return {std::move(value)};
            }
    }

    throw "command did not match";
}

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
