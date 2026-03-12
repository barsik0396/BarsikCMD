#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>

void cmd_help(const std::map<std::string, std::function<void(const std::vector<std::string>&)>>& commands,
              const std::vector<std::string>& args);
void cmd_echo(const std::vector<std::string>& args);
void cmd_clear(const std::vector<std::string>& args);
void cmd_ver(const std::vector<std::string>& args);
void cmd_exit(const std::vector<std::string>& args);