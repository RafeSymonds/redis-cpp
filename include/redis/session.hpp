
#include "net/socket_handle.hpp"

class Redis;

class Session {
    Socket client;
    Redis &redis;

    bool done = false;
};
