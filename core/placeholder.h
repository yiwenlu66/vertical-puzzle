#include <vector>
#include <iostream>

struct PlaceHolder {
    int value;
    friend std::ostream &operator<<(std::ostream &stream, PlaceHolder const &placeholder) {
        return placeholder.print(stream);
    }
    virtual std::ostream &print(std::ostream &stream) const = 0;

protected:
    explicit PlaceHolder() = default;
    explicit PlaceHolder(int value) : value(value) {}
};

struct ExplicitVariable : PlaceHolder {
    std::vector<int> t_bound;
    char letter;
    int no;
    ExplicitVariable(char letter, int no)
            : PlaceHolder(-1), letter(letter), no(no) {}
    bool is_bound() { return !t_bound.empty(); }
    std::ostream &print(std::ostream &stream) const override;
};

struct ImplicitVariable : PlaceHolder {
    std::vector<int> t_bound;
    int no;
    bool is_bound() { return !t_bound.empty(); }
    explicit ImplicitVariable(int no) : PlaceHolder(-1), no(no) {}
    std::ostream &print(std::ostream &stream) const override;
};

struct Constant : PlaceHolder {
    explicit Constant(int value) : PlaceHolder(value) {}
    std::ostream &print(std::ostream &stream) const override;
};