#pragma once

#include <cstdint>

#include "net/socket_handle.hpp"
#include "rdb/rdb_file_manager.hpp"

class ReplicaManager {
    uint64_t replicationId;
    uint64_t replicationOffset;

    uint64_t previousWriteOffset;

    std::vector<Socket> replicas;

    auto getReplicaAck(Socket &replica) -> uint64_t;

  public:
    [[nodiscard]] auto numReplicas() const -> size_t { return replicas.size(); }
    [[nodiscard]] auto getReplicationId() const -> uint64_t { return replicationId; }

    auto propagate(const std::string &command) -> void;
    auto wait(uint32_t numReplicasNeeded, uint32_t timeoutMS) -> bool;

    static auto sendDB(Socket &replica, KVStore &kvStore) -> void;
};
