#pragma once

#include <optional>
#include <type_traits>
#include <variant>

namespace spn::core::meta::unique_variant {

/*
 * =========================================================================================
 * ChatGPT Generated Code
 * =========================================================================================
 *
 * This code was generated by OpenAI's ChatGPT, an AI language model trained to assist with
 * coding, documentation, and various programming tasks.
 *
 * Version: ChatGPT v2 (As of July 2024)
 * Model: GPT-4-turbo
 *
 * Note: While this code is intended to be useful, it's important to review and test it
 * thoroughly before integrating it into your project. Always ensure that it meets your
 * specific needs and adheres to best practices.
 *
 * =========================================================================================
 */

// Note from stan: until C++20 is supported and concepts can be used, let the AI play his flute!
// todo: marked for C++20 refactor

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

/*
   How to use

   namespace uv = spn::core::meta::unique_variant;

   using WrappedT = uv::Wrapper<T>;
   using WrappedE = uv::Wrapper<E>;
   using WrappedI = uv::Wrapper<I>;

   using Value = typename uv::ToVariant<uv::Unique<WrappedT, WrappedE, WrappedI>>::type;
 */

} // namespace spn::core::meta::unique_variant