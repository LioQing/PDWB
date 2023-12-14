#pragma once
#include "fp.hpp"

// _and :: Bool -> Bool -> Bool
template <typename B1, typename B2>
struct _and {
    using Eval = struct _and_Error {};
};

template <bool B1, bool B2>
struct _and<Bool<B1>, Bool<B2>> {
    using Eval = Bool<B1 && B2>;
};

// _or :: Bool -> Bool -> Bool
template <typename B1, typename B2>
struct _or {
    using Eval = struct _or_Error {};
};

template <bool B1, bool B2>
struct _or<Bool<B1>, Bool<B2>> {
    using Eval = Bool<B1 || B2>;
};

// _not :: Bool -> Bool
template <typename B>
struct _not {
    using Eval = struct _not_Error {};
};

template <bool B>
struct _not<Bool<B>> {
    using Eval = Bool<!B>;
};

// negate :: Int -> Int
template <typename I>
struct negate {
    using Eval = struct negate_Error {};
};

template <int I>
struct negate<Int<I>> {
    using Eval = Int<-I>;
};

// add :: Int -> Int -> Int
template <typename I1, typename I2>
struct add {
    using Eval = struct add_Error {};
};

template <int I1, int I2>
struct add<Int<I1>, Int<I2>> {
    using Eval = Int<I1 + I2>;
};

// sub :: Int -> Int -> Int
template <typename I1, typename I2>
struct sub {
    using Eval = struct sub_Error {};
};

template <int I1, int I2>
struct sub<Int<I1>, Int<I2>> {
    using Eval = Int<I1 - I2>;
};

// mul :: Int -> Int -> Int
template <typename I1, typename I2>
struct mul {
    using Eval = struct mul_Error {};
};

template <int I1, int I2>
struct mul<Int<I1>, Int<I2>> {
    using Eval = Int<I1 * I2>;
};

// div :: Int -> Int -> Int
template <typename I1, typename I2>
struct _div {
    using Eval = struct div_Error {};
};

template <int I1, int I2>
struct _div<Int<I1>, Int<I2>> {
    using Eval = Int<I1 / I2>;
};

// mod :: Int -> Int -> Int
template <typename I1, typename I2>
struct mod {
    using Eval = struct mod_Error {};
};

template <int I1, int I2>
struct mod<Int<I1>, Int<I2>> {
    using Eval = Int<I1 % I2>;
};

// eq :: a -> a -> Bool
template <typename T1, typename T2>
struct eq {
    using Eval = Bool<false>;
};

template <typename T>
struct eq<T, T> {
    using Eval = Bool<true>;
};

// neq :: a -> a -> Bool
template <typename T1, typename T2>
struct neq {
    using Eval = typename _not<typename eq<T1, T2>::Eval>::Eval;
};

// lt :: Int -> Int -> Bool
template <typename I1, typename I2>
struct lt {
    using Eval = struct lt_Error {};
};

template <int I1, int I2>
struct lt<Int<I1>, Int<I2>> {
    using Eval = Bool<(I1 < I2)>;
};

// gt :: Int -> Int -> Bool
template <typename I1, typename I2>
struct gt {
    using Eval = struct gt_Error {};
};

template <int I1, int I2>
struct gt<Int<I1>, Int<I2>> {
    using Eval = Bool<(I1 > I2)>;
};

// lte :: Int -> Int -> Bool
template <typename I1, typename I2>
struct lte {
    using Eval = struct lte_Error {};
};

template <int I1, int I2>
struct lte<Int<I1>, Int<I2>> {
    using Eval = Bool<(I1 <= I2)>;
};

// gte :: Int -> Int -> Bool
template <typename I1, typename I2>
struct gte {
    using Eval = struct gte_Error {};
};

template <int I1, int I2>
struct gte<Int<I1>, Int<I2>> {
    using Eval = Bool<(I1 >= I2)>;
};

// fst :: (a, b) -> a
template <typename T>
struct fst {
    using Eval = struct fst_Error {};
};

template <typename X, typename ...Xs>
struct fst<Tuple<X, Xs...>> {
    using Eval = X;
};

// snd :: (a, b) -> b
template <typename T>
struct snd {
    using Eval = struct snd_Error {};
};

template <typename X, typename Y, typename ...Ys>
struct snd<Tuple<X, Y, Ys...>> {
    using Eval = Y;
};

// idx :: Int -> [a] -> a
template <typename I, typename L>
struct idx {
    using Eval = struct idx_Error {};
};

template <int I>
struct idx<Int<I>, List<Nil>> {
    using Eval = struct idx_Error {};
};

