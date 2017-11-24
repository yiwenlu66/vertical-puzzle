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

std::ostream &operator<<(std::ostream &stream, ConstraintChain const &constraint_chain)
{
    for (auto const &constraint : constraint_chain.constraints) {
        stream << constraint;
        if (&constraint != &constraint_chain.constraints.back()) {
            stream << " -> ";
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, Constraint const &constraint)
{
    auto it = constraint._input.cbegin();
    for (auto c : constraint._rpn) {
        if (c == '_') {
            stream << '(' << **(it++) << ')';
        } else {
            stream << c;
        }
    }
    stream << " = ";
    for (auto const &placeholder : constraint._output) {
        stream << *placeholder;
        if (&placeholder != &constraint._output.back()) {
            stream << '_';
        }
    }
    return stream;
}