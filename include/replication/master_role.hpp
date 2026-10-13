#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "kv_store/kv_store.hpp"
#include "net/socket.hpp"

// A connected replica. Only its reader thread reads the socket; writes happen under MasterRole::mutex.
struct Replica {
    Socket socket;
    uint64_t ackOffset = 0; // last offset the replica acknowledged
};

// Replication state of an instance acting as master. Thread-safe: every member function
// takes the internal mutex, because replica reader threads update ack offsets concurrently.
// Not movable (owns a mutex), so it is constructed in place inside Role.
class MasterRole {
    std::string replicationId;
    uint64_t replicationOffset = 0; // bytes sent to replicas so far

    mutable std::mutex mutex;
    std::condition_variable acked;

    // unique_ptr keeps each Replica at a stable address for its reader thread.
    std::vector<std::unique_ptr<Replica>> replicas;

    auto readAcks(Replica &replica) -> void;
    [[nodiscard]] auto countAcked(uint64_t target) const -> std::size_t;

  public:
    MasterRole();

    [[nodiscard]] auto numReplicas() const -> size_t;
    [[nodiscard]] auto getReplicationId() const -> const std::string & { return replicationId; }
    [[nodiscard]] auto getReplicationOffset() const -> uint64_t;

    // Sends the RDB snapshot of store to replica, then starts propagating to it and reading its ACKs.
    auto addReplica(Socket replica, const KVStore &store) -> void;

    // Sends an already RESP-encoded write command to every replica.
    auto propagate(const std::string &command) -> void;

    // Returns the offset replicas must reach to have seen every write so far.
    // Sends REPLCONF GETACK * to all replicas if any of them is behind it.
    auto requestAcks() -> uint64_t;

    // Blocks until numReplicasNeeded replicas acknowledged target or timeout passes.
    // Returns the number of replicas that acknowledged target.
    auto waitForAcks(uint64_t target, std::size_t numReplicasNeeded, std::chrono::milliseconds timeout)
        -> std::size_t;
};
