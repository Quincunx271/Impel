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

    template <typename T, typename Trait>
    class impl;

    namespace detail {
        template <bool Cond>
        struct conditional {
            template <typename A, template <typename...> class B, typename... Ts>
            using semi_eval = A;
        };

        template <>
        struct conditional<false> {
            template <typename A, template <typename...> class B, typename... Ts>
            using semi_eval = B<Ts...>;
        };

        template <bool Cond, typename A, template <typename...> class B, typename... Ts>
        using conditional_semi_eval_t = typename conditional<Cond>::template semi_eval<A, B, Ts...>;

        struct priv_tag { };

        struct impl_self;

        template <typename T, typename Trait>
        concept impls_via_impl_for = sizeof(impl_for<T, Trait>) > 0;

        template <typename T, typename Trait>
        concept impls_via_adl = requires(T x, Trait const& trait) {
            impel_impl_for(x, trait);
        };

        template <typename T, typename Trait>
        concept impls_via_inheritance = std::convertible_to<T*, Trait*>;

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

        template <typename T, typename Trait>
        struct impl_t final : public detail::find_impl_t<T, Trait> {
            static_assert(!std::is_reference_v<T>);

        private:
            T const* it; // Possibly &, const&, &&, const&&

            friend detail::impl_self;
            friend impel::impl<T, Trait>;
            friend impel::impl<T&, Trait>;
            friend impel::impl<T const&, Trait>;
            friend impel::impl<T&&, Trait>;
            friend impel::impl<T const&&, Trait>;

        private:
            constexpr impl_t(T const& ref)
                : it(&ref) {
            }
        };
    }

    template <typename T, typename Trait>
    concept impls = detail::impls_via_impl_for<std::remove_cvref_t<T>, Trait> //
        || detail::impls_via_adl<std::remove_cvref_t<T>, Trait> //
        || detail::impls_via_inheritance<std::remove_cvref_t<T>, Trait>;

    template <typename T, typename Trait>
    class impl {
        // Non-references handled via partial specialization.
        static_assert(std::is_reference_v<T>);

    private:
        using T_ = std::remove_cvref_t<T>;

        template <typename Rhs>
        static constexpr bool compatible_with = std::same_as<T, Rhs>;

        // For non-intrusive inheritance, we need a wrapper to get const correctness right.
        using member_t = detail::conditional_semi_eval_t< //
            detail::impls_via_inheritance<T_, Trait>, // If we inherit
            T, // Use the type itself
            detail::impl_t, // Else use the impl wrapper type
            T_, Trait>;

        member_t it;

    public:
        constexpr T& value()
            & requires compatible_with<T_&>&& detail::impls_via_inheritance<T_, Trait> {
            return it;
        }

        constexpr T const& value()
            const& requires compatible_with<T_ const&>&& detail::impls_via_inheritance<T_, Trait> {
            return it;
        }

        constexpr T&& value()
            && requires compatible_with<T_&&>&& detail::impls_via_inheritance<T_, Trait> {
            return std::move(it);
        }

        constexpr T const&&
        value() const&& requires compatible_with<T_ const&&>&& detail::impls_via_inheritance<T_,
            Trait> {
            return std::move(it);
        }

        constexpr T& value()
                & requires compatible_with<T_&> && (!detail::impls_via_inheritance<T_, Trait>) {
            return const_cast<T&>(*it.it);
        }

        constexpr T const& value() const& requires compatible_with<
            T_ const&> && (!detail::impls_via_inheritance<T_, Trait>) {
            return *it.it;
        }

        constexpr T&& value()
            && requires compatible_with<T_&&> && (!detail::impls_via_inheritance<T_, Trait>) {
            return *std::move(it.it);
        }

        constexpr T const&& value() const&& requires compatible_with<
            T_ const&&> && (!detail::impls_via_inheritance<T_, Trait>) {
            return *std::move(it.it);
        }

    public:
        constexpr member_t& get() requires compatible_with<T_&> {
            return it;
        }

        constexpr member_t const& get() const requires compatible_with<T_ const&> {
            return it;
        }

        constexpr member_t&& get() && requires compatible_with<T_&&> {
            return std::move(it);
        }

        constexpr member_t const&& get() const&& requires compatible_with<T_ const&&> {
            return std::move(it);
        }

        constexpr operator Trait&() requires compatible_with<T_&> {
            return it;
        }

        constexpr operator Trait const &() const requires compatible_with<T_ const&> {
            return it;
        }

        constexpr operator Trait&&() && requires compatible_with<T_&&> {
            return std::move(it);
        }

        constexpr operator Trait const &&() const&& requires compatible_with<T_ const&&> {
            return std::move(it);
        }

    public:
        constexpr impl(T val)
            : it(std::forward<T>(val)) { // Maintain value category of T reference
        }
    };

    template <typename T, typename Trait>
        // clang-format off
        requires (!std::is_reference_v<T>) && (!detail::impls_via_inheritance<T, Trait>)
    class impl<T, Trait> {
        // clang-format on
    private:
        T it;
        detail::impl_t<T, Trait> impl_;

    public:
        // Keep default-constructible if T is.
        constexpr impl()
            : it()
            , impl_(it) {
        }

        constexpr impl(impl const& rhs) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : it(rhs.it)
            , impl_(it) {
        }

        constexpr impl(impl&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
            : it(std::move(rhs.it))
            , impl_(it) {
        }

        constexpr impl& operator=(impl const& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>) {
            it = rhs.it;
            impl_.it = &it;
            return *this;
        }

        constexpr impl& operator=(impl&& rhs) noexcept(std::is_nothrow_move_assignable_v<T>) {
            it = std::move(rhs.it);
            impl_.it = &it;
            return *this;
        }

        friend void swap(impl& lhs, impl& rhs) {
            using std::swap;
            swap(lhs.it, rhs.it);
        }

    public:
        constexpr T& value() & {
            return it;
        }

        constexpr T const& value() const& {
            return it;
        }

        constexpr T&& value() && {
            return std::move(it);
        }

        constexpr T const&& value() const&& {
            return std::move(it);
        }

    public:
        constexpr auto get() & -> detail::impl_t<T, Trait>& {
            return impl_;
        }

        constexpr auto get() const& -> detail::impl_t<T, Trait> const& {
            return impl_;
        }

        constexpr auto get() && -> detail::impl_t<T, Trait>&& {
            return std::move(impl_);
        }

        constexpr auto get() const&& -> detail::impl_t<T, Trait> const&& {
            return std::move(impl_);
        }

        constexpr operator Trait&() & {
            return get();
        }

        constexpr operator Trait const &() const& {
            return get();
        }

        constexpr operator Trait&&() && {
            return std::move(*this).get();
        }

        constexpr operator Trait const &&() const&& {
            return std::move(*this).get();
        }

    public:
        template <typename U>
            // clang-format off
            requires (!std::same_as<std::remove_cvref_t<U>, impl>) && std::constructible_from<T, U&&>
        explicit(std::convertible_to<U&&, T>)
        constexpr impl(U&& val)
            : it(std::forward<U>(val))
            , impl_(it) {
        }
        // clang-format on

        template <typename T1, typename T2, typename... Args>
        // clang-format off
            requires std::constructible_from<T, T1&&, T2&&, Args&&...>
        explicit constexpr impl(T1&& t1, T2&& t2, Args&&... args)
            // clang-format on
            : it(T(std::forward<T1>(t1), std::forward<T2>(t2), std::forward<Args>(args)...))
            , impl_(it) {
        }
    };

    template <typename T, typename Trait>
        // clang-format off
        requires (!std::is_reference_v<T>) && detail::impls_via_inheritance<T, Trait>
    class impl<T, Trait> {
        // clang-format on
    private:
        T it;

    public:
        constexpr T& value() & {
            return it;
        }

        constexpr T const& value() const& {
            return it;
        }

        constexpr T&& value() && {
            return std::move(it);
        }

        constexpr T const&& value() const&& {
            return std::move(it);
        }

    public:
        constexpr auto get() & -> T& {
            return it;
        }

        constexpr auto get() const& -> T const& {
            return it;
        }

        constexpr auto get() && -> T&& {
            return std::move(it);
        }

        constexpr auto get() const&& -> T const&& {
            return std::move(it);
        }

        constexpr operator Trait&() & {
            return get();
        }

        constexpr operator Trait const &() const& {
            return get();
        }

        constexpr operator Trait&&() && {
            return std::move(*this).get();
        }

        constexpr operator Trait const &&() const&& {
            return std::move(*this).get();
        }

    public:
        // Keep default-constructible if T is.
        constexpr impl() = default;

        template <typename U>
            // clang-format off
            requires (!std::same_as<std::remove_cvref_t<U>, impl>) && std::constructible_from<T, U&&>
        explicit(std::convertible_to<U&&, T>)
        constexpr impl(U&& val)
            : it(std::forward<U>(val)) {
        }
        // clang-format on

        template <typename T1, typename T2, typename... Args>
        // clang-format off
            requires std::constructible_from<T, T1&&, T2&&, Args&&...>
        explicit constexpr impl(T1&& t1, T2&& t2, Args&&... args)
            // clang-format on
            : it(T(std::forward<T1>(t1), std::forward<T2>(t2), std::forward<Args>(args)...)) {
        }
    };

    template <typename Trait, typename T>
    constexpr auto impl_ref(T&& it) {
        return impl<T&&, Trait>(std::forward<T>(it));
    }

    template <typename Trait, typename T>
    constexpr auto make_impl(T&& it) {
        return impl<std::remove_cvref_t<T>, Trait>(std::forward<T>(it));
    }

    namespace detail {
        struct impl_self {
            template <typename T, typename Trait, typename Impl>
            static constexpr auto self(Impl& it) -> T& {
                return const_cast<T&>(*static_cast<detail::impl_t<T, Trait>&>(it).it);
            }

            template <typename T, typename Trait, typename Impl>
            static constexpr auto self(Impl const& it) -> T const& {
                return *static_cast<detail::impl_t<T, Trait> const&>(it).it;
            }
            template <typename T, typename Trait, typename Impl>
            static constexpr auto self(Impl&& it) -> T&& {
                return std::move(impl_self::self<T, Trait>(it));
            }

            template <typename T, typename Trait, typename Impl>
            static constexpr auto self(Impl const&& it) -> T const&& {
                return std::move(impl_self::self<T, Trait>(it));
            }
        };
    }

    template <typename T, typename Trait, typename Impl>
    constexpr auto self(Impl&& it) -> decltype(auto) {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Trait>);

        return detail::impl_self::self<T, Trait>(std::forward<Impl>(it));
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait>& it) -> T& {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Trait>);

        return detail::impl_self::self<T, Trait>(it);
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait> const& it) -> T const& {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Trait>);

        return detail::impl_self::self<T, Trait>(it);
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait>&& it) -> T&& {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Trait>);

        return detail::impl_self::self<T, Trait>(std::move(it));
    }

    template <typename T, typename Trait>
    constexpr auto self(impl_for<T, Trait> const&& it) -> T const&& {
        static_assert(!std::is_reference_v<T>);
        static_assert(!std::is_reference_v<Trait>);

        return detail::impl_self::self<T, Trait>(std::move(it));
    }
}

#endif
