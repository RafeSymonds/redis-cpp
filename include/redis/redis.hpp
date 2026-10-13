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
    MASTER,     // replica applying the master's stream: persist writes, no reply
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

    // Runs one command and returns its RESP reply, if any. session is nullptr for AOF replay.
    auto execute(const std::vector<std::string> &args, Origin origin, Session *session = nullptr) -> Reply;

    // Loads state, then accepts clients until the process exits.
    auto run() -> void;
};
