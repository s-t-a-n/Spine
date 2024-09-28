#pragma once

#include "spine/core/debugging.hpp"

#include <optional>
#include <type_traits>
#include <variant>

namespace spn::structure {

namespace detail {

// Helper metafunctions for unique type list creation
template<typename...>
struct TypeList {};

template<typename List, typename T>
struct Prepend;

template<typename... Ts, typename T>
struct Prepend<TypeList<Ts...>, T> {
    using type = TypeList<T, Ts...>;
};

template<typename List, typename T>
struct Contains;

template<typename T>
struct Contains<TypeList<>, T> : std::false_type {};

template<typename Head, typename... Tail, typename T>
struct Contains<TypeList<Head, Tail...>, T>
    : std::conditional_t<std::is_same_v<Head, T>, std::true_type, Contains<TypeList<Tail...>, T>> {};

template<typename List, typename T>
constexpr bool Contains_v = Contains<List, T>::value;

template<typename... Ts>
struct UniqueHelper;

template<typename Head, typename... Tail>
struct UniqueHelper<Head, Tail...> {
    using type =
        std::conditional_t<Contains_v<typename UniqueHelper<Tail...>::type, Head>, typename UniqueHelper<Tail...>::type,
                           typename Prepend<typename UniqueHelper<Tail...>::type, Head>::type>;
};

template<>
struct UniqueHelper<> {
    using type = TypeList<>;
};

template<typename... Ts>
using Unique = typename UniqueHelper<Ts...>::type;

// Convert TypeList to std::variant
template<typename>
struct ToVariant;

template<typename... Ts>
struct ToVariant<TypeList<Ts...>> {
    using type = std::variant<Ts...>;
};

/// Wrapper to create unique types
template<typename T>
struct Wrapper {
    T value;
    Wrapper(T&& v) : value(std::forward<T>(v)) {}
    Wrapper(const T& v) : value(v) {}
    operator T&() { return value; }
    operator const T&() const { return value; }
};

} // namespace detail

// Only slightly inspired on Rust's Result structure.
// Kudos to Ryan Lucas @ github.com/rlucas585 for the inspiration

/// Container for function-driven processing with safety by default
template<typename T, typename E = T, typename I = T>
class Result {
public:
    Result() : _type(Type::NO_VALUE) {}

    Result(const Result& other) = default;
    Result(Result&& other) noexcept = default;
    Result& operator=(const Result& other) = default;
    Result& operator=(Result&& other) noexcept = default;
    ~Result() = default;

    Result(const T& v) : _v(std::in_place, WrappedT(v)), _type(Type::OK) {}
    Result(T&& v) : _v(std::in_place, WrappedT(std::move(v))), _type(Type::OK) {}

    static Result intermediary(const I& v) { return Result(Type::INTERMEDIARY, WrappedI(v)); }
    static Result intermediary(I&& v) { return Result(Type::INTERMEDIARY, WrappedI(std::move(v))); }

    static Result failed(const E& v) { return Result(Type::FAILED, WrappedE(v)); }
    static Result failed(E&& v) { return Result(Type::FAILED, WrappedE(std::move(v))); }

    bool is_success() const { return _type == Type::OK; }
    bool is_intermediary() const { return _type == Type::INTERMEDIARY; }
    bool is_failed() const { return _type == Type::FAILED; }

    operator bool() const { return _type == Type::OK; }

    const E& error_value() const {
        assert(is_failed());
        assert(_v && std::holds_alternative<WrappedE>(*_v));
        return std::get<WrappedE>(*_v);
    }

    const I& intermediary_value() const {
        assert(is_intermediary());
        assert(_v && std::holds_alternative<WrappedI>(*_v));
        return std::get<WrappedI>(*_v);
    }

    const T& value() const {
        assert(is_success());
        assert(_v && std::holds_alternative<WrappedT>(*_v));
        return std::get<WrappedT>(*_v);
    }

    E unwrap_error_value() {
        assert(is_failed());
        assert(_v && std::holds_alternative<WrappedE>(*_v));
        return std::move(std::get<WrappedE>(*_v));
    }

    I unwrap_intermediary_value() {
        assert(is_intermediary());
        assert(_v && std::holds_alternative<WrappedI>(*_v));
        return std::move(std::get<WrappedI>(*_v));
    }

