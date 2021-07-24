#pragma once

#include "spine/core/debugging.hpp"

#include <optional>
#include <type_traits>
#include <variant>

namespace spn::structure {

namespace detail {

// C++ has its quirks, such as std::variant not allowing duplicate types in its template parameters.
// This unique solution was crafted with the assistance of ChatGPT 3.5 to handle such cases elegantly.
// (note from monkey: the whole lot below with exception of the Result class is chatgpt's doing after untangling
// kilometers of compiler errors into a quite neat solution)

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

// inspired by the work of Ryan Lucas:
// https://github.com/rlucas585/webserv/blob/develop/src/Result/src/result.hpp

/// Container for working through data with
template<typename T, typename E = T, typename I = T>
class Result {
public:
    enum class Type : uint8_t { UNDEFINED, OK, INTERMEDIARY, FAILED };

public:
    Result() : _type(Type::UNDEFINED) {}

    Result(const Result& other) : _v(other._v), _type(other._type) {}
    Result(Result&& other) noexcept : _v(std::move(other._v)), _type(other._type) {}
    Result& operator=(const Result& other) {
        if (this == &other) return *this;
        _v = other._v;
        _type = other._type;
        return *this;
    }
    Result& operator=(Result&& other) noexcept {
        if (this == &other) return *this;
        _v = std::move(other._v);
        _type = other._type;
        return *this;
    }
    ~Result() = default;

    Result(const T& v) : _v(std::in_place, WrappedT(v)), _type(Type::OK) {}
    Result(T&& v) : _v(std::in_place, WrappedT(std::move(v))), _type(Type::OK) {}

    bool is_success() const { return _type == Type::OK; }

    static Result intermediary(const I& v) { return Result(Type::INTERMEDIARY, WrappedI(v)); }
    static Result intermediary(I&& v) { return Result(Type::INTERMEDIARY, WrappedI(std::move(v))); }
    bool is_intermediary() const { return _type == Type::INTERMEDIARY; }

    static Result failed(const E& v) { return Result(Type::FAILED, WrappedE(v)); }
    static Result failed(E&& v) { return Result(Type::FAILED, WrappedE(std::move(v))); }
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

    const T* operator->() { return &value(); }
    const T& operator*() const { return value(); }

    template<typename F>
    Result chain(F&& func) {
        using ResultType = Result<std::decay_t<T>, std::decay_t<E>, std::decay_t<I>>;
        static_assert(std::is_same_v<ResultType, decltype(func(std::declval<I&>()))>,
                      "Function passed to chain must return a Result<T, E, I> type.");
        if (is_failed() || is_success()) {
            return *this;
        }
        return func(intermediary_value_mut());
    }

protected:
    I& intermediary_value_mut() {
        assert(is_intermediary());
        assert(_v && std::holds_alternative<WrappedI>(*_v));
        return std::get<WrappedI>(*_v);
    }

private:
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
