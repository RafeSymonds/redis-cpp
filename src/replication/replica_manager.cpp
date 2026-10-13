#include <condition_variable>
#include <iostream>
#include <print>
#include <thread>

#include "replication/replica_manager.hpp"
#include "utils/string_utils.hpp"

namespace {
constexpr std::string_view GETACK_COMMAND = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";
}

auto ReplicaManager::getReplicaAck(Socket &replica) -> uint64_t {
    replica.sendString(GETACK_COMMAND);

    auto result = parseRedisCommand(replica);

    if (result.size() != 3) {
        throw std::runtime_error(
            std::format("replica did not send a valid response. Sent {} items, expected 3.", result.size()));
    }

    return std::stoull(result[2]);
}
auto ReplicaManager::sendDB(Socket &replica, KVStore &kvStore) -> void {
    replica.sendString(RDBFileManager::writeDBFile(kvStore));
}

auto ReplicaManager::propagate(const std::string &command) -> void {
    for (auto &replica : replicas) {
        try {
            replica.sendString(command);
        } catch (const std::exception &e) {
            std::println("Replica socket hit error when trying to propagate command {}", e.what());
            std::cout.flush();
        }
    }
}

auto ReplicaManager::wait(uint32_t numReplicasNeeded, uint32_t timeoutMS) -> bool {
    std::mutex mutex;
    std::condition_variable cv;

    std::size_t completed = 0;

    std::vector<std::thread> threads;
    threads.reserve(numReplicas());

    for (auto &replica : replicas) {
        threads.emplace_back([&] {
            auto offset = getReplicaAck(replica);

            if (offset >= previousWriteOffset) {
                {
                    std::lock_guard lock(mutex);
                    ++completed;
                }
                cv.notify_one();
            }
        });
    }

    std::unique_lock lock(mutex);

    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMS);

    return cv.wait_until(lock, deadline, [&] { return completed >= numReplicasNeeded; });
}
