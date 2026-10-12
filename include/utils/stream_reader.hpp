#pragma once

#include <functional>
#include <istream>
#include <stdexcept>
#include <string>
#include <sys/types.h>

// Adapts a std::istream (file, string stream) to the same read interface as Socket.
class StreamReader {
    std::istream &stream;

  public:
    explicit StreamReader(std::istream &stream) : stream(stream) {}

    auto recvBytes(void *buffer, size_t size) -> ssize_t {
        stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(size));
        return stream.gcount();
    }

    auto recvByte() -> char {
        char c;

        if (recvBytes(&c, 1) != 1) {
            throw std::runtime_error("unexpected end of stream");
        }

        return c;
    }

    auto recvUntil(std::function<bool(const std::string &)> donePredicate) -> std::string {
        std::string s;

        while (!donePredicate(s)) {
            s += recvByte();
        }

        return s;
    }

    auto atEnd() -> bool { return stream.peek() == std::char_traits<char>::eof(); }
};
