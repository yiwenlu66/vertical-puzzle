#include "constraint.h"
#include <deque>
#include "utils.h"

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
    for (auto const &constraint : constraint_chain._constraints) {
        stream << constraint;
        if (&constraint != &constraint_chain._constraints.back()) {
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

int Constraint::get_n_unbound_input(int t) {
    int count = 0;
    for (auto const &item : _input) {
        if (!item->is_bound(t)) {
            ++count;
        }
    }
    return count;
}

ExplicitVariable* Constraint::set_binding_time_for_first_unbound_explicit_input(int t) {
    for (auto &item : _input) {
        auto p_explicit = dynamic_cast<ExplicitVariable *>(item);
        if (p_explicit && !p_explicit->is_bound(t)) {
            p_explicit->t_bound = t;
            return p_explicit;
        }
    }
    return nullptr;
}

std::vector<ExplicitVariable *> Constraint::set_binding_time_for_output(int t) {
    std::vector<ExplicitVariable *> explicit_var_set;
    _t_bound = t;
    for (auto &item : _output) {
        auto p_var = dynamic_cast<Variable *>(item);
        if (p_var && !p_var->is_bound(t)) {
            p_var->t_bound = t;
            auto p_explicit_var = dynamic_cast<ExplicitVariable *>(p_var);
            if (p_explicit_var) {
                explicit_var_set.push_back(p_explicit_var);
            }
        }
    }
    return explicit_var_set;
}

Constraint& ConstraintChain::get_frontier(int t) {
    for (auto &constraint : _constraints) {
        if (!constraint.is_bound(t)) {
            return constraint;
        }
    }
    return _constraints[_constraints.size() - 1];
}

bool Constraint::bind_output(int t, std::vector<bool> &digit_used) {
    std::vector<int> input_digits(_input.size());
    for (size_t i = 0; i < _input.size(); ++i) {
        input_digits[i] = _input[i]->value;
    }
    int output_value = evaluate_rpn(_rpn, input_digits);
    int output_unit = output_value % 10, output_carry = output_value / 10;
    if (output_unit < 0) {
        output_unit += 10;
        --output_carry;
    }

    // put unit to the back
    auto &back = _output.back();
    if (back->value != output_unit) {
        // check for conflict with existing value
        if (back->is_bound(t) && back->value != std::numeric_limits<int>::min()) {
            return false;
        }
        if (dynamic_cast<ExplicitVariable *>(back)) {
            // check for duplicate values
            if (digit_used[output_unit]) {
                return false;
            }
            digit_used[output_unit] = true;
        }
        back->value = output_unit;
    }

    // put carry to the front
    if (_output.size() < 2) {
        return output_carry == 0;
    }
    auto &front = _output.front();
    if (dynamic_cast<ImplicitVariable *>(front)) {
        // no need to check for conflict for implicit variables
        front->value = output_carry;
    } else if (front->value != output_carry) {
        // check for conflict with existing value
        if (front->is_bound(t) && front->value != std::numeric_limits<int>::min()) {
            return false;
        }
        if (dynamic_cast<ExplicitVariable *>(front)) {
            // check for range and for duplicate values
            if (output_carry < 0 || output_carry > 9 || digit_used[output_carry]) {
                return false;
            }
            digit_used[output_carry] = true;
        }
        front->value = output_carry;
    }

    return true;
}