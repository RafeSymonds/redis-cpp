#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "net/socket.hpp"

// Replication state of an instance acting as master.
class MasterRole {
    std::string replicationId;
    uint64_t replicationOffset = 0;

    std::vector<Socket> replicas;

  public:
    MasterRole();

    [[nodiscard]] auto numReplicas() const -> size_t { return replicas.size(); }
    [[nodiscard]] auto getReplicationId() const -> const std::string & { return replicationId; }
    [[nodiscard]] auto getReplicationOffset() const -> uint64_t { return replicationOffset; }

    // Sends an already RESP-encoded write command to every replica.
    auto propagate(const std::string &command) -> void;
};
