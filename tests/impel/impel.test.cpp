//          Copyright Justin Bassett 2020 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <impel/impel.hpp>

#include <catch2/catch.hpp>

namespace {
    class Inflatable {
    public:
        constexpr virtual void inflate(int weight, int volume) & = 0;
    };

    // Custom type
    struct Balloon {
        int weight;
        int volume;
    };
}

template <>
class impel::impl_for<Balloon, Inflatable> : public Inflatable {
public:
    void inflate(int weight, int volume) & final override {
        Balloon& self = impel::self(*this);
        self.weight += weight;
        self.volume += volume;
    }
};

namespace ns {
    struct Balloon {
        int weight;
        int volume;
    };

    auto impel_impl_for(Balloon, Inflatable const&) -> struct BalloonImpl;

    struct BalloonImpl : public Inflatable {
        void inflate(int weight, int volume) & final override {
            Balloon& self = impel::self<Balloon&, Inflatable>(*this);
            self.weight += weight;
            self.volume += volume;
        }
    };
}

namespace {
    template <typename T>
    // clang-format off
        requires impel::impls<T, Inflatable>
    void inflate(T& it, int weight, int volume) {
        // clang-format on
        auto infl = impel::impl<T&, Inflatable>(it);
        infl.inflate(weight, volume);
    }
}

TEST_CASE("Explicit Specialization") {
    ::Balloon b {10, 20};
    ::inflate(b, 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);
}


TEST_CASE("ADL Specialization") {
    ::ns::Balloon b {10, 20};
    ::inflate(b, 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);
}
