#include <format>
#include <ranges>

#include "commands/registry.hpp"
#include "replication/master_role.hpp"
#include "resp/encode.hpp"

namespace {

auto ping(CommandContext & /*ctx*/) -> Reply { return resp::simpleString("PONG"); }

auto echo(CommandContext &ctx) -> Reply { return resp::bulkString(ctx.args[1]); }

auto config(CommandContext &ctx) -> Reply {
    auto &type = ctx.args[1];

    std::vector<std::string> output;

    if (type == "GET") {
        for (auto &&name : ctx.args | std::views::drop(2)) {
            if (auto value = configGet(ctx.config, name)) {
                output.push_back(name);
                output.push_back(std::move(*value));
            }
        }
    }

    return resp::array(output);
}

auto keys(CommandContext &ctx) -> Reply { return resp::array(ctx.store.keys(ctx.args[1])); }

auto info(CommandContext &ctx) -> Reply {
    auto *master = std::get_if<MasterRole>(&ctx.role);

    std::string output = std::format("# Replication\nrole:{}\n", master != nullptr ? "master" : "slave");

    if (master != nullptr) {
        output += std::format("master_repl_offset:{}\n", master->getReplicationOffset());
        output += std::format("master_replid:{}\n", master->getReplicationId());
    }

    return resp::bulkString(output);
}

} // namespace

auto registerServerCommands(CommandRegistry &registry) -> void {
    registry.add("PING", {.handler = ping});
    registry.add("ECHO", {.handler = echo});
    registry.add("CONFIG", {.handler = config});
    registry.add("KEYS", {.handler = keys});
    registry.add("INFO", {.handler = info});
}
