#pragma once

#include <variant>

#include "replication/master_role.hpp"
#include "replication/replica_role.hpp"

using Role = std::variant<MasterRole, ReplicaRole>;

inline auto isMaster(const Role &role) -> bool { return std::holds_alternative<MasterRole>(role); }