    T unwrap() {
        assert(is_success() || is_intermediary());
        assert(_v && std::holds_alternative<WrappedT>(*_v));
        return std::move(std::get<WrappedT>(*_v));
    }

    const T* operator->() const { return &value(); }
    const T& operator*() const { return value(); }

    template<typename F>
    /// Chain multiple processors together. Falls through chain immediately on failure or success.
    Result chain(F&& func) {
        using ExpectedResult = Result<std::decay_t<T>, std::decay_t<E>, std::decay_t<I>>;
        static_assert(std::is_invocable_r_v<ExpectedResult, F, I&>,
                      "Function passed to chain must return a Result<T, E, I> and take I& as a parameter.");
        if (is_failed() || is_success()) {
            return *this;
        }
        return func(intermediary_value_mut());
    }

    template<typename F>
    auto map(F&& func) const -> Result<std::invoke_result_t<F, const T&>, E, I> {
        using NewResultType = Result<std::invoke_result_t<F, const T&>, E, I>;
        if (is_success()) return NewResultType(func(value()));
        if (is_failed()) return NewResultType::failed(error_value());
        if (is_intermediary()) return NewResultType::intermediary(intermediary_value());
        return NewResultType(); // NO_VALUE case
    }

    template<typename F>
    auto map_error(F&& func) const -> Result<T, std::invoke_result_t<F, const E&>, I> {
        using NewResultType = Result<T, std::invoke_result_t<F, const E&>, I>;
        if (is_failed()) return NewResultType::failed(func(error_value()));
        if (is_success()) return NewResultType(value());
        if (is_intermediary()) return NewResultType::intermediary(intermediary_value());
        return NewResultType(); // NO_VALUE case
    }

    template<typename F>
    auto map_intermediary(F&& func) const -> Result<T, E, std::invoke_result_t<F, const I&>> {
        using NewResultType = Result<T, E, std::invoke_result_t<F, const I&>>;
        if (is_intermediary()) return NewResultType::intermediary(func(intermediary_value()));
        if (is_success()) return NewResultType(value());
        if (is_failed()) return NewResultType::failed(error_value());
        return NewResultType(); // NO_VALUE case
    }

    template<typename F>
    auto and_then(F&& func) const -> std::invoke_result_t<F, const T&> {
        using NewResultType = std::invoke_result_t<F, const T&>;
        if (is_success()) return func(value()); // Chain the operation
        if (is_failed()) return NewResultType::failed(error_value());
        if (is_intermediary()) return NewResultType::intermediary(intermediary_value());
        return NewResultType(); // NO_VALUE case
    }

    template<typename F>
    auto or_else(F&& func) const -> std::invoke_result_t<F, const E&> {
        using NewResultType = std::invoke_result_t<F, const E&>;
        if (is_failed()) return func(error_value());
        if (is_success()) return NewResultType(value());
        if (is_intermediary()) return NewResultType::intermediary(intermediary_value());
        return NewResultType(); // NO_VALUE case
    }

    template<typename SuccessFn, typename ErrorFn, typename IntermediaryFn>
    auto match(SuccessFn&& success_fn, ErrorFn&& error_fn, IntermediaryFn&& intermediary_fn) const {
        if (is_success()) return success_fn(value());
        if (is_intermediary()) return intermediary_fn(intermediary_value());
        return error_fn(error_value());
    }

    T value_or(T&& default_value) const {
        if (is_success()) return value();
        return std::forward<T>(default_value);
    }

    E error_or(E&& default_error) const {
        if (is_failed()) return error_value();
        return std::forward<E>(default_error);
    }

    template<typename F>
    T unwrap_or_else(F&& fallback_func) const {
        if (is_success()) return value();
        return fallback_func();
    }

protected:
    I& intermediary_value_mut() {
        assert(is_intermediary());
        return std::get<WrappedI>(*_v);
    }

private:
    enum class Type : uint8_t { NO_VALUE, OK, INTERMEDIARY, FAILED };

    using WrappedT = detail::Wrapper<T>;
    using WrappedE = detail::Wrapper<E>;
    using WrappedI = detail::Wrapper<I>;

    using Value = typename detail::ToVariant<detail::Unique<WrappedT, WrappedE, WrappedI>>::type;

    Result(Type type, const Value& v) : _v(v), _type(type) {}
    Result(Type type, Value&& v) : _v(std::move(v)), _type(type) {}

    std::optional<Value> _v;
    Type _type;
};

} // namespace spn::structure
