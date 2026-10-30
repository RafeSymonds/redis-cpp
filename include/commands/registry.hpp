#include <functional>
#include <unordered_map>

#include "commands/command.hpp"

class Session;

using CommandFunc = std::function<Session()>;

class CommandRegistry {
    static std::unordered_map<Command, CommandFunc> commands;

  public:
    static auto registerCommand(Command command, CommandFunc func) -> void;
    static auto getCommand(Command command) -> const CommandFunc &;
};
