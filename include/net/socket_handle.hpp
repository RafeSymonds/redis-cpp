#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace {
auto ipToString(in_addr_t ip) -> std::string {
    in_addr addr{};
    addr.s_addr = ip;

    char buffer[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &addr, buffer, sizeof(buffer));

    return buffer;
}

} // namespace

class Socket {
  private:
    int fd;
    in_addr_t ip{};
    uint32_t port{};

  public:
    Socket(int fd) : fd(fd) {}
    Socket(int fd, in_addr_t ip, uint32_t port) : fd(fd), ip(ip), port(port) {}

    static auto createServer(uint32_t port) -> Socket;

    static auto createClient(const std::string &hostname, uint32_t port) -> Socket;

    auto acceptClient() -> Socket;

    auto recvBytesNoWait(void *buffer, size_t size) -> ssize_t;

    auto recvBytes(void *buffer, size_t size) -> ssize_t;

    auto recvByte() -> char;

    auto recvUntil(std::function<bool(const std::string &)> donePredicate) -> std::string;

    auto sendBytes(const void *buffer, size_t size) -> int;

    template <typename T> auto recvObject(T &obj) -> ssize_t { return recvBytes(&obj, sizeof(T)); }

    template <typename T> auto sendObject(const T &obj) -> int { return sendBytes(&obj, sizeof(T)); }

    auto sendString(std::string_view str) -> int;

    auto close() -> int;

    [[nodiscard]] auto getFd() const -> int { return fd; }

    [[nodiscard]] auto activitySet(const fd_set &readfds) const -> bool { return FD_ISSET(fd, &readfds); }

    [[nodiscard]] auto getIp() const -> in_addr_t { return ip; }
    [[nodiscard]] auto getIpStr() const -> std::string { return ipToString(ip); }
    [[nodiscard]] auto getPort() const -> uint16_t { return port; }
};
