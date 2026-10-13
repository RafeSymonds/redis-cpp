#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/time_utils.hpp"

struct Item {
    std::string value;
    TimePoint expireTime;

    [[nodiscard]] auto IsExpired(TimePoint time) const -> bool { return expireTime < time; }
};

// Not thread-safe. Redis::execute serializes all access.
class KVStore {
    std::unordered_map<std::string, Item> keyValueStore;

  public:
    auto set(const std::string &key, const std::string &value, TimePoint expireTime) -> void;

    auto get(const std::string &key) -> std::optional<std::string>;

    [[nodiscard]] auto keys(const std::string &pattern) const -> std::vector<std::string>;

    [[nodiscard]] auto items() const -> const std::unordered_map<std::string, Item> & { return keyValueStore; }
};
