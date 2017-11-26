#include "utils.h"
#include <cctype>
#include <stack>

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

int evaluate_rpn(std::string const &rpn, std::vector<int> const &inputs)
{
    std::stack<int> opnd;
    auto it = inputs.cbegin();
    for (auto c : rpn) {
        switch (c) {
            case '_':
                opnd.push(*(it++));
                break;
            case '+':
            case '-':
            case '*': {
                int n1 = opnd.top();
                opnd.pop();
                int n2 = opnd.top();
                opnd.pop();
                switch (c) {
                    case '+':
                        opnd.push(n2 + n1);
                        break;
                    case '-':
                        opnd.push(n2 - n1);
                        break;
                    case '*':
                        opnd.push(n2 * n1);
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
    return opnd.top();
}
