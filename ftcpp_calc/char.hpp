#pragma once
#include "prelude.hpp"

// chr :: Int -> Char
template <typename I>
struct chr {
    using Eval = struct chr_Error {};
};

template <int I>
struct chr<Int<I>> {
    using Eval = Char<I>;
};

// ord :: Char -> Int
template <typename C>
struct ord {
    using Eval = struct ord_Error {};
};

template <char C>
struct ord<Char<C>> {
    using Eval = Int<C>;
};

// isDigit :: Char -> Bool
template <typename C>
struct isDigit {
    using Eval = struct isDigit_Error {};
};

template <char C>
struct isDigit<Char<C>> {
    using Eval = typename _and<
        typename gte<typename ord<Char<C>>::Eval, typename ord<Char<'0'>>::Eval>::Eval,
        typename lte<typename ord<Char<C>>::Eval, typename ord<Char<'9'>>::Eval>::Eval
    >::Eval;
};

// isLower :: Char -> Bool
template <typename C>
struct isLower {
    using Eval = struct isLower_Error {};
};

template <char C>
struct isLower<Char<C>> {
    using Eval = typename _and<
        typename gte<typename ord<Char<C>>::Eval, typename ord<Char<'a'>>::Eval>::Eval,
        typename lte<typename ord<Char<C>>::Eval, typename ord<Char<'z'>>::Eval>::Eval
    >::Eval;
};

// isUpper :: Char -> Bool
template <typename C>
struct isUpper {
    using Eval = struct isUpper_Error {};
};

template <char C>
struct isUpper<Char<C>> {
    using Eval = typename _and<
        typename gte<typename ord<Char<C>>::Eval, typename ord<Char<'A'>>::Eval>::Eval,
        typename lte<typename ord<Char<C>>::Eval, typename ord<Char<'Z'>>::Eval>::Eval
    >::Eval;
};

// isAlpha :: Char -> Bool
template <typename C>
struct isAlpha {
    using Eval = struct isAlpha_Error {};
};

template <char C>
struct isAlpha<Char<C>> {
    using Eval = typename _or<
        typename isLower<Char<C>>::Eval,
        typename isUpper<Char<C>>::Eval
    >::Eval;
};

// isAlphaNum :: Char -> Bool
template <typename C>
struct isAlphaNum {
    using Eval = struct isAlphaNum_Error {};
};

template <char C>
struct isAlphaNum<Char<C>> {
    using Eval = typename _or<
        typename isAlpha<Char<C>>::Eval,
        typename isDigit<Char<C>>::Eval
    >::Eval;
};

// isSpace :: Char -> Bool
template <typename C>
struct isSpace {
    using Eval = struct isSpace_Error {};
};

template <char C>
struct isSpace<Char<C>> {
    using isSpace_2 = typename _or<
        typename eq<Char<C>, Char<'\n'>>::Eval,
        typename eq<Char<C>, Char<'\r'>>::Eval
    >::Eval;

    using isSpace_1 = typename _or<
        typename eq<Char<C>, Char<'\t'>>::Eval,
        isSpace_2
    >::Eval;

    using Eval = typename _or<
        typename eq<Char<C>, Char<' '>>::Eval,
        isSpace_1
    >::Eval;
};

// parseInt :: String -> Maybe Int
template <typename X>
struct parseInt {
    using Eval = struct parseInt_Error {};
};

template <typename X, typename XS>
struct parseInt<List<Cons<X, XS>>> {
    template <typename N, typename Y>
    struct _parseInt_1 {
        struct _parseInt_1_isNotDigit_Error {};

        using Eval = typename IfThenElse<
            typename isDigit<Y>::Eval,
            typename add<
                typename mul<N, Int<10>>::Eval,
                typename sub<
                    typename ord<Y>::Eval,
                    typename ord<Char<'0'>>::Eval
                >::Eval
            >::Eval,
            _parseInt_1_isNotDigit_Error
        >::Eval;
    };

    using Eval = typename foldl<
        _parseInt_1,
        Int<0>,
        typename Cons<X, XS>::Eval
    >::Eval;
};
