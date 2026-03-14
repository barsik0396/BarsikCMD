#include "commands.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cstdlib>

#ifdef _WIN32
    #include <cstdlib>
    #define CLEAR_CMD "cls"
#else
    #define CLEAR_CMD "clear"
#endif

// Цвета
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define WHITE   "\033[97m"

struct CommandInfo {
    std::string description;
};

static const std::map<std::string, std::string> CMD_DESCRIPTIONS = {
    { "help",  "показать список команд" },
    { "echo",  "вывести текст на экран" },
    { "clear", "очистить экран" },
    { "ver",   "показать версию BarsikCMD" },
    { "exit",   "выйти из BarsikCMD" },
    { "update", "проверить и установить обновления" },
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
    std::cout << CYAN << BOLD << "BarsikCMD версия 2026.3.3" << RESET << "\n";
}

void cmd_exit(const std::vector<std::string>& args) {
    (void)args;
    std::cout << RED << BOLD << "Пока!" << RESET << "\n";
    exit(0);
}

// ─── Вспомогательные функции для update ───────────────────────────────────

static size_t curlWriteCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

static std::string fetchUrl(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "";
    return response;
}

// Простой поиск строкового значения в JSON по ключу
// Ищет "key": "value" и возвращает value
static std::string jsonGetString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";

    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return "";

    size_t end = json.find('"', pos + 1);
    if (end == std::string::npos) return "";

    return json.substr(pos + 1, end - pos - 1);
}

// Возвращает подстроку JSON внутри объекта с нужным ключом
static std::string jsonGetObject(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find('{', pos + search.size());
    if (pos == std::string::npos) return "";

    int depth = 0;
    size_t start = pos;
    for (size_t i = pos; i < json.size(); ++i) {
        if (json[i] == '{') depth++;
        else if (json[i] == '}') {
            depth--;
            if (depth == 0) return json.substr(start, i - start + 1);
        }
    }
    return "";
}

// Получить числовое значение (без кавычек): "key": 123
static int jsonGetInt(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return -1;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return -1;

    // Пропускаем пробелы
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n')) pos++;

    std::string num;
    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
        num += json[pos++];
    }
    if (num.empty()) return -1;
    return std::stoi(num);
}

// ─── Команда update ────────────────────────────────────────────────────────

static const std::string CURRENT_VERSION = "2026.3.3";
static const int CURRENT_VERSION_ID = 3; // третий компонент версии (релиз за день)
static const std::string UPDATE_CHANNEL = "preview";
static const std::string UPDATES_URL =
    "https://raw.githubusercontent.com/barsik0396/BarsikCMD/refs/heads/main/server/updates.json";

void cmd_update(const std::vector<std::string>& args) {
    (void)args;

    std::cout << CYAN << "Проверка обновлений..." << RESET << "\n";

    std::string json = fetchUrl(UPDATES_URL);
    if (json.empty()) {
        std::cout << RED << "Ошибка: не удалось получить информацию об обновлениях." << RESET << "\n";
        return;
    }

    // Получаем последнюю версию для канала
    std::string latestObj = jsonGetObject(json, "latest");
    if (latestObj.empty()) {
        std::cout << RED << "Ошибка: неверный формат updates.json (нет 'latest')." << RESET << "\n";
        return;
    }

    std::string latestVersion = jsonGetString(latestObj, UPDATE_CHANNEL);
    if (latestVersion.empty()) {
        std::cout << RED << "Ошибка: не найден канал '" << UPDATE_CHANNEL << "'." << RESET << "\n";
        return;
    }

    // Уже актуальная версия
    if (latestVersion == CURRENT_VERSION) {
        std::cout << GREEN << BOLD << "Обновлений нет. У вас актуальная версия (" << CURRENT_VERSION << ")." << RESET << "\n";
        return;
    }

    std::cout << YELLOW << "Доступна новая версия: " << BOLD << latestVersion << RESET << "\n";

    // Получаем объект updates -> <latestVersion>
    std::string updatesObj = jsonGetObject(json, "updates");
    if (updatesObj.empty()) {
        std::cout << RED << "Ошибка: неверный формат updates.json (нет 'updates')." << RESET << "\n";
        return;
    }

    std::string versionObj = jsonGetObject(updatesObj, latestVersion);
    if (versionObj.empty()) {
        std::cout << RED << "Ошибка: не найдены данные для версии " << latestVersion << "." << RESET << "\n";
        return;
    }

    // Проверяем min-ver
    int minVer = jsonGetInt(versionObj, "min-ver");
    if (minVer > CURRENT_VERSION_ID) {
        std::cout << RED << "Для установки этого обновления требуется версия с ID >= "
                  << minVer << ".\n"
                  << "Ваш текущий ID версии: " << CURRENT_VERSION_ID << ".\n"
                  << "Пожалуйста, установите более новую версию вручную." << RESET << "\n";
        return;
    }

    // Получаем ссылку для MINT
    std::string mintObj = jsonGetObject(versionObj, "MINT");
    if (mintObj.empty()) {
        std::cout << RED << "Ошибка: нет данных для платформы MINT." << RESET << "\n";
        return;
    }

    std::string link = jsonGetString(mintObj, "link");
    if (link.empty()) {
        std::cout << RED << "Ошибка: не найдена ссылка на пакет." << RESET << "\n";
        return;
    }

    std::cout << CYAN << "Скачиваю пакет..." << RESET << "\n";

    std::string tmpFile = "/tmp/barsikcmd_update.deb";
    std::string downloadCmd = "curl -L --fail -o " + tmpFile + " \"" + link + "\"";
    int ret = system(downloadCmd.c_str());
    if (ret != 0) {
        std::cout << RED << "Ошибка: не удалось скачать пакет (curl вернул ошибку)." << RESET << "\n";
        return;
    }

    // Проверяем что файл не пустой и похож на deb (минимум 100 байт)
    FILE* f = fopen(tmpFile.c_str(), "rb");
    if (!f) {
        std::cout << RED << "Ошибка: файл не найден после скачивания." << RESET << "\n";
        return;
    }
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fclose(f);

    if (fileSize < 100) {
        std::cout << RED << "Ошибка: скачанный файл повреждён или пустой (" << fileSize << " байт).\n"
                  << "Проверь ссылку в updates.json: " << link << RESET << "\n";
        return;
    }

    std::cout << CYAN << "Размер пакета: " << fileSize / 1024 << " КБ\n" << RESET;
    std::cout << CYAN << "Устанавливаю пакет (потребуется пароль)..." << RESET << "\n";
    std::string installCmd = "pkexec dpkg -i " + tmpFile;
    ret = system(installCmd.c_str());
    if (ret != 0) {
        std::cout << RED << "Ошибка при установке пакета." << RESET << "\n";
        return;
    }

    std::cout << GREEN << BOLD << "BarsikCMD успешно обновлён до версии " << latestVersion << "!" << RESET << "\n";
}