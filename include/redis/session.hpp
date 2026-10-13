#pragma once

#include <string_view>
#include <utility>

#include "net/socket.hpp"

class Redis;

// One client connection: reads commands, executes them, writes replies.
// Per-client state (MULTI queue, WATCH keys, subscriptions, auth) belongs here.
class Session {
    Socket socket;
    Redis &redis;

    // Set once the connection has been handed off (PSYNC). The session stops reading.
    bool detached = false;

  public:
    Session(Socket socket, Redis &redis) : socket(std::move(socket)), redis(redis) {}

    // Writes bytes straight to the client, for commands that reply in several parts.
    auto sendRaw(std::string_view bytes) -> void { socket.sendString(bytes); }

    // Gives up the connection. run() returns after the current command.
    auto detach() -> Socket {
        detached = true;
        return std::move(socket);
    }

    // Returns when the client disconnects, sends something unparseable, or is detached.
    auto run() -> void;
};
