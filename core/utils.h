#ifndef UTILS_H
#define UTILS_H

typedef enum {
    BLANK,
    OPERATOR,
    LETTER,
    DIGIT,
    UNKNOWN
} char_type;

char_type check_char_type(char c);

#endif
