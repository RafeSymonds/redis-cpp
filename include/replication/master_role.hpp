#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "kv_store/kv_store.hpp"
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

    // Sends the RDB snapshot of store to replica, then starts propagating to it.
    auto addReplica(Socket replica, const KVStore &store) -> void;

    // Sends an already RESP-encoded write command to every replica.
    auto propagate(const std::string &command) -> void;
};
