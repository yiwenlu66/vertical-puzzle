#include <iostream>
#include <fstream>
#include <chrono>
#include "parsers.h"
#include "solver.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: vertical-puzzle-cli <path-to-puzzle> <max-solutions>" << std::endl;
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
        std::cout << std::endl;

        auto start_t = std::chrono::high_resolution_clock::now();
        Solver solver(multiple_constraint_parser.get_constraint_chains(), text_parser.get_explicit_variables(),
                      std::stoi(argv[2]));
        auto end_t = std::chrono::high_resolution_clock::now();
        std::cout <<
                  "Found " << solver.get_solutions().size() << " solution(s) in "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end_t - start_t).count()
                  << " microseconds." << std::endl << std::endl;
        int i = 1;
        for (auto const &solution : solver.get_solutions()) {
            std::cout << "Solution " << i++ << ':' << std::endl;
            for (int j = 0; j < 10; ++j) {
                int no = text_parser.get_lut_letter_to_explicit_variable_no()[j];
                if (no != -1) {
                    std::cout << static_cast<char>('A' + j) << '=' << solution[no] << std::endl;
                }
            }
            std::cout << std::endl;
        }
    } catch (TextStreamParserException &e) {
        std::cout << "Parser error at line " << e.lineno << ", column " << e.column << ": " << e.what();
    } catch (std::runtime_error &e) {
        std::cout << e.what();
    }
    return 0;
}