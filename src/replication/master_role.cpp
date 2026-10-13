#include <chrono>
#include <condition_variable>
#include <format>
#include <iostream>
#include <print>
#include <mutex>
#include <random>
#include <stdexcept>
#include <thread>

#include "rdb/rdb.hpp"
#include "replication/master_role.hpp"
#include "resp/parser.hpp"
#include "utils/string_utils.hpp"

namespace {

constexpr std::size_t REPLICATION_ID_LENGTH = 40;

constexpr std::string_view GETACK_COMMAND = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";

auto generateReplicationId() -> std::string {
    constexpr std::string_view HEX = "0123456789abcdef";

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<std::size_t> distribution(0, HEX.size() - 1);

    std::string id(REPLICATION_ID_LENGTH, '0');
    for (auto &c : id) {
        c = HEX[distribution(generator)];
    }

    return id;
}

} // namespace

MasterRole::MasterRole() : replicationId(generateReplicationId()) {}

auto MasterRole::numReplicas() const -> size_t {
    std::lock_guard guard(mutex);
    return replicas.size();
}

auto MasterRole::getReplicationOffset() const -> uint64_t {
    std::lock_guard guard(mutex);
    return replicationOffset;
}

auto MasterRole::addReplica(Socket replica, const KVStore &store) -> void {
    std::lock_guard guard(mutex);

    // Bulk-string framing without the trailing CRLF.
    auto snapshot = rdb::serialize(store);
    replica.sendString(std::format("${}\r\n{}", snapshot.size(), snapshot));

    auto &added = *replicas.emplace_back(std::make_unique<Replica>(Replica{.socket = std::move(replica)}));

    // Replicas are never removed and MasterRole lives as long as the server, so the
    // detached thread's references stay valid.
    std::thread([this, &added] { readAcks(added); }).detach();
}

// The only reader of replica.socket. Records each REPLCONF ACK <offset> and wakes waiters.
auto MasterRole::readAcks(Replica &replica) -> void {
    try {
        while (true) {
            auto args = resp::parse(replica.socket);

            if (args.size() == 3 && toUpper(args[0]) == "REPLCONF" && toUpper(args[1]) == "ACK") {
                {
                    std::lock_guard guard(mutex);
                    replica.ackOffset = std::stoull(args[2]);
                }
                acked.notify_all();
            }
        }
    } catch (const std::exception &e) {
        std::println("Replica connection closed: {}", e.what());
        std::cout.flush();
    }
}

auto MasterRole::propagate(const std::string &command) -> void {
    std::lock_guard guard(mutex);

    for (auto &replica : replicas) {
        if (replica->socket.sendString(command) < 0) {
            std::println("Failed to propagate command to replica");
            std::cout.flush();
        }
    }

    replicationOffset += command.size();
}

// Caller holds mutex.
auto MasterRole::countAcked(uint64_t target) const -> std::size_t {
    std::size_t count = 0;

    for (const auto &replica : replicas) {
        if (replica->ackOffset >= target) {
            count++;
        }
    }

    return count;
}

auto MasterRole::requestAcks() -> uint64_t {
    std::lock_guard guard(mutex);

    auto target = replicationOffset;

    if (countAcked(target) == replicas.size()) {
        return target;
    }

    for (auto &replica : replicas) {
        replica->socket.sendString(GETACK_COMMAND);
    }

    // Replicas count GETACK itself in their offset.
    replicationOffset += GETACK_COMMAND.size();

    return target;
}

auto MasterRole::waitForAcks(uint64_t target, std::size_t numReplicasNeeded, std::chrono::milliseconds timeout)
    -> std::size_t {
    std::unique_lock guard(mutex);

    acked.wait_for(guard, timeout, [&] { return countAcked(target) >= numReplicasNeeded; });

    return countAcked(target);
}
