#include "placeholder.h"
#include <iostream>

std::ostream &operator<<(std::ostream &stream, PlaceHolder const &placeholder) {
    auto p_explicit_variable = dynamic_cast<ExplicitVariable const *>(&placeholder);
    if (p_explicit_variable) {
        stream << p_explicit_variable->letter;
    } else {
        // constant
        stream << placeholder.value;
    }
    return stream;
}
