#include "shell.h"
#include "commands.h"
#include <iostream>
#include <sstream>

Shell::Shell() {
    prompt = "\033[1m\033[33mBarsikCMD>\033[0m ";
    registerCommands();
}

void Shell::registerCommands() {
    commands["help"]    = [this](const std::vector<std::string>& args) { cmd_help(commands, args); };
    commands["echo"]    = [](const std::vector<std::string>& args)     { cmd_echo(args); };
    commands["clear"]   = [](const std::vector<std::string>& args)     { cmd_clear(args); };
    commands["ver"]     = [](const std::vector<std::string>& args)     { cmd_ver(args); };
    commands["exit"]    = [](const std::vector<std::string>& args)     { cmd_exit(args); };
    commands["update"]  = [](const std::vector<std::string>& args)     { cmd_update(args); };
}

void Shell::run(int argc, char* argv[]) {
    // Обработка флагов -run и -run2
    if (argc >= 3) {
        std::string flag = argv[1];
        if (flag == "-run" || flag == "-run2") {
            std::string input;
            for (int i = 2; i < argc; ++i) {
                if (i > 2) input += " ";
                input += argv[i];
            }
            execute(input);
            if (flag == "-run") return;
            // -run2: продолжить работу в интерактивном режиме
        }
    }

    std::cout << "\033[1m\033[33mBarsikCMD v2026.3.2\033[0m — введи 'help' для списка команд\n\n";

    std::string input;
    while (true) {
        std::cout << prompt << std::flush;
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
        std::cout << "\033[31m" << "Неизвестная команда: " << cmd << ". Введи 'help'." << "\033[0m\n";
    }
}

std::vector<std::string> Shell::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}