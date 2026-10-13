#include "commands/registry.hpp"

CommandRegistry::CommandRegistry() {
    registerServerCommands(*this);
    registerStringCommands(*this);
    registerReplicationCommands(*this);
}

auto CommandRegistry::add(const std::string &name, Command command) -> void { commands[name] = command; }

auto CommandRegistry::find(const std::string &name) const -> const Command * {
    auto it = commands.find(name);

    if (it == commands.end()) {
        return nullptr;
    }

    return &it->second;
}
