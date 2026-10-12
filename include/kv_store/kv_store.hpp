#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "utils/time_utils.hpp"

struct Item {
    std::string value;
    TimePoint expireTime;

    [[nodiscard]] auto IsExpired(TimePoint time) const -> bool { return expireTime < time; }
};

class KVStore {
    std::unordered_map<std::string, Item> keyValueStore;
    std::mutex lock;

  public:
    KVStore() = default;

    KVStore(KVStore &&other) noexcept {
        std::lock_guard guard(other.lock);
        keyValueStore = std::move(other.keyValueStore);
    }

    KVStore(const KVStore &) = delete;
    KVStore &operator=(const KVStore &) = delete;

    auto updatedKeyValueStore(const std::string &key, const std::string &value, TimePoint expireTime) -> void;

    auto getValue(const std::string &key) -> std::optional<std::string>;

    auto getValues(const std::string &pattern) -> std::vector<std::string>;
};
