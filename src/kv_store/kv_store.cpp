#include <iostream>
#include <print>

#include "kv_store/kv_store.hpp"

auto KVStore::set(const std::string &key, const std::string &value, TimePoint expireTime) -> void {
    keyValueStore[key] = {value, expireTime};
}

auto KVStore::get(const std::string &key) -> std::optional<std::string> {
    auto it = keyValueStore.find(key);

    if (it == keyValueStore.end()) {
        return {};
    }

    if (Clock::now() > it->second.expireTime) {
        keyValueStore.erase(it);

        return {};
    }

    return it->second.value;
}

auto KVStore::keys(const std::string &pattern) const -> std::vector<std::string> {

    auto currentTime = Clock::now();

    std::println("num values {}", keyValueStore.size());
    std::cout.flush();

    std::vector<std::string> values;

    for (const auto &[key, item] : keyValueStore) {
        if (item.IsExpired(currentTime)) {
            continue;
        }

        values.push_back(key);
    }

    return values;
}
