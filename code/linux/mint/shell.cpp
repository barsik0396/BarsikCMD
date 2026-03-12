#include "shell.h"
#include "commands.h"
#include <iostream>
#include <sstream>

Shell::Shell() {
    prompt = "BarsikCMD> ";
    registerCommands();
}

void Shell::registerCommands() {
    commands["help"]    = [this](const std::vector<std::string>& args) { cmd_help(commands, args); };
    commands["echo"]    = [](const std::vector<std::string>& args)     { cmd_echo(args); };
    commands["clear"]   = [](const std::vector<std::string>& args)     { cmd_clear(args); };
    commands["ver"]     = [](const std::vector<std::string>& args)     { cmd_ver(args); };
    commands["exit"]    = [](const std::vector<std::string>& args)     { cmd_exit(args); };
}

void Shell::run() {
    std::cout << "BarsikCMD v2026.3.11.1 — введи 'help' для списка команд\n\n";

    std::string input;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, input)) break;
        if (input.empty()) continue;

        history.push_back(input);
        execute(input);
    }
}

void Shell::execute(const std::string& input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return;

    const std::string& cmd = tokens[0];
    auto it = commands.find(cmd);
    if (it != commands.end()) {
        it->second(tokens);
    } else {
        std::cout << "Неизвестная команда: " << cmd << ". Введи 'help'.\n";
    }
}

std::vector<std::string> Shell::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}