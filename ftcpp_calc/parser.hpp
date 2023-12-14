#pragma once
#include "fp.hpp"
#include "char.hpp"

// Parser data type
template <typename T>
struct Parser;

template <template <typename> typename F>
struct P {
    using Eval = Parser<P<F>>;
};

// parse :: Parser a -> String -> Maybe (a, String)
template <typename Q, typename Inp>
struct parse {
    using Eval = struct parse_Error {};
};

template <template <typename> typename F, char C, typename CS>
struct parse<Parser<P<F>>, List<Cons<Char<C>, CS>>> {
    using Eval = typename F<List<Cons<Char<C>, CS>>>::Eval;
};

template <template <typename> typename F>
struct parse<Parser<P<F>>, List<Nil>> {
    using Eval = typename F<List<Nil>>::Eval;
};

// ret :: a -> Parser a
template <typename V>
struct ret {
    template <typename Inp>
    struct _ret_1 {
        using Eval = typename Just<Tuple<V, Inp>>::Eval;
    };

    using Eval = typename P<_ret_1>::Eval;
};

// bind :: Parser a -> (a -> Parser b) -> Parser b
namespace _bind_namespace {
    template <typename X>
    struct _bind_1_1_1 {
        using Eval = struct _bind_1_1_1_Error {};
    };

    template <typename V, typename Out>
    struct _bind_1_1_1<Tuple<V, Out>> {
        using Eval = V;
    };

    template <typename X>
    struct _bind_1_1_2 {
        using Eval = struct _bind_1_1_2_Error {};
    };

    template <typename V, typename Out>
    struct _bind_1_1_2<Tuple<V, Out>> {
        using Eval = Out;
    };

    template <template <typename> typename F, typename X>
    struct _bind_1_1 {
        using Eval = struct _bind_1_1_Error {};
    };

    template <template <typename> typename F>
    struct _bind_1_1<F, Maybe<Nothing>> {
        using Eval = Nothing::Eval;
    };

    template <template <typename> typename F, typename X>
    struct _bind_1_1<F, Maybe<Just<X>>> {
        using Eval = typename parse<typename F<typename _bind_1_1_1<X>::Eval>::Eval, typename _bind_1_1_2<X>::Eval>::Eval;
    };
}

template <typename Q, template <typename> typename F>
struct bind {
    using Eval = struct bind_Error {};
};

template <template <typename> typename Q, template <typename> typename F>
struct bind<Parser<P<Q>>, F> {
    template <typename Inp>
    struct _bind_1 {
        using Eval = typename _bind_namespace::_bind_1_1<F, typename parse<typename P<Q>::Eval, Inp>::Eval>::Eval;
    };

    using Eval = typename P<_bind_1>::Eval;
};

// alt :: Parser a -> Parser a -> Parser a
namespace _alt_namespace {
    template <typename Q, typename Inp, typename X>
    struct _alt_1_1 {
        using Eval = struct _alt_1_1_Error {};
    };

    template <template <typename> typename Q, typename Inp>
    struct _alt_1_1<Parser<P<Q>>, Inp, Maybe<Nothing>> {
        using Eval = typename parse<typename P<Q>::Eval, Inp>::Eval;
    };

    template <template <typename> typename Q, typename Inp, typename X>
    struct _alt_1_1<Parser<P<Q>>, Inp, X> {
        using Eval = X;
    };
}

template <typename Q, typename R>
struct alt {
    using Eval = struct alt_Error {};
};

template <template <typename> typename Q, template <typename> typename R>
struct alt<Parser<P<Q>>, Parser<P<R>>> {
    template <typename Inp>
    struct _alt_1 {
        using Eval = typename _alt_namespace::_alt_1_1<typename P<R>::Eval, Inp, typename parse<typename P<Q>::Eval, Inp>::Eval>::Eval;
    };

    using Eval = typename P<_alt_1>::Eval;
};

// failure :: Parser a
struct failure {
    template <typename Inp>
    struct _failure_1 {
        using Eval = Nothing::Eval;
    };

    using Eval = typename P<_failure_1>::Eval;
};

// item :: Parser Char
namespace _item_namespace {
    template <typename Inp>
    struct _item_1_1 {
        using Eval = struct _item_1_1_Error {};
    };

    template <>
    struct _item_1_1<List<Nil>> {
        using Eval = Nothing::Eval;
    };

    template <typename X, typename XS>
    struct _item_1_1<List<Cons<X, XS>>> {
        using Eval = typename Just<Tuple<X, XS>>::Eval;
    };
}

struct item {
    template <typename Inp> 
    struct _item_1 {
        using Eval = typename _item_namespace::_item_1_1<Inp>::Eval;
    };

    using Eval = typename P<_item_1>::Eval;
};

// sat :: (Char -> Bool) -> Parser Char
template <template <typename> typename Q>
struct sat {
    template <typename C>
    struct _sat_1 {
        using Eval = typename IfThenElse<
            typename Q<C>::Eval,
            typename ret<C>::Eval,
            typename failure::Eval
        >::Eval;
    };

