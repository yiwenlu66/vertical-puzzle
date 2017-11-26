#include "parsers.h"
#include <vector>
#include <sstream>
#include "utils.h"

void TextStreamParser::_parse_stream(std::istream &stream) {
    int lineno = 0;
    std::string line;
    std::vector<std::vector<PlaceHolder *> > addends;
    std::vector<PlaceHolder *> multiplicand, multiplier;
    std::string rpn;
    int prev_width = 0;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            throw TextStreamParserException(lineno + 1, 1, "empty line");
        }
        auto parsed_line = _parse_line(line, ++lineno);
        auto optr = std::get<0>(parsed_line);
        auto opnd = std::get<1>(parsed_line);
        auto optr_pos = std::get<2>(parsed_line);
        auto width = std::get<3>(parsed_line);
        switch (_parser_state) {
            case START:
                switch (optr) {
                    case '\0':
                    case '+':
                        addends.push_back(opnd);
                        rpn = "_";
                        _parser_state = WAIT_OPTR;
                        break;
                    default:
                        throw TextStreamParserException(lineno, optr_pos, "unexpected operator");
                }
                break;
            case WAIT_OPTR:
                switch (optr) {
                    case '\0':
                        throw TextStreamParserException(lineno, optr_pos, "expected an operator");
                    case '+':
                    case '-':
                    case '*':
                        if (width != prev_width) {
                            throw TextStreamParserException(lineno, width,
                                                            "expected the line width to match the previous line");
                        }
                        if (optr == '*') {
                            multiplicand = addends[0];
                            multiplier = opnd;
                            addends.clear();
                            rpn.clear();
                            _parser_state = MUL_IN;
                        } else {
                            addends.push_back(opnd);
                            rpn.append("_").append(1, optr);
                            _parser_state = ADD_SUB_IN;
                        }
                        break;
                    default:
                        throw TextStreamParserException(lineno, optr_pos,
                                                        "expected an operator; got a horizontal line instead");
                }
                break;
            case ADD_SUB_IN:
                switch (optr) {
                    case '\0':
                        throw TextStreamParserException(lineno, optr_pos, "expected an operator or a horizontal line");
                    case '+':
                    case '-':
                        if (width != prev_width) {
                            throw TextStreamParserException(lineno, width,
                                                            "expected the line width to match the previous line");
                        }
                        addends.push_back(opnd);
                        rpn.append("_").append(1, optr);
                        break;
                    case '*':
                        throw TextStreamParserException(lineno, optr_pos,
                                                        "expected '+', '-' or a horizontal line; got '*' instead");
                    default:
                        _parser_state = ADD_SUB_OUT;
                        break;
                }
                break;
            case ADD_SUB_OUT:
                if (optr != '\0') {
                    throw TextStreamParserException(lineno, optr_pos, "unexpected operator");
                }
                if (width != prev_width) {
                    throw TextStreamParserException(lineno, width,
                                                    "expected the width of result to match those of operands");
                }
                _multiple_constraints.emplace_back(addends, opnd, rpn);
                addends.clear();
                addends.push_back(opnd);
                rpn = "_";
                _parser_state = WAIT_OPTR;
                break;
            case MUL_IN:
                if (optr != '_') {
                    throw TextStreamParserException(lineno, optr_pos, "expected horizontal line");
                }
                _parser_state = MUL_INTERMEDIATE;
                break;
            case MUL_INTERMEDIATE: {
                auto i = addends.size();

                // check operator
                if (optr != '\0' && optr != '+') {
                    throw TextStreamParserException(lineno, optr_pos, "unexpected operator");
                }

                // check width
                if (i == 0) {
                    if (width != prev_width) {
                        throw TextStreamParserException(lineno, width,
                                            "expected the width to match those of the multiplicand and the multiplier");
                    }
                } else if (width != prev_width - 1) {
                    throw TextStreamParserException(lineno, width,
                                              "expected this line to shift exact 1 column left from the previous line");
                }

                // add constraint for the multiplicand multiplied by a single digit of the multiplier
                {
                    std::vector<std::vector<PlaceHolder *> > input;
                    input.push_back(multiplicand);
                    input.emplace_back(1, multiplier[multiplier.size() - 1 - i]);
                    _multiple_constraints.emplace_back(input, opnd, "__*");
                }

                // store the partial product for later addition
                for (int j = 0; j < i; ++j) {
                    opnd.push_back(&_constants[0]);
                }
                addends.push_back(opnd);
                if (i == 0) {
                    rpn = "_";
                } else {
                    rpn.append("_+");
                }

                // break after all partial products are stored
                if (multiplier.size() == 1) {
                    _parser_state = WAIT_OPTR;
                } else if (i == multiplier.size() - 1) {
                    width += i;     // recover width to that of the first partial product
                    _parser_state = MUL_OUT;
                }

                break;
            }

            case MUL_OUT:
                if (optr != '_') {
                    throw TextStreamParserException(lineno, optr_pos, "expected horizontal line");
                }
                _parser_state = ADD_SUB_OUT;
                break;
            default:
                break;
        }
        if (optr != '_') {
            prev_width = width;
        }
    }
    if (_parser_state != WAIT_OPTR || _multiple_constraints.empty()) {
        throw TextStreamParserException(lineno, prev_width, "unexpected EOF");
    }
    if (_explicit_variables.empty()) {
        throw std::runtime_error("The puzzle does not contain any variable!");
    }
}

