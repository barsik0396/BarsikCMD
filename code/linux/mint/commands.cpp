#include "commands.h"
#include "commands/colors.h"
#include "shell.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#ifdef _WIN32
    #define CLEAR_CMD "cls"
#else
    #define CLEAR_CMD "clear"
#endif

static const std::map<std::string, std::string> CMD_DESCRIPTIONS = {
    { "help",   "показать список команд" },
    { "echo",   "вывести текст на экран" },
    { "clear",  "очистить экран" },
    { "ver",    "показать версию BarsikCMD" },
    { "exit",   "выйти из BarsikCMD" },
    { "update", "проверить и установить обновления" },
    { "meow",   "показать котика" },
    { "info",   "информация о проекте. Аргументы: links" },
    { "reload", "перезапустить BarsikCMD" },
    { "file",   "работа с файлами. Аргументы: new, del, set, edit, move" },
};

void cmd_help(const std::map<std::string, std::function<void(const std::vector<std::string>&)>>& commands,
              const std::vector<std::string>& args) {
    (void)args;
    std::cout << BOLD << YELLOW << "BarsikCMD — доступные команды:\n" << RESET;
    std::cout << CYAN << "─────────────────────────────────\n" << RESET;
    for (const auto& [name, _] : commands) {
        auto it = CMD_DESCRIPTIONS.find(name);
        std::string desc = (it != CMD_DESCRIPTIONS.end()) ? it->second : "";
        std::cout << "  " << GREEN << BOLD << name << RESET
                  << " — " << desc << "\n";
    }
    std::cout << CYAN << "─────────────────────────────────\n" << RESET;
}

void cmd_echo(const std::vector<std::string>& args) {
    std::cout << WHITE;
    for (size_t i = 1; i < args.size(); ++i) {
        std::cout << args[i];
        if (i + 1 < args.size()) std::cout << " ";
    }
    std::cout << RESET << "\n";
}

void cmd_clear(const std::vector<std::string>& args) {
    (void)args;
    system(CLEAR_CMD);
}

void cmd_ver(const std::vector<std::string>& args) {
    (void)args;
    std::cout << CYAN << BOLD << "BarsikCMD версия 2026.3.5" << RESET << "\n";
}

void cmd_exit(const std::vector<std::string>& args) {
    (void)args;
    std::cout << RED << BOLD << "Пока!" << RESET << "\n";
    exit(0);
}

void cmd_info(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << YELLOW << "Использование: " << BOLD << "info <аргумент>" << RESET << "\n";
        std::cout << "Доступные аргументы:\n";
        std::cout << "  " << GREEN << BOLD << "links" << RESET << " — ссылки проекта\n";
        return;
    }

    const std::string& sub = args[1];

    if (sub == "links") {
        std::cout << BOLD << YELLOW << "Ссылки проекта BarsikCMD:\n" << RESET;
        std::cout << CYAN << "─────────────────────────────────────────\n" << RESET;
        std::cout << "  " << GREEN << "GitHub:  " << RESET << "https://github.com/barsik0396/BarsikCMD\n";
        std::cout << "  " << GREEN << "Релизы: " << RESET << "https://github.com/barsik0396/BarsikCMD/releases\n";
        std::cout << CYAN << "─────────────────────────────────────────\n" << RESET;
    } else {
        std::cout << RED << "Неизвестный аргумент: " << sub << RESET << "\n";
        std::cout << "Введи " << BOLD << "info" << RESET << " без аргументов для списка доступных.\n";
    }
}

void cmd_reload(const std::vector<std::string>& args) {
    (void)args;
    std::cout << CYAN << "Перезапуск BarsikCMD..." << RESET << "\n";
    execv(Shell::s_argv[0], Shell::s_argv);
    // Если execv вернул управление — что-то пошло не так
    std::cout << RED << "Ошибка: не удалось перезапустить BarsikCMD." << RESET << "\n";
}