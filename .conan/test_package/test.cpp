//          Copyright Justin Bassett 2020 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <impel/impel.hpp>
#include <iostream>
#include <string>

namespace {
    class Printable {
    public:
        virtual void print() const = 0;
    };
}

template <>
class impel::impl_for<std::string, Printable> : public Printable {
public:
    void print() const final override {
        std::cout << impel::self(*this);
    }
};

void print(Printable const& p) {
    p.print();
}

int main() {
    std::string const message = "Hello, World!";

    ::print(impel::impl_ref<Printable>(message));
    std::cout << '\n';
}
