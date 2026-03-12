#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>

class Shell {
public:
    Shell();
    void run();

private:
    std::string prompt;
    std::vector<std::string> history;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commands;

    void registerCommands();
    void execute(const std::string& input);
    std::vector<std::string> tokenize(const std::string& input);
};