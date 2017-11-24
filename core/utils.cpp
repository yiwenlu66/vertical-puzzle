#include "utils.h"
#include <cctype>

char_type check_char_type(char c)
{
    if (isblank(c)) {
        return BLANK;
    }
    if (c == '+' || c == '-' || c == '*' || c == '_') {
        return OPERATOR;
    }
    if (('A' <= c && c <= 'J') || ('a' <= c && c <= 'j')) {
        return LETTER;
    }
    if (isdigit(c)) {
        return DIGIT;
    }
    return UNKNOWN;
}
