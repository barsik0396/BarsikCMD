#include "input.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

// ─── Таблица цветов команд ────────────────────────────────────────────────

const std::vector<CommandColor> COMMAND_COLORS = {
    { "meow",            "\033[38;2;245;122;7m" },
    { "purr",            "\033[38;2;245;122;7m" },
    { "info",            "\033[38;2;0;110;255m" },
    { "help",            "\033[38;2;0;110;255m" },
    { "update",          "\033[38;2;0;110;255m" },
    { "ver",             "\033[38;2;0;110;255m" },
    { "clear",           "\033[32m"             },
    { "reload",          "\033[32m"             },
    { "exit",            "\033[31m"             },
    { "file",            "\033[33m"             },
    { "echo",            "\033[97m"             },
    { "change-channel",  "\033[35m"             },
    { "install-version", "\033[35m"             },
};

static const std::vector<std::string> INFO_ARGS  = { "links" };
static const std::vector<std::string> FILE_ARGS  = { "new", "del", "set", "edit", "move" };

// ─── Подсветка буфера ─────────────────────────────────────────────────────

static std::string colorizeBuffer(const char* buf) {
    if (!buf || buf[0] == '\0') return "";

    std::string line(buf);
    size_t cmdEnd = 0;
    while (cmdEnd < line.size() && line[cmdEnd] != ' ') cmdEnd++;
    std::string cmd  = line.substr(0, cmdEnd);
    std::string rest = line.substr(cmdEnd);

    std::string cmdColor;
    for (const auto& cc : COMMAND_COLORS)
        if (cc.command == cmd) { cmdColor = cc.ansi_color; break; }

    if (cmdColor.empty()) return line;

    if (cmd == "echo")
        return cmdColor + cmd + "\033[0m\033[97m" + rest + "\033[0m";

    if (cmd == "info" && !rest.empty()) {
        size_t argStart = rest.find_first_not_of(' ');
        if (argStart != std::string::npos) {
            std::string spaces = rest.substr(0, argStart);
            std::string arg    = rest.substr(argStart);
            bool known = false;
            for (const auto& a : INFO_ARGS)
                if (arg == a) { known = true; break; }
            std::string argColor = known ? "\033[32m" : "\033[31m";
            return cmdColor + cmd + "\033[0m" + spaces + argColor + arg + "\033[0m";
        }
    }

    // file: подсветка аргумента (new/del/set/edit/move)
    if (cmd == "file" && !rest.empty()) {
        size_t argStart = rest.find_first_not_of(' ');
        if (argStart != std::string::npos) {
            std::string spaces = rest.substr(0, argStart);
            // Берём только первое слово аргумента
            size_t argEnd = rest.find(' ', argStart);
            std::string arg  = rest.substr(argStart, argEnd == std::string::npos ? std::string::npos : argEnd - argStart);
            std::string tail = argEnd == std::string::npos ? "" : rest.substr(argEnd);
            bool known = false;
            for (const auto& a : FILE_ARGS)
                if (arg == a) { known = true; break; }
            std::string argColor = known ? "\033[32m" : "\033[31m";
            return cmdColor + cmd + "\033[0m" + spaces + argColor + arg + "\033[0m\033[97m" + tail + "\033[0m";
        }
    }

    return cmdColor + cmd + "\033[0m" + rest;
}

// ─── Кастомная перерисовка ────────────────────────────────────────────────

static std::string g_rawPrompt;

static int visibleLen(const std::string& s) {
    int len = 0;
    bool inEsc = false, inIgnore = false;
    for (char c : s) {
        if (c == '\001') { inIgnore = true;  continue; }
        if (c == '\002') { inIgnore = false; continue; }
        if (inIgnore) continue;
        if (c == '\033') { inEsc = true; continue; }
        if (inEsc) {
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) inEsc = false;
            continue;
        }
        len++;
    }
    return len;
}

static void colorRedisplay() {
    if (!rl_line_buffer) { rl_redisplay(); return; }

    std::string colored = colorizeBuffer(rl_line_buffer);
    int promptLen    = visibleLen(g_rawPrompt);
    int cursorOffset = rl_point; // байты до курсора в оригинальном буфере

    fprintf(rl_outstream, "\r%s%s\033[0m\033[K",
            g_rawPrompt.c_str(), colored.c_str());

    int targetCol = promptLen + cursorOffset;
    if (targetCol > 0)
        fprintf(rl_outstream, "\r\033[%dC", targetCol);
    else
        fprintf(rl_outstream, "\r");

    fflush(rl_outstream);
}

// ─── Ctrl+C через SIGINT ──────────────────────────────────────────────────

static volatile sig_atomic_t g_gotSigint = 0;

static void sigintHandler(int) {
    g_gotSigint = 1;
    // Выводим сообщение прямо из обработчика (async-signal-safe функции)
    const char* msg = "\n\033[33mДля выхода введи 'exit' или нажми Ctrl+D.\033[0m\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    // Говорим readline что строка пустая и нужно перерисовать промпт
    rl_replace_line("", 0);
    rl_point = 0;
    rl_on_new_line();
    rl_redisplay();
}

// ─── Публичный API ────────────────────────────────────────────────────────

void input_init() {
    // Перехватываем SIGINT — без SA_RESTART чтобы readline прервался
    struct sigaction sa {};
    sa.sa_handler = sigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);

    rl_catch_signals   = 0;
    rl_catch_sigwinch  = 0;

    rl_redisplay_function = colorRedisplay;
    rl_variable_bind("bell-style", "none");
    using_history();
}

bool input_readline(const std::string& rawPrompt, std::string& out) {
    g_rawPrompt = rawPrompt;

    // Оборачиваем ANSI в \001..\002
    std::string wrapped;
    bool inEsc = false;
    std::string escBuf;
    for (char c : rawPrompt) {
        if (c == '\033') { inEsc = true; escBuf = c; }
        else if (inEsc) {
            escBuf += c;
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                wrapped += "\001" + escBuf + "\002";
                escBuf.clear(); inEsc = false;
            }
        } else { wrapped += c; }
    }

    while (true) {
        g_gotSigint = 0;
        char* line = readline(wrapped.c_str());

        if (g_gotSigint) {
            // SIGINT прервал readline — сообщение уже выведено в обработчике
            if (line) free(line);
            g_gotSigint = 0;
            continue; // перезапускаем readline
        }

        if (!line) {
            // Ctrl+D
            fprintf(rl_outstream, "\n\033[1m\033[31mПока!\033[0m\n");
            fflush(rl_outstream);
            return false;
        }

        // Перевод строки после Enter — readline не делает это сам
        fprintf(rl_outstream, "\n\033[0m");
        fflush(rl_outstream);

        out = std::string(line);
        free(line);
        return true;
    }
}

void input_add_history(const std::string& line) {
    if (!line.empty()) add_history(line.c_str());
}