std::tuple<char, std::vector<PlaceHolder *>, int, int> TextStreamParser::_parse_line(std::string line, int lineno) {
    char optr;
    std::vector<PlaceHolder *> opnd;
    int optr_pos, width;
    _line_parser_state = READ_OPERATOR;
    for (auto i = 0; i < line.size(); ++i) {
        char_type type = check_char_type(line[i]);
        if (type == UNKNOWN) {
            throw TextStreamParserException(lineno, i + 1, "unknown character");
        }
        switch (_line_parser_state) {
            case READ_OPERATOR:
                if (type == OPERATOR) {
                    optr = line[i];
                    optr_pos = i + 1;
                    _line_parser_state = optr == '_' ? HORIZ_LINE : BLANK_BEFORE_OPERAND;
                } else if (type == LETTER || type == DIGIT) {
                    // no operator
                    optr = '\0';
                    optr_pos = (i--) + 1;
                    _line_parser_state = READ_OPERAND;
                }
                break;
            case HORIZ_LINE:
                if (type == BLANK) {
                    width = i;
                    _line_parser_state = BLANK_TRAILING;
                } else if (line[i] != '_') {
                    std::stringstream errmsg;
                    errmsg << "expected '_' or blank character; got '" << line[i] << "' instead";
                    throw TextStreamParserException(lineno, i + 1, errmsg.str());
                }
                break;
            case BLANK_BEFORE_OPERAND:
                if (type == OPERATOR) {
                    throw TextStreamParserException(lineno, i + 1, "duplicate operator");
                } else if (type == LETTER || type == DIGIT) {
                    --i;
                    _line_parser_state = READ_OPERAND;
                }
                break;
            case READ_OPERAND:
                switch (type) {
                    case LETTER: {
                        int letter = toupper(line[i]);
                        if (_lut_letter_to_explicit_variable_no[letter - 'A'] == -1) {
                            // new explicit variable
                            auto no = static_cast<int>(_explicit_variables.size());
                            _explicit_variables.emplace_back(letter, no);
                            _lut_letter_to_explicit_variable_no[letter - 'A'] = no;
                        }
                        opnd.push_back(&_explicit_variables[_lut_letter_to_explicit_variable_no[letter - 'A']]);
                        break;
                    }
                    case DIGIT:
                        opnd.push_back(&_constants[line[i] - '0']);
                        break;
                    case OPERATOR:
                        throw TextStreamParserException(lineno, i + 1, "duplicate operator");
                    case BLANK:
                        width = i;
                        _line_parser_state = BLANK_TRAILING;
                        break;
                    default:
                        break;
                }
                break;
            case BLANK_TRAILING:
                if (type != BLANK) {
                    std::stringstream errmsg;
                    errmsg << "expected blank character; got '" << line[i] << "' instead";
                    throw TextStreamParserException(lineno, i + 1, errmsg.str());
                }
                break;
            default:
                break;
        }
    }
    switch (_line_parser_state) {
        case READ_OPERATOR:
            throw TextStreamParserException(lineno, static_cast<int>(line.size()), "blank line");
        case BLANK_BEFORE_OPERAND:
            throw TextStreamParserException(lineno, static_cast<int>(line.size()), "no operand");
        case HORIZ_LINE:
        case READ_OPERAND:
            width = static_cast<int>(line.size());
            break;
        default:
            break;
    }
    return std::make_tuple(optr, opnd, optr_pos, width);
}

