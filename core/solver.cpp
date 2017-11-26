#include "solver.h"
#include <deque>

int Solver::_calculate_binding_time() {
    int t = 0;

    std::deque<ConstraintChain *> dq;
    for (auto &constraint_chain : _constraint_chains) {
        dq.push_back(&constraint_chain);
    }

    while (!dq.empty()) {
        auto cmp = [=](ConstraintChain *c1, ConstraintChain *c2) {
            return c1->get_frontier(t).get_n_unbound_input(t) < c2->get_frontier(t).get_n_unbound_input(t);
        };
        std::sort(dq.begin(), dq.end(), cmp);
        auto p_chain = dq.front();
        dq.pop_front();
        auto &frontier = p_chain->get_frontier(t);
        while (frontier.get_n_unbound_input(t)) {
            // for the chain with least unbound input variables at frontier, all unbound inputs must be explicit
            _active_explicit_variable_bound_at_t.push_back(frontier.set_binding_time_for_first_unbound_explicit_input(++t));
        }

        while (_constraints_bound_at_t.size() <= t) {
            _constraints_bound_at_t.emplace_back(std::vector<Constraint *>());
            _passive_explicit_variables_bound_at_t.emplace_back(std::vector<ExplicitVariable *>());
        }
        _constraints_bound_at_t[t].push_back(&frontier);
        auto passive = frontier.set_binding_time_for_output(t);
        _passive_explicit_variables_bound_at_t[t].insert(_passive_explicit_variables_bound_at_t[t].end(),
                                                         passive.begin(), passive.end());

        if (!p_chain->is_bound(t)) {
            dq.push_back(p_chain);
        }
    }

    return t;
}

void Solver::_solve() {
    int max_t = _calculate_binding_time();
    _partial_solution = std::vector<int>(static_cast<size_t>(max_t) + 1, std::numeric_limits<int>::min());

    while (_solutions.size() < _max_solutions) {
        for (auto &constraint : _constraints_bound_at_t[_t]) {
            if (!constraint->bind_output(_t, _digit_used)) {
                if (!_backtrace()) {
                    return;
                } else {
                    break;
                }
            }
        }
        for (auto &p_explicit_var : _passive_explicit_variables_bound_at_t[_t]) {
            _digit_used[p_explicit_var->value] = true;
        }
        if (_t == max_t) {
            _commit_solution();
        }
        while (!_move_forward()) {
            if (!_backtrace()) {
                return;
            }
        }
    }
}


int Solver::_get_first_unused_digit(int lower_bound) {
    lower_bound = std::max(lower_bound, 0);
    for (int i = lower_bound; i <= 9; ++i) {
        if (!_digit_used[i]) {
            return i;
        }
    }
    return -1;
}


bool Solver::_backtrace() {
    if (!_t) {
        return false;
    }
    _digit_used[_partial_solution[_t]] = false;
    _active_explicit_variable_bound_at_t[_t]->value = std::numeric_limits<int>::min();
    for (auto p_explicit_var : _passive_explicit_variables_bound_at_t[_t]) {
        if (p_explicit_var->value >= 0) {
            _digit_used[p_explicit_var->value] = false;
            p_explicit_var->value = std::numeric_limits<int>::min();
        }
    }
    if (_t + 1 < _partial_solution.size()) {
        _partial_solution[_t + 1] = std::numeric_limits<int>::min();
    }
    --_t;
    return true;
}


bool Solver::_move_forward() {
    if (_t + 1 >= _partial_solution.size()) {
        return false;
    }
    int new_value = _get_first_unused_digit(_partial_solution[_t + 1] + 1);
    if (new_value >= 0) {
        _partial_solution[++_t] = new_value;
        _digit_used[new_value] = true;
        _active_explicit_variable_bound_at_t[_t]->value = new_value;
        return true;
    }
    return false;
}


void Solver::_commit_solution() {
    std::vector<int> solution(_explicit_variables.size());
    for (size_t i = 0; i < _explicit_variables.size(); ++i) {
        solution[i] = _explicit_variables[i].value;
    }
    _solutions.push_back(solution);
}