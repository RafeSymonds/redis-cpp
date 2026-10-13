#pragma once

#include <cstdint>
#include <functional>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/select.h>
#include <sys/types.h>

// Owns a socket file descriptor. Move-only; the destructor closes the fd.
class Socket {
  private:
    int fd = -1;
    in_addr_t ip{};
    uint16_t port{};

  public:
    explicit Socket(int fd) : fd(fd) {}
    Socket(int fd, in_addr_t ip, uint16_t port) : fd(fd), ip(ip), port(port) {}

    Socket(Socket &&other) noexcept;
    auto operator=(Socket &&other) noexcept -> Socket &;

    Socket(const Socket &) = delete;
    auto operator=(const Socket &) -> Socket & = delete;

    ~Socket();

    static auto createServer(uint16_t port) -> Socket;

    static auto createClient(const std::string &hostname, uint16_t port) -> Socket;

    auto acceptClient() -> Socket;

    auto recvBytesNoWait(void *buffer, size_t size) -> ssize_t;

    auto recvBytes(void *buffer, size_t size) -> ssize_t;

    // Throws when the peer closed the connection or recv failed.
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
    [[nodiscard]] auto getIpStr() const -> std::string;
    [[nodiscard]] auto getPort() const -> uint16_t { return port; }
};
