#include <vector>
#include <iostream>

struct PlaceHolder {
    int value;
    virtual ~PlaceHolder() = default;

    friend std::ostream &operator<<(std::ostream &stream, PlaceHolder const &placeholder);

protected:
    explicit PlaceHolder() {}
    explicit PlaceHolder(int value) : value(value) {}
};

struct ExplicitVariable : PlaceHolder {
    std::vector<int> t_bound;
    char letter;
    int no;
    ExplicitVariable(char letter, int no)
            : PlaceHolder(-1), letter(letter), no(no) {}
};

struct ImplicitVariable : PlaceHolder {
    std::vector<int> t_bound;
    int no;
};

struct Constant : PlaceHolder {
    explicit Constant(int value) : PlaceHolder(value) {}
};