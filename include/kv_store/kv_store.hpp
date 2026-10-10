#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "utils/time_utils.hpp"

struct Item {
    std::string value;
    TimePoint expireTime;
};

class KVStore {
    std::unordered_map<std::string, Item> keyValueStore;
    std::mutex lock;

  public:
    auto updatedKeyValueStore(const std::string &key, const std::string &value, TimePoint expireTime) -> void;

    auto getValue(const std::string &key) -> std::optional<std::string>;
};
