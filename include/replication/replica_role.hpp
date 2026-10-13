#pragma once

#include <utility>

#include "config/config.hpp"

// Replication state of an instance acting as replica.
class ReplicaRole {
    ReplicaOf master;

    uint64_t totalCommandBytes = 0;

  public:
    explicit ReplicaRole(ReplicaOf master) : master(std::move(master)) {}

    [[nodiscard]] auto getMaster() const -> const ReplicaOf & { return master; }
    [[nodiscard]] auto getTotalCommandBytes() const -> uint64_t { return totalCommandBytes; }
    auto processCommand(const std::string &command) -> void { totalCommandBytes += command.size(); }
};
