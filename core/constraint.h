#include <string>
#include <iostream>
#include "placeholder.h"

class MultipleConstraint {
public:
    MultipleConstraint(std::vector<std::vector<PlaceHolder *> > input, std::vector<PlaceHolder *> output,
                       std::string rpn)
            : _input(std::move(input)), _output(std::move(output)), _rpn(std::move(rpn)) {}
    friend std::ostream &operator<<(std::ostream &stream, MultipleConstraint const &multiple_constraint);
private:
    const std::vector<std::vector<PlaceHolder *> > _input;  // each element is an operand with one or more digits
    const std::vector<PlaceHolder *> _output;               // a number with one or more digits
    const std::string _rpn;                                 // supported operators: '+', '-', '*'; use '_' for operand
};