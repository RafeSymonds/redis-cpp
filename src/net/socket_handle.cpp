#include <stdexcept>

#include "net/network_utils.hpp"
#include "net/socket_handle.hpp"

auto Socket::createServer(uint32_t port) -> Socket {
    sockaddr_in addr{};
    make_server_sockaddr(&addr, port);
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        throw std::runtime_error("socket is invalid");
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("failed to set socket.");
    }

    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("failed to bind");
    }

    if (listen(sockfd, 1024) < 0) {
        throw std::runtime_error("failed to listen");
    }

    return Socket(sockfd);
}

auto Socket::createClient(const char *hostname, uint32_t port) -> Socket {
    sockaddr_in addr{};
    make_client_sockaddr(&addr, hostname, port);

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        throw std::runtime_error("socket is invalid");
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("failed to set socket.");
    }

    if (connect(sockfd, (sockaddr *)&addr, sizeof addr) < 0) {
        throw std::runtime_error("failed to connect socket");
    }

    return Socket(sockfd, addr.sin_addr.s_addr, ntohs(addr.sin_port));
}

auto Socket::acceptClient() -> Socket {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    int clientFd = accept(fd, reinterpret_cast<sockaddr *>(&clientAddr), &clientLen);

    if (clientFd < 0) {
        throw std::runtime_error("failed to accept client");
    }

    return Socket(clientFd, clientAddr.sin_addr.s_addr, ntohs(clientAddr.sin_port));
}

auto Socket::recvBytesNoWait(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_DONTWAIT); }

auto Socket::recvBytes(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_WAITALL); }

auto Socket::recvByte() -> char {
    char c;

    recvBytes(&c, 1);

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

        if (n < 0) {
            return -1;
        }

        if (n == 0) {
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
