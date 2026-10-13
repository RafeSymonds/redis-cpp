#include <iostream>
#include <print>
#include <random>

#include "replication/master_role.hpp"

namespace {

constexpr std::size_t REPLICATION_ID_LENGTH = 40;

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

auto MasterRole::propagate(const std::string &command) -> void {
    for (auto &replica : replicas) {
        try {
            replica.sendString(command);
        } catch (const std::exception &e) {
            std::println("Replica socket hit error when trying to propagate command {}", e.what());
            std::cout.flush();
        }
    }
}
