#pragma once

#include <string>
#include <vector>

#include "config/config.hpp"
#include "kv_store/kv_store.hpp"
#include "replication/role.hpp"

// Everything a command handler may touch. Handlers only read args, mutate state and
// return the RESP reply. Side effects (AOF, propagation) are applied by Redis::execute.
struct CommandContext {
    const std::vector<std::string> &args;
    KVStore &store;
    const RedisConfig &config;
    Role &role;
};

using CommandHandler = std::string (*)(CommandContext &ctx);

struct Command {
    CommandHandler handler;
    bool write = false; // persisted to the AOF and propagated to replicas
};
