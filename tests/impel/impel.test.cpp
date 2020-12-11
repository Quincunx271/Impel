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
            Balloon& self = impel::self<Balloon, Inflatable>(*this);
            self.weight += weight;
            self.volume += volume;
        }
    };

    class DirectBalloon : public Inflatable {
    public:
        int weight = 0;
        int volume = 0;

        DirectBalloon() = default;

        explicit DirectBalloon(int weight, int volume)
            : weight(weight)
            , volume(volume) {
        }

    public:
        void inflate(int dweight, int dvolume) & final override {
            weight += dweight;
            volume += dvolume;
        }
    };
}

namespace {
#ifdef _MSC_VER // MSVC doesn't support shorthand syntax yet
    template <impel::impls<Inflatable> T>
    void inflate(T& it, int weight, int volume) {
#else
    void inflate(impel::impls<Inflatable> auto& it, int weight, int volume) {
#endif
        auto impl = impel::impl_ref<Inflatable>(it);
        auto& infl = impl.get();
        infl.inflate(weight, volume);
    }

    void inflate_dyn(Inflatable& it, int weight, int volume) {
        it.inflate(weight, volume);
    }
}

TEST_CASE("Explicit Specialization") {
    ::Balloon b {10, 20};
    ::inflate(b, 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);

    auto x = impel::impl<Balloon&, Inflatable>(b);
    CHECK(&x.value() == &b);
}

TEST_CASE("ADL Specialization") {
    ::ns::Balloon b {10, 20};
    ::inflate(b, 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);

    auto x = impel::impl_ref<Inflatable>(b);
    CHECK(&x.value() == &b);
}

TEST_CASE("Regular inheritance") {
    ::ns::DirectBalloon b(10, 20);
    ::inflate(b, 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);

    auto x = impel::impl_ref<Inflatable>(b);
    CHECK(&x.value() == &b);
}

TEST_CASE("Dynamic polymorphism") {
    ::Balloon b {10, 20};
    ::inflate_dyn(impel::impl_ref<Inflatable>(b), 10, 20);
    CHECK(b.weight == 10 + 10);
    CHECK(b.volume == 20 + 20);
}

TEST_CASE("impl<T, ...> for non-reference T") {
    using impl_t = impel::impl<Balloon, Inflatable>;
    // Why does this fail?
    STATIC_REQUIRE(std::is_default_constructible_v<impl_t>);
    STATIC_REQUIRE(std::is_constructible_v<impl_t, int&&, int&&>);
    STATIC_REQUIRE(std::is_constructible_v<impl_t, int&&>);
    STATIC_REQUIRE(std::is_constructible_v<impl_t, Balloon&&>);

    impl_t impl(10, 20);
    ::inflate_dyn(impl, 10, 20);
    CHECK(impl.value().weight == 10 + 10);
    CHECK(impl.value().volume == 20 + 20);
}

TEST_CASE("impl<T, ...> for non-reference T and regular inheritance") {
    using impl_t = impel::impl<ns::DirectBalloon, Inflatable>;
    STATIC_REQUIRE(std::is_default_constructible_v<impl_t>);
    STATIC_REQUIRE(std::is_constructible_v<impl_t, int&&, int&&>);
    STATIC_REQUIRE(std::is_constructible_v<impl_t, ns::DirectBalloon&&>);

    impl_t impl(10, 20);
    ::inflate_dyn(impl, 10, 20);
    CHECK(impl.value().weight == 10 + 10);
    CHECK(impl.value().volume == 20 + 20);
}
