#include "placeholder.h"
#include <iostream>

std::ostream& ExplicitVariable::print(std::ostream &stream) const {
    stream << letter;
}

std::ostream& ImplicitVariable::print(std::ostream &stream) const {
    stream << 'i' << no;
}

std::ostream& Constant::print(std::ostream &stream) const {
    stream << value;
}
