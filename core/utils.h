#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

typedef enum {
    BLANK,
    OPERATOR,
    LETTER,
    DIGIT,
    UNKNOWN
} char_type;

char_type check_char_type(char c);

int evaluate_rpn(std::string const &rpn, std::vector<int> const &inputs);

#endif
