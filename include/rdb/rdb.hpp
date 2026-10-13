#pragma once

#include <string>

#include "kv_store/kv_store.hpp"

namespace rdb {

// Loads the RDB file at path into store. A missing file leaves store untouched.
auto load(const std::string &path, KVStore &store) -> void;

// Serializes store into RDB bytes (used for replica full resync).
auto serialize(const KVStore &store) -> std::string;

} // namespace rdb
