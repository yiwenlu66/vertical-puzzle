#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <string>
#include <iostream>
#include "placeholder.h"

class MultipleConstraint {
public:
    MultipleConstraint(std::vector<std::vector<PlaceHolder *> > input, std::vector<PlaceHolder *> output,
                       std::string rpn)
            : _input(std::move(input)), _output(std::move(output)), _rpn(std::move(rpn)) {
        for (auto const &item : _input) {
            _max_input_length = std::max(_max_input_length, item.size());
        }
    }
    friend std::ostream &operator<<(std::ostream &stream, MultipleConstraint const &multiple_constraint);
    friend class MultipleConstraintParser;

private:
    size_t _max_input_length = 0;
    const std::vector<std::vector<PlaceHolder *> > _input;  // each element is an operand with one or more digits
    const std::vector<PlaceHolder *> _output;               // a number with one or more digits
    const std::string _rpn;                                 // supported operators: '+', '-', '*'; use '_' for operand
};

class Constraint {
public:
    Constraint(std::vector<PlaceHolder *> input, std::vector<PlaceHolder *> output, std::string rpn)
            : _input(std::move(input)), _output(std::move(output)), _rpn(std::move(rpn)) {}
    int get_n_unbound_input();
    void bind_output();
    friend std::ostream &operator<<(std::ostream &stream, Constraint const &constraint);

private:
    const std::vector<PlaceHolder *> _input, _output;
    const std::string _rpn;
};

struct ConstraintChain {
    std::vector<Constraint> constraints;
    int frontier = 0;
    ConstraintChain(std::vector<Constraint> constraints)
            : constraints(std::move(constraints)) {}
    friend std::ostream &operator<<(std::ostream &stream, ConstraintChain const &constraint_chain);
};

#endif
