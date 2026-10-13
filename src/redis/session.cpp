#include <iostream>
#include <print>

#include "redis/redis.hpp"
#include "redis/session.hpp"
#include "resp/parser.hpp"

auto Session::run() -> void {
    try {
        while (true) {
            auto args = resp::parse(socket);
            socket.sendString(redis.execute(args, Origin::CLIENT));
        }
    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}
