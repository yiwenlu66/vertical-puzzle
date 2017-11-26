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
    int get_n_unbound_input(int t);

    ExplicitVariable *set_binding_time_for_first_unbound_explicit_input(int t);
    std::vector<ExplicitVariable *> set_binding_time_for_output(int t);
    bool is_bound(int t) {
        return t >= _t_bound;
    }

    bool bind_output(int t, std::vector<bool> &digit_used);             // return true for success, false for failure

    friend std::ostream &operator<<(std::ostream &stream, Constraint const &constraint);
    friend class ConstraintChain;

private:
    int _t_bound = std::numeric_limits<int>::max();
    const std::vector<PlaceHolder *> _input, _output;
    const std::string _rpn;
};

class ConstraintChain {
public:
    explicit ConstraintChain(std::vector<Constraint> constraints)
            : _constraints(std::move(constraints)) {}

    Constraint &get_frontier(int t);
    bool is_bound(int t) {
        return _constraints.back().is_bound(t);
    }

    friend std::ostream &operator<<(std::ostream &stream, ConstraintChain const &constraint_chain);

private:
    std::vector<Constraint> _constraints;
};

#endif
