#include <cstdint>

#include "net/socket_handle.hpp"
#include "rdb/rdb_file_manager.hpp"

class ReplicaManager {
    uint64_t replicationId;
    uint64_t replicationOffset;

    uint64_t previousWriteOffset;

    std::vector<Socket> replicas;

    auto getReplicaAck(Socket &replica) -> uint64_t;
    auto sendDB(Socket &replica, KVStore &kvStore) -> void;

  public:
    [[nodiscard]] auto numReplicas() const -> size_t { return replicas.size(); }

    auto propagate(const std::string &command) -> void;
    auto wait(uint32_t numReplicasNeeded, uint32_t timeoutMS) -> bool;
};
