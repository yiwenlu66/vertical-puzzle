#ifndef SOLVER_H
#define SOLVER_H

#include "placeholder.h"
#include "constraint.h"

class Solver {
public:
    Solver(std::vector<ConstraintChain> &constraint_chains, std::vector<ExplicitVariable> &explicit_variables,
           int max_solutions)
            : _constraint_chains(constraint_chains),
              _explicit_variables(explicit_variables),
              _max_solutions(max_solutions),
              _digit_used(std::vector<bool>(10, false))
    {
        _active_explicit_variable_bound_at_t.reserve(_explicit_variables.size() + 1);
        _active_explicit_variable_bound_at_t.push_back(nullptr);   // no explicit variable bound at t=0
        _constraints_bound_at_t.reserve(_explicit_variables.size() + 1);
        _passive_explicit_variables_bound_at_t.reserve(_explicit_variables.size() + 1);
        _solve();
    }
    std::vector<std::vector<int> > const &get_solutions() {
        return _solutions;
    }

private:
    std::vector<ConstraintChain> &_constraint_chains;
    std::vector<ExplicitVariable> &_explicit_variables;
    int _max_solutions;
    std::vector<bool> _digit_used;
    std::vector<std::vector<int> > _solutions;

    std::vector<std::vector<Constraint *> > _constraints_bound_at_t;
    std::vector<ExplicitVariable *> _active_explicit_variable_bound_at_t;
    std::vector<std::vector<ExplicitVariable *> > _passive_explicit_variables_bound_at_t;

    int _t = 0;
    std::vector<int> _partial_solution;             // the first element is not used

    void _solve();
    int _calculate_binding_time();                  // return largest binding time
    int _get_first_unused_digit(int lower_bound);   // return the smallest number no less than lower_bound, -1 on overflow
    bool _backtrace();                              // return true on success, false on failure
    bool _move_forward();                           // return true on success, false on failure
    void _commit_solution();
};


#endif
