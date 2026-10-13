#include <format>
#include <stdexcept>

#include "commands/registry.hpp"
#include "resp/encode.hpp"

namespace {

auto replconf(CommandContext & /*ctx*/) -> std::string { return std::string(resp::OK); }

auto psync(CommandContext &ctx) -> std::string {
    auto *master = std::get_if<MasterRole>(&ctx.role);

    if (master == nullptr) {
        throw std::runtime_error("PSYNC sent to a replica");
    }

    return resp::simpleString(
        std::format("FULLRESYNC {} {}", master->getReplicationId(), master->getReplicationOffset()));
}

} // namespace

auto registerReplicationCommands(CommandRegistry &registry) -> void {
    registry.add("REPLCONF", {.handler = replconf});
    registry.add("PSYNC", {.handler = psync});
}
