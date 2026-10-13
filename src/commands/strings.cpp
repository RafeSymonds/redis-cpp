#include "commands/registry.hpp"
#include "resp/encode.hpp"

namespace {

auto set(CommandContext &ctx) -> Reply {
    auto &key = ctx.args[1];
    auto &value = ctx.args[2];

    auto expireTime = TimePoint::max();

    if (ctx.args.size() >= 5) {
        auto &expireType = ctx.args[3];
        auto delta = std::stoll(ctx.args[4]);

        expireTime = Clock::now() + std::chrono::milliseconds(expireType == "EX" ? delta * 1000 : delta);
    }

    ctx.store.set(key, value, expireTime);

    return std::string(resp::OK);
}

auto get(CommandContext &ctx) -> Reply {
    auto value = ctx.store.get(ctx.args[1]);

    if (value.has_value()) {
        return resp::bulkString(*value);
    }
    return std::string(resp::NULL_BULK_STRING);
}

} // namespace

auto registerStringCommands(CommandRegistry &registry) -> void {
    registry.add("SET", {.handler = set, .write = true});
    registry.add("GET", {.handler = get});
}
