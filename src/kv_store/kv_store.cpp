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
