#include <variant>

#include "replication/primary_role.hpp"
#include "replication/replica_role.hpp"

using Role = std::variant<PrimaryRole, ReplicaRole>;
