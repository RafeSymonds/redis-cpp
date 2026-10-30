#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "utils/time_utils.hpp"

struct Item {
    std::string value;
    TimePoint expireTime;

    [[nodiscard]] auto isExpired(TimePoint time) const -> bool { return expireTime < time; }
};

class KVStore {
    std::unordered_map<std::string, Item> keyValueStore;

  public:
    KVStore() = default;

    auto updatedKeyValueStore(const std::string &key, const std::string &value, TimePoint expireTime) -> void;

    auto getValue(const std::string &key) -> std::optional<std::string>;

    auto getValues(const std::string &pattern) -> std::vector<std::string>;

    auto getItems() -> std::unordered_map<std::string, Item>;
};
