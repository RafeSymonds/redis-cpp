#include <string>

#include "aof/aof.hpp"

struct AOFConfig {
    bool appendonly = false;
    std::string appenddirname = "appendonlydir";
    std::string appendfilename = "appendonly.aof";
    AppendFSync appendfsync = AppendFSync::EVERYSEC;

    [[nodiscard]] auto appendonlyString() const -> std::string {
        if (appendonly) {
            return "yes";
        }

        return "no";
    }
};