void MultipleConstraintParser::_alloc_implicit_variables() {
    size_t implicit_variable_count = 0;
    for (auto const &multiple_constraint : _multiple_constraints) {
        implicit_variable_count += (multiple_constraint._max_input_length - 1);
    }
    _implicit_variables.reserve(implicit_variable_count);
}

void MultipleConstraintParser::_parse() {
    for (auto const &multiple_constraint : _multiple_constraints) {
        _parse_multiple_constraint(multiple_constraint);
    }
}

void MultipleConstraintParser::_parse_multiple_constraint(MultipleConstraint const &multiple_constraint) {
    std::vector<Constraint> constraints;
    constraints.reserve(multiple_constraint._max_input_length);
    for (int i = 0; i < multiple_constraint._max_input_length; ++i) {
        std::vector<PlaceHolder *> input, output;
        std::string rpn = multiple_constraint._rpn;
        input.reserve(multiple_constraint._input.size());
        output.reserve(static_cast<size_t>(i < multiple_constraint._max_input_length - 1 ? 2 :
                       std::max(1,
                                static_cast<int>(multiple_constraint._output.size())
                                - static_cast<int>(multiple_constraint._max_input_length)
                                + 1)
                       ));
        if (rpn[rpn.size() - 1] == '*') {
            auto const &input_multiplicand = multiple_constraint._input[0];
            auto const &input_multiplier = multiple_constraint._input[1];
            input.push_back(input_multiplicand[input_multiplicand.size() - 1 - i]);
            input.push_back(input_multiplier[0]);
        } else {
            for (auto const &input_item : multiple_constraint._input) {
                if (input_item.size() > i) {
                    input.push_back(input_item[input_item.size() - 1 - i]);
                } else {
                    input.push_back(&_zero);
                }
            }
        }
        if (i > 0) {
            // add carry
            input.push_back(&_implicit_variables[_implicit_variables.size() - 1]);
            rpn.append("_+");
        }
        if (i < multiple_constraint._max_input_length - 1) {
            // create new implicit variable
            _implicit_variables.emplace_back(ImplicitVariable(static_cast<int>(_implicit_variables.size())));
            output.push_back(&_implicit_variables[_implicit_variables.size() - 1]);

            if (multiple_constraint._output.size() > i) {
                output.push_back(multiple_constraint._output[multiple_constraint._output.size() - 1 - i]);
            } else {
                output.push_back(&_zero);
            }
        } else {
            if (multiple_constraint._output.size() >= multiple_constraint._max_input_length) {
                for (size_t j = 0; j <= multiple_constraint._output.size() - multiple_constraint._max_input_length; ++j) {
                    output.push_back(multiple_constraint._output[j]);
                }
            } else {
                output.push_back(&_zero);
            }
        }
        constraints.emplace_back(input, output, rpn);
    }
    _constraint_chains.emplace_back(ConstraintChain(std::move(constraints)));
}