    using Eval = typename bind<item::Eval, _sat_1>::Eval;
};

// char :: Char -> Parser Char
template <typename C>
struct pChar {
    using Eval = struct pChar_Error {};
};

template <char C>
struct pChar<Char<C>> {
    template <typename X>
    struct _char_1 {
        using Eval = typename eq<Char<C>, X>::Eval;
    };

    using Eval = typename sat<_char_1>::Eval;
};

// digit :: Parser Char
struct digit {
    using Eval = typename sat<isDigit>::Eval;
};

// lower :: Parser Char
struct lower {
    using Eval = typename sat<isLower>::Eval;
};

// upper :: Parser Char
struct upper {
    using Eval = typename sat<isUpper>::Eval;
};

// alpha :: Parser Char
struct alpha {
    using Eval = typename sat<isAlpha>::Eval;
};

// alphanum :: Parser Char
struct alphanum {
    using Eval = typename sat<isAlphaNum>::Eval;
};

// space :: Parser Char
struct space {
    using Eval = typename sat<isSpace>::Eval;
};

// string :: String -> Parser String
template <typename S>
struct pString {
    using Eval = struct pString_Error {};
};

template <>
struct pString<List<Nil>> {
    using Eval = typename ret<List<Nil>>::Eval;
};

template <char X, typename XS>
struct pString<List<Cons<Char<X>, XS>>> {
    template <typename C>
    struct _pString_1 {
        template <typename CS>
        struct _pString_1_1 {
            using Eval = typename ret<typename Cons<Char<X>, XS>::Eval>::Eval;
        };

        using Eval = typename bind<typename pString<XS>::Eval, _pString_1_1>::Eval;
    };

    using Eval = typename bind<typename pChar<Char<X>>::Eval, _pString_1>::Eval;
};

// many :: Parser a -> Parser [a]
template <typename Q> struct many1;

template <typename Q>
struct many {
    using Eval = struct many_Error {};
};

template <template <typename> typename Q>
struct many<Parser<P<Q>>> {
    using Eval = typename alt<
        typename many1<typename P<Q>::Eval>::Eval,
        typename ret<Nil::Eval>::Eval
    >::Eval;
};

// many1 :: Parser a -> Parser [a]
template <typename Q>
struct many1 {
    using Eval = struct many1_Error {};
};

template <template <typename> typename Q>
struct many1<Parser<P<Q>>> {
    template <typename X>
    struct _many1_1 {
        template <typename XS>
        struct _many1_1_1 {
            using Eval = typename ret<typename Cons<X, XS>::Eval>::Eval;
        };

        using Eval = typename bind<typename many<typename P<Q>::Eval>::Eval, _many1_1_1>::Eval;
    };

    using Eval = typename bind<typename P<Q>::Eval, _many1_1>::Eval;
};

// nat :: Parser Int
struct nat {
    template <typename XS>
    struct _nat_1 {
        using Eval = typename ret<typename parseInt<XS>::Eval>::Eval;
    };

    using Eval = typename bind<typename many1<digit::Eval>::Eval, _nat_1>::Eval;
};

// int :: Parser Int
struct pInt {
    struct _pInt_1 {
        template <typename X>
        struct _pInt_1_1 {
            template <typename N>
            struct _pInt_1_1_1 {
                using Eval = typename ret<typename negate<N>::Eval>::Eval;
            };

            using Eval = typename bind<
                nat::Eval,
                _pInt_1_1_1
            >::Eval;
        };

        using Eval = typename bind<
            typename pChar<Char<'-'>>::Eval,
            _pInt_1_1
        >::Eval;
    };

    using Eval = typename alt<
        _pInt_1::Eval,
        nat::Eval
    >::Eval;
};

// spaces :: Parser String
struct spaces {
    using Eval = typename many<space::Eval>::Eval;
};

// token :: Parser a -> Parser a
template <typename Q>
struct token {
    using Eval = struct token_Error {};
};

template <template <typename> typename Q>
struct token<Parser<P<Q>>> {
    template <typename _1>
    struct _token_1 {
        template <typename V>
        struct _token_1_1 {
            template <typename _2>
            struct _token_1_1_1 {
                using Eval = typename ret<V>::Eval;
            };

            using Eval = typename bind<
                typename many<space::Eval>::Eval,
                _token_1_1_1
            >::Eval;
        };

        using Eval = typename bind<
            typename P<Q>::Eval,
            _token_1_1
        >::Eval;
    };

    using Eval = typename bind<
        typename many<space::Eval>::Eval,
        _token_1
    >::Eval;
};

// symbol :: String -> Parser String
template <typename S>
struct symbol {
    using Eval = typename token<typename pString<S>::Eval>::Eval;
};

// natural :: Parser Int
struct natural {
    using Eval = typename token<nat::Eval>::Eval;
};

// integer :: Parser Int
struct integer {
    using Eval = typename token<pInt::Eval>::Eval;
};
