#include "cli.h"
#include <fstream>
#include "parsers.h"
#include "solver.h"

void vertical_puzzle_cli(std::ostream &stream, std::string path_to_puzzle, int max_solutions, bool verbose)
{
    std::ifstream f(path_to_puzzle);
    if (!f.is_open()) {
        stream << "Failed to open file!" << std::endl;
        return;
    }
    try {
        TextStreamParser text_parser(f);
        if (verbose) {
            for (auto const &multiple_constraint : text_parser.get_multiple_constraints()) {
                stream << multiple_constraint << std::endl;
            }
            stream << std::endl;
        }
        MultipleConstraintParser multiple_constraint_parser(text_parser.get_multiple_constraints());
        if (verbose) {
            for (auto const &constraint_chain : multiple_constraint_parser.get_constraint_chains()) {
                stream << constraint_chain << std::endl;
            }
            stream << std::endl;
        }

        auto start_t = std::chrono::high_resolution_clock::now();
        Solver solver(multiple_constraint_parser.get_constraint_chains(), text_parser.get_explicit_variables(),
                      max_solutions);
        auto end_t = std::chrono::high_resolution_clock::now();
        stream << "Found " << solver.get_solutions().size() << " solution(s) in "
               << std::chrono::duration_cast<std::chrono::microseconds>(end_t - start_t).count()
               << " microseconds." << std::endl << std::endl;
        int i = 1;
        for (auto const &solution : solver.get_solutions()) {
            stream << "Solution " << i++ << ':' << std::endl;
            for (int j = 0; j < 10; ++j) {
                int no = text_parser.get_lut_letter_to_explicit_variable_no()[j];
                if (no != -1) {
                    stream << static_cast<char>('A' + j) << '=' << solution[no] << std::endl;
                }
            }
            stream << std::endl;
        }
    } catch (TextStreamParserException &e) {
        stream << "Parser error at line " << e.lineno << ", column " << e.column << ": " << e.what();
    } catch (std::runtime_error &e) {
        stream << e.what();
    }
}
