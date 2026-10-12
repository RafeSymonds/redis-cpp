#include <string>
#include <utility>

struct AOFConfig {
    bool appendonly = false;
    std::string appenddirname = "appendonlydir";
    std::string appendfilename = "appendonly.aof";
    std::string appendfsync = "everysec";

    [[nodiscard]] auto appendonlyString() const -> std::string {
        if (appendonly) {
            return "yes";
        }

        return "no";
    }
};

class AOFFileManager {

    AOFConfig config;

  public:
    explicit AOFFileManager(AOFConfig config) : config(std::move(config)) {}

    [[nodiscard]] auto getConfig() const -> const AOFConfig & { return config; }
};
