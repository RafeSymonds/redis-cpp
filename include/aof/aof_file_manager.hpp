#include <cstdint>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

enum class AppendFSync { ALWAYS, EVERYSEC, NO };

inline auto parseAppendFSync(const std::string &s) -> AppendFSync {
    if (s == "always") {
        return AppendFSync::ALWAYS;
    }

    if (s == "everysec") {
        return AppendFSync::EVERYSEC;
    }

    if (s == "no") {
        return AppendFSync::NO;
    }

    throw std::invalid_argument(std::format("invalid appendfsync value: {}", s));
}

inline auto toString(AppendFSync appendfsync) -> std::string {
    switch (appendfsync) {
        case AppendFSync::ALWAYS:
            return "always";
        case AppendFSync::EVERYSEC:
            return "everysec";
        case AppendFSync::NO:
            return "no";
    }

    throw std::invalid_argument("invalid appendfsync value");
}

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

class AOFFileManager {
    std::string dir;
    AOFConfig config;

    uint32_t fileIncrement = 1;

    auto setup() -> void;

    std::optional<std::ofstream> fileHandle;

    [[nodiscard]] auto fileIncrementName() const -> std::string { return std::format(".{}.incr.aof", fileIncrement); }

    [[nodiscard]] auto dirPath() const -> std::string { return dir + "/" + config.appenddirname; }
    [[nodiscard]] auto fullFilePath() const -> std::string {
        return dirPath() + "/" + config.appendfilename + fileIncrementName();
    }

    [[nodiscard]] auto manifestFileName() const -> std::string { return config.appendfilename + ".manifest"; }
    [[nodiscard]] auto manifestFullFilePath() const -> std::string { return dirPath() + "/" + manifestFileName(); }

  public:
    AOFFileManager(std::string dir, AOFConfig config) : dir(std::move(dir)), config(std::move(config)) { setup(); }

    [[nodiscard]] auto getConfig() const -> const AOFConfig & { return config; }

    auto persistCommand(const std::string &command) -> void;
};
