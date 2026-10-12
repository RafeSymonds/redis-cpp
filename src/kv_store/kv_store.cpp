#include <iostream>
#include <print>

#include "kv_store/kv_store.hpp"

auto KVStore::updatedKeyValueStore(const std::string &key, const std::string &value, TimePoint expireTime) -> void {
    std::lock_guard<std::mutex> guard(lock);

    keyValueStore[key] = {value, expireTime};
}
auto KVStore::getValue(const std::string &key) -> std::optional<std::string> {
    std::lock_guard<std::mutex> guard(lock);

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
auto KVStore::getValues(const std::string &pattern) -> std::vector<std::string> {

    auto currentTime = Clock::now();

    std::println("num values {}", keyValueStore.size());
    std::cout.flush();

    std::vector<std::string> values;

    for (const auto &[key, item] : keyValueStore) {
        if (item.IsExpired(currentTime)) {
            continue;
        }

        values.push_back(item.value);
    }

    return values;
}
