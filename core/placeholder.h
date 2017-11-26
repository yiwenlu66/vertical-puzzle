#ifndef PLACEHOLDER_H
#define PLACEHOLDER_H

#include <vector>
#include <iostream>
#include <limits>

struct PlaceHolder {
    int value = std::numeric_limits<int>::min();
    int t_bound = std::numeric_limits<int>::max();    // the time when first bound

    bool is_bound(int t) {
        return t >= t_bound;
    }

    friend std::ostream &operator<<(std::ostream &stream, PlaceHolder const &placeholder) {
        return placeholder.print(stream);
    }
    virtual std::ostream &print(std::ostream &stream) const = 0;

protected:
    explicit PlaceHolder() = default;
    explicit PlaceHolder(int t_bound, int value) : t_bound(t_bound), value(value) {}
};


struct Variable : PlaceHolder {
    int no;

    explicit Variable(int no) : PlaceHolder(), no(no) {}
};

struct ExplicitVariable : Variable {
    char letter;

    ExplicitVariable(char letter, int no)
            : Variable(no), letter(letter) {}

    std::ostream &print(std::ostream &stream) const override;
};

struct ImplicitVariable : Variable {
    explicit ImplicitVariable(int no) : Variable(no) {}

    std::ostream &print(std::ostream &stream) const override;
};


struct Constant : PlaceHolder {
    explicit Constant(int value) : PlaceHolder(0, value) {}

    std::ostream &print(std::ostream &stream) const override;
};

#endif