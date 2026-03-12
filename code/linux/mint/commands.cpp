#include "commands.h"
#include <iostream>

#ifdef _WIN32
    #include <cstdlib>
    #define CLEAR_CMD "cls"
#else
    #define CLEAR_CMD "clear"
#endif

void cmd_help(const std::map<std::string, std::function<void(const std::vector<std::string>&)>>& commands,
              const std::vector<std::string>& args) {
    std::cout << "Доступные команды:\n";
    for (const auto& [name, _] : commands) {
        std::cout << "  " << name << "\n";
    }
}

void cmd_echo(const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        std::cout << args[i];
        if (i + 1 < args.size()) std::cout << " ";
    }
    std::cout << "\n";
}

void cmd_clear(const std::vector<std::string>& args) {
    system(CLEAR_CMD);
}

void cmd_ver(const std::vector<std::string>& args) {
    std::cout << "BarsikCMD версия 2026.3.11.1\n";
}

void cmd_exit(const std::vector<std::string>& args) {
    std::cout << "Пока!\n";
    exit(0);
}