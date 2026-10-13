#include <cstdint>

#include "net/socket_handle.hpp"

class ReplicationClient {
    Socket socket;

    uint64_t totalCommandBytes = 0;

    auto handshake() -> void {}

  public:
    ReplicationClient(const std::string &hostname, uint32_t port) : socket(Socket::createClient(hostname, port)) {
        handshake();
    }
};
