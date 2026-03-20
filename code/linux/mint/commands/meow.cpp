#include "meow.h"
#include "colors.h"
#include <iostream>
#include <cstdlib>

void cmd_meow(const std::vector<std::string>& args) {
    (void)args;

    static int lastCat = -1;
    int cat;
    do { cat = rand() % 5; } while (cat == lastCat);
    lastCat = cat;

    std::cout << CYAN;
    switch (cat) {
        case 0:
            std::cout <<
                "  /\\_____/\\\n"
                " (  o   o  )\n"
                " =( Y  Y )=\n"
                "  )      (\n"
                " (_|----|_)\n";
            break;
        case 1:
            std::cout <<
                " /\\_/\\\n"
                "( o.o )\n"
                " > ^ <\n";
            break;
        case 2:
            std::cout <<
                "  ,_,\n"
                " (o,o)\n"
                " {\"\"\"}\n"
                "  \" \"\n";
            break;
        case 3:
            std::cout <<
                "  ^~^\n"
                " (- -)\n"
                " /|  |\\\n"
                "(  --  )\n"
                "  vv vv\n";
            break;
        case 4:
            std::cout <<
                " |\\_/|\n"
                " | @ @   Woof?\n"
                " |   <>\n"
                "  \\  _/\n"
                "  ||\n";
            break;
    }
    std::cout << RESET;
}