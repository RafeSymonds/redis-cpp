#pragma once

#include <string>
#include <unordered_map>

#include "commands/command.hpp"

class CommandRegistry {
    std::unordered_map<std::string, Command> commands;

  public:
    // Registers every command group below.
    CommandRegistry();

    auto add(const std::string &name, Command command) -> void;

    // nullptr for unknown commands.
    [[nodiscard]] auto find(const std::string &name) const -> const Command *;
};

// One per file in src/commands/.
auto registerServerCommands(CommandRegistry &registry) -> void;
auto registerStringCommands(CommandRegistry &registry) -> void;
auto registerReplicationCommands(CommandRegistry &registry) -> void;
