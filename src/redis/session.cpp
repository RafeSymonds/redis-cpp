#include <iostream>
#include <print>

#include "redis/redis.hpp"
#include "redis/session.hpp"
#include "resp/parser.hpp"

auto Session::run() -> void {
    try {
        while (!detached) {
            auto args = resp::parse(socket);

            if (auto reply = redis.execute(args, Origin::CLIENT, this)) {
                socket.sendString(*reply);
            }
        }
    } catch (const std::exception &e) {
        std::println(std::cerr, "Exit with error {}", e.what());
        std::cerr.flush();
    }
}
