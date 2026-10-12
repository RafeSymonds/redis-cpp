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
    std::string dir;
    AOFConfig config;

    auto setup() -> void;

    [[nodiscard]] auto dirPath() const -> std::string { return dir + "/" + config.appenddirname; }
    [[nodiscard]] auto fullFilePath() const -> std::string { return dirPath() + "/" + config.appendfilename; }

  public:
    AOFFileManager(std::string dir, AOFConfig config) : dir(std::move(dir)), config(std::move(config)) { setup(); }

    [[nodiscard]] auto getConfig() const -> const AOFConfig & { return config; }
};
