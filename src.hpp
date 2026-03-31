// Implementation of a Python-like list (int or nested list) with reference semantics.
#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>

class pylist {
    struct Node {
        bool is_int;
        int ivalue;
        std::shared_ptr<std::vector<pylist>> lvalue;
        Node() : is_int(false), ivalue(0), lvalue(std::make_shared<std::vector<pylist>>()) {}
        explicit Node(int v) : is_int(true), ivalue(v), lvalue(nullptr) {}
    };

    std::shared_ptr<Node> p;

    static void print_impl(std::ostream &os, const pylist &ls, std::vector<const Node*> &stack) {
        if (!ls.p) return; // should not happen
        if (ls.p->is_int) {
            os << ls.p->ivalue;
            return;
        }
        const Node* addr = ls.p.get();
        for (const Node* anc : stack) {
            if (anc == addr) {
                os << "[...]";
                return;
            }
        }
        stack.push_back(addr);
        os << '[';
        const auto &vec = *ls.p->lvalue;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, vec[i], stack);
        }
        os << ']';
        stack.pop_back();
    }

public:
    // Default: an empty list
    pylist() : p(std::make_shared<Node>()) {}

    // Construct from int
    pylist(int v) : p(std::make_shared<Node>(v)) {}

    // Append an int
    void append(int x) {
        if (!p->is_int) {
            p->lvalue->push_back(pylist(x));
        }
    }

    // Append a pylist
    void append(const pylist &x) {
        if (!p->is_int) {
            p->lvalue->push_back(x);
        }
    }

    // Pop from the end, return popped element
    pylist pop() {
        if (p->is_int || p->lvalue->empty()) {
            return pylist();
        }
        pylist ret = p->lvalue->back();
        p->lvalue->pop_back();
        return ret;
    }

    // Indexing
    pylist &operator[](size_t i) {
        return (*p->lvalue)[i];
    }

    const pylist &operator[](size_t i) const {
        return (*p->lvalue)[i];
    }

    // Assignment from int to replace current content
    pylist &operator=(int v) {
        p = std::make_shared<Node>(v);
        return *this;
    }

    // Implicit conversion to int when holding an int
    operator int() const {
        return p->is_int ? p->ivalue : 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        if (!ls.p) return os;
        if (ls.p->is_int) {
            os << ls.p->ivalue;
            return os;
        }
        std::vector<const Node*> stack;
        print_impl(os, ls, stack);
        return os;
    }
};

#endif // SRC_HPP

