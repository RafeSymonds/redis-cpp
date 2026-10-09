#pragma once

#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network_utils.hpp"

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

auto ipToString(in_addr_t ip) -> std::string {
    in_addr addr{};
    addr.s_addr = ip;

    char buffer[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &addr, buffer, sizeof(buffer));

    return buffer;
}

class Socket {
  private:
    int fd;
    in_addr_t ip;
    int port;

  public:
    Socket(int fd) : fd(fd) {}
    Socket(int fd, in_addr_t ip, int port) : fd(fd), ip(ip), port(port) {}

    static auto createServer(int port) -> Socket {
        sockaddr_in addr{};
        make_server_sockaddr(&addr, port);
        int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sockfd < 0) {
            spdlog::debug("socket invalid");
            throw "socket is invalid";
        }

        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            spdlog::debug("failed to set socket");
            throw "failed to set socket.";
        }

        if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
            spdlog::debug("failed to bind socket");
            throw "failed to bind";
        }

        if (listen(sockfd, 1024) < 0) {
            spdlog::debug("failed to listen on socket");
            throw "failed to listen";
        }

        return Socket(sockfd);
    }

    static auto createClient(const char *hostname, int port) -> Socket {
        sockaddr_in addr{};
        make_client_sockaddr(&addr, hostname, port);

        int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd < 0) {
            throw "socket is invalid";
        }

        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            spdlog::debug("failed to set socket");
            throw "failed to set socket.";
        }

        if (connect(sockfd, (sockaddr *)&addr, sizeof addr) < 0) {
            throw "failed to connect socket";
        }

        return Socket(sockfd, addr.sin_addr.s_addr, ntohs(addr.sin_port));
    }

    auto acceptClient() -> Socket {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd = accept(fd, reinterpret_cast<sockaddr *>(&clientAddr), &clientLen);

        if (clientFd < 0) {
            throw "failed to accept client";
        }

        return Socket(clientFd, clientAddr.sin_addr.s_addr, ntohs(clientAddr.sin_port));
    }

    auto recvBytesNoWait(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_DONTWAIT); }

    auto recvBytes(void *buffer, size_t size) -> ssize_t { return recv(fd, buffer, size, MSG_WAITALL); }

    auto sendBytes(const void *buffer, size_t size) -> int {
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

    auto close() -> int {
        if (fd < 0) {
            return 0;
        }

        int result = ::close(fd);
        fd = -1;
        return result;
    }

    auto getFd() const -> int { return fd; }

    auto activitySet(const fd_set &readfds) const -> bool { return FD_ISSET(fd, &readfds); }

    auto getIp() const -> in_addr_t { return ip; }
    auto getIpStr() const -> std::string { return ipToString(ip); }
    auto getPort() const -> uint16_t { return port; }
};
