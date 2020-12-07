//          Copyright Justin Bassett 2020 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef IMPEL_H_642D468BB237
#define IMPEL_H_642D468BB237

#include <concepts>
#include <type_traits>
#include <utility>

namespace impel {
    template <typename T, typename Trait>
    class impl_for;

    namespace detail {
        struct priv_tag { };

        struct impl_self;

        template <typename T, typename Trait>
        concept impls_via_impl_for = sizeof(impl_for<T, Trait>) > 0;

        template <typename T, typename Trait>
        concept impls_via_adl = requires(T x, Trait const& trait) {
            impel_impl_for(x, trait);
        };

        template <bool ViaImplFor>
        struct find_impl;

        template <>
        struct find_impl<true> {
            template <typename T, typename Trait>
            using type = impl_for<T, Trait>;
        };

        template <>
        struct find_impl<false> {
            template <typename T, typename Trait>
            using type = decltype(impel_impl_for(std::declval<T>(), std::declval<Trait>()));
        };

        template <typename T, typename Trait>
        using find_impl_t =
            typename find_impl<impls_via_impl_for<T, Trait>>::template type<T, Trait>;
    }

    template <typename T, typename Trait>
    concept impls = detail::impls_via_impl_for<T, Trait> || detail::impls_via_adl<T, Trait>;

    template <typename T, typename Trait, typename Impl>
    constexpr auto self(Impl&& it) -> T;

    template <typename T, typename Trait>
    class impl {
        static_assert(std::is_reference_v<T>);

    private:
        using T_ = std::remove_cvref_t<T>;

        friend detail::impl_self;

        struct impl_t : public detail::find_impl_t<std::remove_cvref_t<T>, Trait> {
        private:
            T it;

            friend detail::impl_self;

            template <typename U>
            constexpr auto get_self(detail::priv_tag) const -> U {
                return static_cast<U>(const_cast<U>(it));
            }

        public:
            constexpr impl_t(T val)
                : it(std::forward<T>(val)) {
            }
        };

        impl_t it;

    public:
        constexpr impl(T val)
            : it(std::forward<T>(val)) {
        }

        constexpr Trait& get() requires std::same_as<T, T_&> {
            return static_cast<Trait&>(it);
        }

        constexpr Trait const& get() const requires std::same_as<T, T_ const&> {
            return static_cast<Trait const&>(it);
        }

        constexpr Trait&& get() requires std::same_as<T, T_&&> {
            return static_cast<Trait const&>(it);
        }

        constexpr Trait const&& get() const requires std::same_as<T, T_ const&&> {
            return static_cast<Trait const&>(it);
        }

        constexpr operator Trait&() requires std::same_as<T, T_&> {
            return static_cast<Trait&>(it);
        }

        constexpr operator Trait const &() const requires std::same_as<T, T_ const&> {
            return static_cast<Trait const&>(it);
        }

        constexpr operator Trait&&() requires std::same_as<T, T_&&> {
            return static_cast<Trait const&>(it);
        }

        constexpr operator Trait const &&() const requires std::same_as<T, T_ const&&> {
            return static_cast<Trait const&>(it);
        }
    };

    template <typename Trait, typename T>
    constexpr auto impl_of(T&& it) {
        return impl<T&&, Trait>(std::forward<T>(it));
    }

    namespace detail {
        struct impl_self {
            template <typename T, typename Trait, typename Impl>
            static constexpr auto self(Impl&& it) -> T {
                return static_cast<typename impl<T, Trait>::impl_t const&>(it).template get_self<T>(
                    detail::priv_tag {});
            }
        };
    }

    template <typename T, typename Trait, typename Impl>
    constexpr auto self(Impl&& it) -> T {
        return detail::impl_self::self<T, Trait>(std::forward<Impl>(it));
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait>& it) -> T& {
        return impel::self<T&, Trait>(it);
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait> const& it) -> T const& {
        return impel::self<T const&, Trait>(it);
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait>&& it) -> T&& {
        return impel::self<T&&, Trait>(it);
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait> const&& it) -> T const&& {
        return impel::self<T const&&, Trait>(it);
    }
}

#endif
