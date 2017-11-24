#include "constraint.h"

std::ostream &operator<<(std::ostream &stream, MultipleConstraint const &multiple_constraint)
{
    auto it = multiple_constraint._input.cbegin();
    for (auto c : multiple_constraint._rpn) {
        if (c == '_') {
            stream << '(';
            for (auto placeholder : *(it++)) {
                stream << *placeholder;
            }
            stream << ')';
        } else {
            stream << c;
        }
    }
    stream << " = ";
    for (auto placeholder : multiple_constraint._output) {
        stream << *placeholder;
    }
    return stream;
}
