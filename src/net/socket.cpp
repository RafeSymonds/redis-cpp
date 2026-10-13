#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

#include "net/socket.hpp"

namespace {

auto makeServerSockaddr(uint16_t port) -> sockaddr_in {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return addr;
}

auto makeClientSockaddr(const std::string &hostname, uint16_t port) -> sockaddr_in {
    hostent *host = gethostbyname(hostname.c_str());
    if (host == nullptr) {
        throw std::runtime_error("unknown host " + hostname);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(port);
    return addr;
}

} // namespace

Socket::Socket(Socket &&other) noexcept
    : fd(std::exchange(other.fd, -1)), ip(other.ip), port(other.port) {}

auto Socket::operator=(Socket &&other) noexcept -> Socket & {
    if (this != &other) {
        close();
        fd = std::exchange(other.fd, -1);
        ip = other.ip;
        port = other.port;
    }
    return *this;
}

Socket::~Socket() { close(); }

auto Socket::createServer(uint16_t port) -> Socket {
    auto addr = makeServerSockaddr(port);
    Socket server(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

    if (server.fd < 0) {
        throw std::runtime_error("socket is invalid");
    }

    int opt = 1;
    if (setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("failed to set socket.");
    }

    if (bind(server.fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("failed to bind");
    }

    if (listen(server.fd, 1024) < 0) {
        throw std::runtime_error("failed to listen");
    }

    return server;
}

auto Socket::createClient(const std::string &hostname, uint16_t port) -> Socket {
    auto addr = makeClientSockaddr(hostname, port);
    Socket client(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), addr.sin_addr.s_addr, port);

    if (client.fd < 0) {
        throw std::runtime_error("socket is invalid");
    }

    int opt = 1;
    if (setsockopt(client.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("failed to set socket.");
    }

    if (connect(client.fd, reinterpret_cast<sockaddr *>(&addr), sizeof addr) < 0) {
        throw std::runtime_error("failed to connect socket");
    }

    return client;
}

auto Socket::acceptClient() -> Socket {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFd = accept(fd, reinterpret_cast<sockaddr *>(&clientAddr), &clientLen);

    if (clientFd < 0) {
        throw std::runtime_error("failed to accept client");
    }

    return {clientFd, clientAddr.sin_addr.s_addr, ntohs(clientAddr.sin_port)};
}

auto Socket::recvBytesNoWait(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_DONTWAIT); }

auto Socket::recvBytes(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_WAITALL); }

auto Socket::recvByte() -> char {
    char c;

    if (recvBytes(&c, 1) != 1) {
        throw std::runtime_error("connection closed");
    }

    return c;
}

auto Socket::recvUntil(std::function<bool(const std::string &)> donePredicate) -> std::string {
    std::string s;

    while (!donePredicate(s)) {
        s += recvByte();
    }

    return s;
}

auto Socket::sendBytes(const void *buffer, size_t size) -> int {
    const char *buf = static_cast<const char *>(buffer);

    size_t sent = 0;

    while (sent < size) {
        ssize_t n = send(fd, buf + sent, size - sent, 0);

        if (n <= 0) {
            return -1;
        }

        sent += static_cast<size_t>(n);
    }

    return 0;
}

auto Socket::sendString(std::string_view str) -> int { return sendBytes(str.data(), str.size()); }

auto Socket::close() -> int {
    if (fd < 0) {
        return 0;
    }

    int result = ::close(fd);
    fd = -1;
    return result;
}

auto Socket::getIpStr() const -> std::string {
    in_addr addr{};
    addr.s_addr = ip;

    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, buffer, sizeof(buffer));

    return buffer;
}
