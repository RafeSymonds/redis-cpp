#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "aof/aof.hpp"
#include "commands/registry.hpp"
#include "config/config.hpp"
#include "kv_store/kv_store.hpp"
#include "replication/role.hpp"

// Where a command came from. Decides which side effects execute() applies.
enum class Origin : uint8_t {
    CLIENT,     // reply, persist writes, propagate writes
    AOF_REPLAY, // apply only
};

class Redis {
    RedisConfig config;
    KVStore store;
    AOF aof;
    Role role;
    CommandRegistry registry;

    // Serializes every command so each one is atomic against all clients.
    std::mutex lock;

    // Startup: load RDB, replay AOF, open AOF for appending.
    auto loadState() -> void;

  public:
    explicit Redis(RedisConfig config);

    // Runs one command and returns its RESP reply.
    auto execute(const std::vector<std::string> &args, Origin origin) -> std::string;

    // Loads state, then accepts clients until the process exits.
    auto run() -> void;
};