template <int I, typename X, typename XS>
struct idx<Int<I>, List<Cons<X, XS>>> {
    using Eval = typename IfThenElse<
        typename eq<Int<I>, Int<0>>::Eval,
        X,
        typename idx<Int<I - 1>, XS>::Eval
    >::Eval;
};

// unwrap :: Maybe a -> a
template <typename T>
struct unwrap {
    using Eval = struct unwrap_Error {};
};

template <typename X>
struct unwrap<Maybe<Just<X>>> {
    using Eval = X;
};

// id :: a -> a
template <typename T>
struct id {
    using Eval = typename T::Eval;
};

// map :: (a -> b) -> [a] -> [b]
template <template <typename> typename F, typename L>
struct map {
    using Eval = struct map_Error {};
};

template <template <typename> typename F>
struct map<F, List<Nil>> {
    using Eval = Nil::Eval;
};

template <template <typename> typename F, typename X, typename XS>
struct map<F, List<Cons<X, XS>>> {
    using Eval = typename Cons<typename F<X>::Eval, typename map<F, XS>::Eval>::Eval;
};

// filter :: (a -> Bool) -> [a] -> [a]
template <template <typename> typename F, typename L>
struct filter {
    using Eval = struct filter_Error {};
};

template <template <typename> typename F>
struct filter<F, List<Nil>> {
    using Eval = Nil::Eval;
};

template <template <typename> typename F, typename X, typename XS>
struct filter<F, List<Cons<X, XS>>> {
    using Eval = typename IfThenElse<
        typename F<X>::Eval,
        typename Cons<X, typename filter<F, XS>::Eval>::Eval,
        typename filter<F, XS>::Eval
    >::Eval;
};

// foldl :: (b -> a -> b) -> b -> [a] -> b
template <template <typename, typename> typename F, typename B, typename L>
struct foldl {
    using Eval = struct foldl_Error {};
};

template <template <typename, typename> typename F, typename B>
struct foldl<F, B, List<Nil>> {
    using Eval = B;
};

template <template <typename, typename> typename F, typename B, typename X, typename XS>
struct foldl<F, B, List<Cons<X, XS>>> {
    using Eval = typename foldl<F, typename F<B, X>::Eval, XS>::Eval;
};

// foldr :: (a -> b -> b) -> b -> [a] -> b
template <template <typename, typename> typename F, typename B, typename L>
struct foldr {
    using Eval = struct foldr_Error {};
};

template <template <typename, typename> typename F, typename B>
struct foldr<F, B, List<Nil>> {
    using Eval = B;
};

template <template <typename, typename> typename F, typename B, typename X, typename XS>
struct foldr<F, B, List<Cons<X, XS>>> {
    using Eval = typename F<X, typename foldr<F, B, XS>::Eval>::Eval;
};

// takeWhile :: (a -> Bool) -> [a] -> [a]
template <template <typename> typename F, typename L>
struct takeWhile {
    using Eval = struct takeWhile_Error {};
};

template <template <typename> typename F>
struct takeWhile<F, List<Nil>> {
    using Eval = Nil::Eval;
};

template <template <typename> typename F, typename X, typename XS>
struct takeWhile<F, List<Cons<X, XS>>> {
    using Eval = typename IfThenElse<
        typename F<X>::Eval,
        typename Cons<X, typename takeWhile<F, XS>::Eval>::Eval,
        Nil::Eval
    >::Eval;
};

// dropWhile :: (a -> Bool) -> [a] -> [a]
template <template <typename> typename F, typename L>
struct dropWhile {
    using Eval = struct dropWhile_Error {};
};

template <template <typename> typename F>
struct dropWhile<F, List<Nil>> {
    using Eval = Nil::Eval;
};

template <template <typename> typename F, typename X, typename XS>
struct dropWhile<F, List<Cons<X, XS>>> {
    using Eval = typename IfThenElse<
        typename F<X>::Eval,
        typename dropWhile<F, XS>::Eval,
        typename Cons<X, XS>::Eval
    >::Eval;
};

// lookup :: Eq a => a -> [(a, b)] -> Maybe b
template <typename A, typename L>
struct lookup {
    using Eval = struct lookup_Error {};
};

template <typename A>
struct lookup<A, List<Nil>> {
    using Eval = Nothing::Eval;
};

template <typename A, typename X1, typename X2, typename XS>
struct lookup<A, List<Cons<Tuple<X1, X2>, XS>>> {
    using Eval = typename IfThenElse<
        typename eq<A, X1>::Eval,
        typename Just<X2>::Eval,
        typename lookup<A, XS>::Eval
    >::Eval;
};
