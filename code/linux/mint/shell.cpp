#include "shell.h"
#include "commands.h"
#include "commands/update.h"
#include "commands/colors.h"
#include "input.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <set>
#include <unistd.h>
#include <csignal>

int   Shell::s_argc = 0;
char** Shell::s_argv = nullptr;

Shell::Shell() {
    prompt = "\033[1m\033[33mBarsikCMD>\033[0m ";
    srand(static_cast<unsigned>(time(nullptr)));
    input_init();
    registerCommands();
}

void Shell::registerCommands() {
    commands["help"]    = [this](const std::vector<std::string>& args) { cmd_help(commands, args); };
    commands["echo"]    = [](const std::vector<std::string>& args)     { cmd_echo(args); };
    commands["clear"]   = [](const std::vector<std::string>& args)     { cmd_clear(args); };
    commands["ver"]     = [](const std::vector<std::string>& args)     { cmd_ver(args); };
    commands["exit"]    = [](const std::vector<std::string>& args)     { cmd_exit(args); };
    commands["update"]  = [](const std::vector<std::string>& args)     { cmd_update(args); };
    commands["meow"]    = [](const std::vector<std::string>& args)     { cmd_meow(args); };
    commands["info"]    = [](const std::vector<std::string>& args)     { cmd_info(args); };
    commands["reload"]  = [](const std::vector<std::string>& args)     { cmd_reload(args); };
    commands["file"]    = [](const std::vector<std::string>& args)     { cmd_file(args); };
}

// Проверяет содержит ли строка ANSI escape-последовательности
static bool containsAnsi(const std::string& s) {
    for (size_t i = 0; i + 1 < s.size(); ++i) {
        if (s[i] == '\033' && s[i+1] == '[') return true;
    }
    return false;
}

void Shell::run(int argc, char* argv[]) {
    s_argc = argc;
    s_argv = argv;

    // ── Парсинг аргументов ────────────────────────────────────────────────
    std::set<std::string> flags;
    std::string runCmd;
    bool hasRun  = false;
    bool isRun2  = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-run" || arg == "-run2") {
            // -run/-run2 нельзя комбинировать с другими флагами
            if (!flags.empty()) {
                std::cout << RED << "Ошибка: -run/-run2 нельзя комбинировать с другими аргументами." << RESET << "\n";
                return;
            }
            if (i + 1 >= argc) {
                std::cout << RED << "Ошибка: после " << arg << " нужно указать команду." << RESET << "\n";
                return;
            }
            isRun2 = (arg == "-run2");
            hasRun = true;
            // Собираем команду из оставшихся аргументов
            for (int j = i + 1; j < argc; ++j) {
                if (!runCmd.empty()) runCmd += " ";
                runCmd += argv[j];
            }
            break;
        }

        flags.insert(arg);
    }

    // Блокируем -run/-run2 если есть другие флаги
    if (hasRun && !flags.empty()) {
        std::cout << RED << "Ошибка: -run/-run2 нельзя комбинировать с другими аргументами." << RESET << "\n";
        return;
    }

    // ── Обработка -run / -run2 ────────────────────────────────────────────
    if (hasRun) {
        execute(runCmd);
        if (!isRun2) return;
    }

    // ── Обработка -help ───────────────────────────────────────────────────
    if (flags.count("-help")) {
        std::cout << BOLD << YELLOW << "BarsikCMD — доступные аргументы запуска:\n" << RESET;
        std::cout << CYAN << "──────────────────────────────────────────────────────\n" << RESET;
        std::cout << "  " << GREEN << BOLD << "-help" << RESET                          << "                        — показать этот список\n";
        std::cout << "  " << GREEN << BOLD << "-run <команда>" << RESET                 << "               — выполнить команду и закрыть\n";
        std::cout << "  " << GREEN << BOLD << "-run2 <команда>" << RESET                << "              — выполнить команду и остаться\n";
        std::cout << "  " << GREEN << BOLD << "-disable-version-check-on-init" << RESET << " — не проверять обновления при запуске\n";
        std::cout << "  " << GREEN << BOLD << "-load-type-select" << RESET              << "            — (заглушка, будет реализовано позже)\n";
        std::cout << CYAN << "──────────────────────────────────────────────────────\n" << RESET;
        return;
    }

    // ── Обработка -load-type-select ───────────────────────────────────────
    if (flags.count("-load-type-select")) {
        std::cout << "Аргумент принят.\n";
    }

    // ── Приветствие ───────────────────────────────────────────────────────
    std::cout << "\033[1m\033[33mBarsikCMD v2026.3.5\033[0m — введи 'help' для списка команд\n\n";

    // ── Проверка обновлений при запуске ───────────────────────────────────
    if (!flags.count("-disable-version-check-on-init")) {
        std::string result = check_for_update_silent();
        if (result == "ERR_NO_NET") {
            std::cout << YELLOW << "Предупреждение: нет подключения к интернету, проверка обновлений пропущена." << RESET << "\n\n";
        } else if (result == "ERR") {
            std::cout << YELLOW << "Предупреждение: не удалось проверить обновления." << RESET << "\n\n";
        } else if (!result.empty()) {
            std::cout << GREEN << BOLD << "Доступно обновление: v" << result
                      << ". Введи 'update' для установки." << RESET << "\n\n";
        }
    }

    // ── Основной цикл ─────────────────────────────────────────────────────

    // Ctrl+\ (SIGQUIT) — игнорировать
    struct sigaction sa_quit {};
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, nullptr);

    std::string input;
    while (true) {
        if (!input_readline(prompt, input)) break;

        if (input.empty()) continue;

        if (containsAnsi(input)) {
            std::cout << RED << "Ошибка: использование управляющих escape-последовательностей запрещено." << RESET << "\n";
            continue;
        }

        input_add_history(input);
        history.push_back(input);
        execute(input);
    }
}

void Shell::execute(const std::string& input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return;

    const std::string& cmd = tokens[0];

    // ── Секреты (незадокументировано) ─────────────────────────────────────

    // "purr" — ласковый кот
    if (cmd == "purr") {
        std::cout << "\033[35m"
            "  /\\_/\\\n"
            " ( ^.^ )  Мурр... ты нашёл меня.\n"
            "  >🐟 <\n"
            "\033[0m";
        return;
    }

    // "meow meow meow" — особый ответ на тройной meow
    static int meowStreak = 0;
    if (cmd == "meow") {
        meowStreak++;
        if (meowStreak >= 3) {
            meowStreak = 0;
            std::cout << "\033[35m\033[1m"
                "  /\\_/\\  /\\_/\\  /\\_/\\\n"
                " ( o.o )( o.o )( o.o )\n"
                "  > ^ <  > ^ <  > ^ <\n"
                "  ВСЕ КОТЫ ПРИШЛИ!\n"
                "\033[0m";
            return;
        }
    } else {
        meowStreak = 0;
    }
    auto it = commands.find(cmd);
    if (it != commands.end()) {
        it->second(tokens);
    } else {
        std::cout << RED << "Неизвестная команда: " << cmd << ". Введи 'help'." << RESET << "\n";
    }
}

std::vector<std::string> Shell::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}