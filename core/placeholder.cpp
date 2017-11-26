#include "placeholder.h"
#include <iostream>

std::ostream& ExplicitVariable::print(std::ostream &stream) const {
    stream << letter;
    return stream;
}

std::ostream& ImplicitVariable::print(std::ostream &stream) const {
    stream << 'i' << no;
    return stream;
}

std::ostream& Constant::print(std::ostream &stream) const {
    stream << value;
    return stream;
}
