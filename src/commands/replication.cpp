#include <chrono>
#include <format>
#include <stdexcept>

#include "commands/registry.hpp"
#include "redis/session.hpp"
#include "resp/encode.hpp"
#include "utils/string_utils.hpp"

namespace {

auto replconf(CommandContext &ctx) -> Reply {
    if (ctx.args.size() >= 2 && toUpper(ctx.args[1]) == "GETACK") {
        auto *replica = std::get_if<ReplicaRole>(&ctx.role);

        if (replica == nullptr) {
            throw std::runtime_error("Tried to ack on master");
        }

        return resp::array({"REPLCONF", "ACK", std::to_string(replica->getTotalCommandBytes())});
    }

    return std::string(resp::OK);
}

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

// WAIT <numreplicas> <timeout>: how many replicas have acknowledged every write so far.
// Releases the server lock while waiting so other clients keep running.
auto wait(CommandContext &ctx) -> Reply {
    auto *master = std::get_if<MasterRole>(&ctx.role);

    if (master == nullptr) {
        throw std::runtime_error("WAIT sent to a replica");
    }

    auto numReplicasNeeded = std::stoull(ctx.args[1]);
    auto timeout = std::chrono::milliseconds(std::stoll(ctx.args[2]));

    auto target = master->requestAcks();

    ctx.lock.unlock();
    auto acked = master->waitForAcks(target, numReplicasNeeded, timeout);
    ctx.lock.lock();

    return resp::integer(static_cast<int64_t>(acked));
}

} // namespace

auto registerReplicationCommands(CommandRegistry &registry) -> void {
    registry.add("REPLCONF", {.handler = replconf});
    registry.add("PSYNC", {.handler = psync});
    registry.add("WAIT", {.handler = wait});
}
