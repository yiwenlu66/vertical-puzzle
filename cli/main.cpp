#include <iostream>
#include <fstream>
#include "parsers.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: vertical-puzzle-cli <path-to-puzzle>" << std::endl;
        return 0;
    }
    std::ifstream f(argv[1]);
    if (!f.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return 0;
    }
    try {
        TextStreamParser text_parser(f);
        for (auto const &multiple_constraint : text_parser.get_multiple_constraints()) {
            std::cout << multiple_constraint << std::endl;
        }
        std::cout << std::endl;
        MultipleConstraintParser multiple_constraint_parser(text_parser.get_multiple_constraints());
        for (auto const &constraint_chain : multiple_constraint_parser.get_constraint_chains()) {
            std::cout << constraint_chain << std::endl;
        }
    } catch (TextStreamParserException &e) {
        std::cout << "Parser error at line " << e.lineno << ", column " << e.column << ": " << e.what();
    } catch (std::runtime_error &e) {
        std::cout << e.what();
    }
    return 0;
}