#include "cli.h"
#include <fstream>
#include "parsers.h"
#include "solver.h"

std::vector<std::vector<int> > vertical_puzzle_cli(std::ostream &ostream, std::istream &istream, int max_solutions, bool verbose)
{
    try {
        TextStreamParser text_parser(istream);
        if (verbose) {
            for (auto const &multiple_constraint : text_parser.get_multiple_constraints()) {
                ostream << multiple_constraint << std::endl;
            }
            ostream << std::endl;
        }
        MultipleConstraintParser multiple_constraint_parser(text_parser.get_multiple_constraints());
        if (verbose) {
            for (auto const &constraint_chain : multiple_constraint_parser.get_constraint_chains()) {
                ostream << constraint_chain << std::endl;
            }
            ostream << std::endl;
        }

        auto start_t = std::chrono::high_resolution_clock::now();
        Solver solver(multiple_constraint_parser.get_constraint_chains(), text_parser.get_explicit_variables(),
                      max_solutions);
        auto end_t = std::chrono::high_resolution_clock::now();
        ostream << "Found " << solver.get_solutions().size() << " solution(s) in "
               << std::chrono::duration_cast<std::chrono::microseconds>(end_t - start_t).count()
               << " microseconds." << std::endl << std::endl;
        std::vector<std::vector<int> > result(solver.get_solutions().size());
        int i = 0;
        for (auto const &solution : solver.get_solutions()) {
            ostream << "Solution " << i + 1 << ':' << std::endl;
            for (int j = 0; j < 10; ++j) {
                int no = text_parser.get_lut_letter_to_explicit_variable_no()[j];
                if (no != -1) {
                    ostream << static_cast<char>('A' + j) << '=' << solution[no] << std::endl;
                }
                result[i].push_back(no == -1 ? -1 : solution[no]);
            }
            ++i;
            ostream << std::endl;
        }
        return result;
    } catch (TextStreamParserException &e) {
        ostream << "Parser error at line " << e.lineno << ", column " << e.column << ": " << e.what();
    } catch (std::runtime_error &e) {
        ostream << e.what();
    }
    return std::vector<std::vector<int> >();
}

