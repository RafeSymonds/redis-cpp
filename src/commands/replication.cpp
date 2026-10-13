#include <format>
#include <stdexcept>

#include "commands/registry.hpp"
#include "redis/session.hpp"
#include "resp/encode.hpp"

namespace {

auto replconf(CommandContext & /*ctx*/) -> Reply { return std::string(resp::OK); }

// Full resync: reply, send the snapshot, then hand the connection to MasterRole.
// Runs under the execute lock, so no write lands between the snapshot and registration.
auto psync(CommandContext &ctx) -> Reply {
    auto *master = std::get_if<MasterRole>(&ctx.role);

    if (master == nullptr) {
        throw std::runtime_error("PSYNC sent to a replica");
    }

    if (ctx.session == nullptr) {
        throw std::runtime_error("PSYNC requires a client connection");
    }

    ctx.session->sendRaw(resp::simpleString(
        std::format("FULLRESYNC {} {}", master->getReplicationId(), master->getReplicationOffset())));
    master->addReplica(ctx.session->detach(), ctx.store);

    return std::nullopt;
}

} // namespace

auto registerReplicationCommands(CommandRegistry &registry) -> void {
    registry.add("REPLCONF", {.handler = replconf});
    registry.add("PSYNC", {.handler = psync});
}
