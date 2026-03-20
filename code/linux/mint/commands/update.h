#pragma once
#include <vector>
#include <string>

void cmd_update(const std::vector<std::string>& args);
// Тихая проверка обновлений при запуске. Возвращает новую версию или "".
// "ERR_NO_NET" если нет интернета, "ERR" при другой ошибке.
std::string check_for_update_silent();