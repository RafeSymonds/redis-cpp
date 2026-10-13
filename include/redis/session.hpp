#pragma once

#include <utility>

#include "net/socket.hpp"

class Redis;

// One client connection: reads commands, executes them, writes replies.
// Per-client state (MULTI queue, WATCH keys, subscriptions, auth) belongs here.
class Session {
    Socket socket;
    Redis &redis;

  public:
    Session(Socket socket, Redis &redis) : socket(std::move(socket)), redis(redis) {}

    // Returns when the client disconnects or sends something unparseable.
    auto run() -> void;
};
