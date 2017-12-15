#include <iostream>
#include <fstream>
#include "parsers.h"
#include "cli.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: vertical-puzzle-cli <path-to-puzzle> <max-solutions> [-v]" << std::endl;
        return 0;
    }
    bool verbose = false;
    if (argc >= 4 && !strcmp(argv[3], "-v")) {
        verbose = true;
    }
    std::ifstream f(argv[1]);
    if (!f.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return 0;
    }
    vertical_puzzle_cli(std::cout, f, std::stoi(argv[2]), verbose);
    return 0;
}