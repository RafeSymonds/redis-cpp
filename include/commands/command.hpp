#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "config/config.hpp"
#include "kv_store/kv_store.hpp"
#include "replication/role.hpp"

class Session;

// RESP reply to send, or nullopt to send nothing.
using Reply = std::optional<std::string>;

// Everything a command handler may touch. Handlers only read args, mutate state and
// return the RESP reply. Side effects (AOF, propagation) are applied by Redis::execute.
struct CommandContext {
    const std::vector<std::string> &args;
    KVStore &store;
    const RedisConfig &config;
    Role &role;
    Session *session; // nullptr when there is no connection (AOF replay)

    // The server lock, held while the handler runs. A blocking command (WAIT) may
    // release it while it waits, and must reacquire it before returning.
    std::unique_lock<std::mutex> &lock;
};

using CommandHandler = Reply (*)(CommandContext &ctx);

struct Command {
    CommandHandler handler;
    bool write = false; // persisted to the AOF and propagated to replicas
};
