#pragma once

#include <utility>

#include "config/config.hpp"

// Replication state of an instance acting as replica.
class ReplicaRole {
    ReplicaOf master;

  public:
    explicit ReplicaRole(ReplicaOf master) : master(std::move(master)) {}

    auto connect(uint16_t port) const -> void;

    [[nodiscard]] auto getMaster() const -> const ReplicaOf & { return master; }
};
