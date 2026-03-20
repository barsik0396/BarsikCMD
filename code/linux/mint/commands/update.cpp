#include "update.h"
#include "colors.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cstdlib>

// ─── Вспомогательные функции ───────────────────────────────────────────────

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

static int jsonGetInt(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return -1;

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return -1;

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

static const std::string CURRENT_VERSION = "2026.3.4";
static const int CURRENT_VERSION_ID = 4;
static const std::string UPDATE_CHANNEL = "preview";
static const std::string UPDATES_URL =
    "https://raw.githubusercontent.com/barsik0396/BarsikCMD/refs/heads/main/server/updates.json";

std::string check_for_update_silent() {
    std::string json = fetchUrl(UPDATES_URL);
    if (json.empty()) {
        // Пробуем отличить отсутствие сети от других ошибок через curl напрямую
        CURL* curl = curl_easy_init();
        if (!curl) return "ERR";
        curl_easy_setopt(curl, CURLOPT_URL, UPDATES_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == CURLE_COULDNT_RESOLVE_HOST || res == CURLE_COULDNT_CONNECT
            || res == CURLE_OPERATION_TIMEDOUT) {
            return "ERR_NO_NET";
        }
        return "ERR";
    }

    std::string latestObj = jsonGetObject(json, "latest");
    if (latestObj.empty()) return "ERR";

    std::string latestVersion = jsonGetString(latestObj, UPDATE_CHANNEL);
    if (latestVersion.empty()) return "ERR";

    if (latestVersion == CURRENT_VERSION) return "";
    return latestVersion;
}

void cmd_update(const std::vector<std::string>& args) {
    (void)args;

    std::cout << CYAN << "Проверка обновлений..." << RESET << "\n";

    std::string json = fetchUrl(UPDATES_URL);
    if (json.empty()) {
        std::cout << RED << "Ошибка: не удалось получить информацию об обновлениях." << RESET << "\n";
        return;
    }

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

    if (latestVersion == CURRENT_VERSION) {
        std::cout << GREEN << BOLD << "Обновлений нет. У вас актуальная версия (" << CURRENT_VERSION << ")." << RESET << "\n";
        return;
    }

    std::cout << YELLOW << "Доступна новая версия: " << BOLD << latestVersion << RESET << "\n";

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

    int minVer = jsonGetInt(versionObj, "min-ver");
    if (minVer > CURRENT_VERSION_ID) {
        std::cout << RED << "Для установки этого обновления требуется версия с ID >= "
                  << minVer << ".\n"
                  << "Ваш текущий ID версии: " << CURRENT_VERSION_ID << ".\n"
                  << "Пожалуйста, установите более новую версию вручную." << RESET << "\n";
        return;
    }

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
    std::cout << CYAN << "Введи 'reload' чтобы перезапустить BarsikCMD с новой версией." << RESET << "\n";
}