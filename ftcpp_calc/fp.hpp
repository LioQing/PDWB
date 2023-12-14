#pragma once
#include <string>

template <typename T> struct show {};

// Unit
struct Unit {};

// Error
struct Error {};

// Bool
template <bool B> struct Bool {};

template <bool B> struct show<Bool<B>> {
    static std::string to_string() {
        if constexpr (B) {
            return "true";
        } else {
            return "false";
        }
    }
};

// Int
template <int I> struct Int : std::integral_constant<int, I> {};

template <int I> struct show<Int<I>> {
    static const std::string to_string() {
        return std::to_string(I);
    }
};

// Char
template <char C> struct Char : std::integral_constant<char, C> {};

template <char C> struct show<Char<C>> {
    static const std::string to_string() {
        return std::string(1, '\'') + std::string(1, C) + "'";
    }
};

// Tuple
template <typename ...Ts> struct Tuple {};

template <typename ...Ts> struct show<Tuple<Ts...>> {
    static std::string to_string() {
        std::string s("(");
        bool first = true;
        ((s += (first ? "" : ", ") + show<Ts>::to_string(), first = false), ...);
        return s + ")";
    }
};

// List
template <typename T> struct List;

struct Nil { using Eval = List<Nil>; };

template <typename X, typename XS> struct Cons { struct Cons_Error {}; using Eval = Cons_Error; };
template <typename X, typename XS> struct Cons<X, List<XS>> { using Eval = List<Cons<X, List<XS>>>; };

template <typename T> struct List {};
template <> struct List<Nil> {};
template <typename X, typename XS> struct List<Cons<X, XS>> {};

template <> struct show<List<Nil>> {
    static std::string _to_string() {
        return "";
    }

    static std::string to_string() {
        return "[]";
    }
};

template <typename X, typename XS> struct show<List<Cons<X, XS>>> {
    static std::string _to_string() {
        if constexpr (std::is_same<XS, List<Nil>>::value) {
            return show<X>::to_string();
        } else {
            return show<X>::to_string() + ", " + show<XS>::_to_string();
        }
    }

    static std::string to_string() {
        return std::string(1, '[') + _to_string() + "]";
    }
};

// String
template <typename C, typename CS> struct String { struct String_Error; using Eval = String_Error; };
template <char C, typename CS> struct String<Char<C>, CS> { using Eval = typename Cons<Char<C>, CS>::Eval; };
template <char C> struct String<Char<C>, Nil> { using Eval = typename Cons<Char<C>, Nil::Eval>::Eval; };

template <char C, typename CS> struct show<List<Cons<Char<C>, CS>>> {
    static std::string _to_string() {
        if constexpr (std::is_same<CS, List<Nil>>::value) {
            return std::string(1, C);
        } else {
            return std::string(1, C) + show<CS>::_to_string();
        }
    }

    static std::string to_string() {
        return std::string(1, '"') + _to_string() + "\"";
    }
};

// Literal
namespace _Literal_namespace {
    constexpr size_t strlen(const char* s) {
        size_t i = 0;
        while (s[i] != '\0') {
            i++;
        }
        return i;
    }

    template <const char* S, size_t I, size_t N> struct _Literal_1 {
        using Eval = typename String<Char<S[I]>, typename _Literal_1<S, I + 1, N>::Eval>::Eval;
    };

    template <const char* S, size_t N> struct _Literal_1<S, N, N> {
        using Eval = Nil::Eval;
    };
}

template <const char* S>
using Literal = _Literal_namespace::_Literal_1<S, 0, _Literal_namespace::strlen(S)>;

// Maybe
template <typename T> struct Maybe;

struct Nothing { using Eval = Maybe<Nothing>; };

template <typename T> struct Just { using Eval = Maybe<Just<T>>; };

template <typename T> struct Maybe {};
template <> struct Maybe<Nothing> {};
template <typename T> struct Maybe<Just<T>> {};

template <> struct show<Maybe<Nothing>> {
    static std::string to_string() {
        return "Nothing";
    }
};

template <typename T> struct show<Maybe<Just<T>>> {
    static std::string to_string() {
        return "Just " + show<T>::to_string();
    }
};

// IfThenElse
template <typename B, typename T, typename E> struct IfThenElse { struct IfThenElse_Error; using Eval = IfThenElse_Error; };
template <typename T, typename E> struct IfThenElse<Bool<true>, T, E> { using Eval = T; };
template <typename T, typename E> struct IfThenElse<Bool<false>, T, E> { using Eval = E; };