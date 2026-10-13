#pragma once

#include <cstdint>
#include <string>

#include "config/config.hpp"
#include "net/socket.hpp"

class Redis;

// A replica's connection to its master: handshake, RDB transfer, then applying
// propagated commands without replying. The replica-side counterpart of Session.
class MasterLink {
    Socket connection;
    uint16_t listeningPort;
    Redis &redis;

    auto handshake() -> void;
    auto receiveRDB() -> std::string;

  public:
    MasterLink(const ReplicaOf &master, uint16_t listeningPort, Redis &redis)
        : connection(Socket::createClient(master.hostname, master.port)), listeningPort(listeningPort), redis(redis) {}

    // Returns when the master disconnects or sends something unparseable.
    auto run() -> void;
};
