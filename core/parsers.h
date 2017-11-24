#include <istream>
#include <tuple>
#include <stdexcept>
#include "constraint.h"


class TextStreamParser {

public:
    explicit TextStreamParser(std::istream &stream) {
        _explicit_variables.reserve(10);
        for (int i = 0; i < 10; ++i) {
            _constants.emplace_back(Constant(i));
        }
        _parse_stream(stream);
    }
    std::vector<MultipleConstraint> const &get_multiple_constraints() {
        return _multiple_constraints;
    }
    std::vector<ExplicitVariable> const &get_explicit_variables() {
        return _explicit_variables;
    }
    std::vector<int> const &get_lut_letter_to_explicit_variable_no() {
        return _lut_letter_to_explicit_variable_no;
    }

private:
    std::vector<MultipleConstraint> _multiple_constraints;
    std::vector<ExplicitVariable> _explicit_variables;
    std::vector<Constant> _constants;
    std::vector<int> _lut_letter_to_explicit_variable_no = std::vector<int>(10, -1);

    enum {
        START,
        WAIT_OPTR,
        ADD_SUB_IN,
        ADD_SUB_OUT,
        MUL_IN,
        MUL_INTERMEDIATE,
        MUL_OUT
    } _parser_state = START;
    void _parse_stream(std::istream &stream);

    enum {
        READ_OPERATOR,
        HORIZ_LINE,
        BLANK_BEFORE_OPERAND,
        READ_OPERAND,
        BLANK_TRAILING
    } _line_parser_state = READ_OPERATOR;

    // _parse_line: return operator, operand, position of operator,
    // and number of columns (up to the last non-blank character)
    std::tuple<char, std::vector<PlaceHolder *>, int, int> _parse_line(std::string line, int lineno);
};

struct TextStreamParserException : std::runtime_error {
    int lineno, column;
    TextStreamParserException(int lineno, int column, std::string msg)
            : std::runtime_error(msg), lineno(lineno), column(column) {}
};