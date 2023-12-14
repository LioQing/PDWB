#include <type_traits>
#include <iostream>
#include "fp.hpp"
#include "parser.hpp"
#include "char.hpp"
#include "prelude.hpp"

// data Binop = Add | Sub | Mul | Div | Mod
template <typename O> struct Binop;

struct Add { using Eval = Binop<Add>; };
struct Sub { using Eval = Binop<Sub>; };
struct Mul { using Eval = Binop<Mul>; };
struct Div { using Eval = Binop<Div>; };
struct Mod { using Eval = Binop<Mod>; };

template <typename O> struct Binop {};
template <> struct Binop<Add> {};
template <> struct Binop<Sub> {};
template <> struct Binop<Mul> {};
template <> struct Binop<Div> {};
template <> struct Binop<Mod> {};

template <> struct show<Binop<Add>> {
    static std::string to_string() { return "Add"; }
};

template <> struct show<Binop<Sub>> {
    static std::string to_string() { return "Sub"; }
};

template <> struct show<Binop<Mul>> {
    static std::string to_string() { return "Mul"; }
};

template <> struct show<Binop<Div>> {
    static std::string to_string() { return "Div"; }
};

template <> struct show<Binop<Mod>> {
    static std::string to_string() { return "Mod"; }
};

// data Expr = Bin Binop Expr Expr
//           | Val Int
//           | Var String
template <typename E> struct Expr;

template <typename B, typename E1, typename E2>
struct Bin {
    using Eval = struct Bin_Error {};
};

template <typename B, typename E1, typename E2>
struct Bin<Binop<B>, Expr<E1>, Expr<E2>> {
    using Eval = Expr<Bin<Binop<B>, Expr<E1>, Expr<E2>>>;
};

template <typename I>
struct Val {
    using Eval = struct Val_Error {};
};

template <int I>
struct Val<Int<I>> {
    using Eval = Expr<Val<Int<I>>>;
};

template <typename S>
struct Var {
    using Eval = struct Var_Error {};
};

template <char C, typename CS>
struct Var<List<Cons<Char<C>, CS>>> {
    using Eval = Expr<Var<List<Cons<Char<C>, CS>>>>;
};

template <typename E> struct Expr {};
template <typename B, typename E1, typename E2> struct Expr<Bin<B, E1, E2>> {};
template <typename I> struct Expr<Val<I>> {};
template <typename S> struct Expr<Var<S>> {};

template <typename B, typename E1, typename E2> struct show<Expr<Bin<B, E1, E2>>> {
    static std::string to_string() { return "Bin(" + show<B>::to_string() + ", " + show<E1>::to_string() + ", " + show<E2>::to_string() + ")"; }
};

template <typename I> struct show<Expr<Val<I>>> {
    static std::string to_string() { return "Val(" + show<I>::to_string() + ")"; }
};

template <typename S> struct show<Expr<Var<S>>> {
    static std::string to_string() { return "Var(" + show<S>::to_string() + ")"; }
};

// type Env = [(String, Int)]
template <typename E, typename ES>
struct Env {
    using Eval = struct Env_Error {};
};

template <char X, typename XS, int I, typename ES>
struct Env<Tuple<List<Cons<Char<X>, XS>>, Int<I>>, ES> {
    using Eval = List<Cons<Tuple<List<Cons<Char<X>, XS>>, Int<I>>, ES>>;
};

// Parsers
struct pExpr;

// pFactor :: Parser Expr
struct pFactor {
    // pParen :: Parser Expr
    struct pParen {
        constexpr static char openParenSymbol[] = "(";
        constexpr static char closeParenSymbol[] = ")";

        template <typename _1>
        struct _pParen_1;

        using Eval = typename bind<
            typename symbol<typename Literal<openParenSymbol>::Eval>::Eval,
            _pParen_1
        >::Eval;
    };

    // pInteger :: Parser Expr
    struct pInteger {
        template <typename I>
        struct _pInteger_1 {
            using Eval = typename ret<typename Val<I>::Eval>::Eval;
        };

        using Eval = typename bind<
            integer::Eval,
            _pInteger_1
        >::Eval;
    };

    // pIdentifier :: Parser Expr
    struct pIdentifier {
        struct _pIdentifier_1 {
            struct _pIdentifier_1_1 {
                template <typename X>
                struct _pIdentifier_1_1_1 {
                    template <typename XS>
                    struct _pIdentifier_1_1_1_1 {
                        using Eval = typename ret<typename Cons<X, XS>::Eval>::Eval;
                    };

                    using Eval = typename bind<
                        typename many<alphanum::Eval>::Eval,
                        _pIdentifier_1_1_1_1
                    >::Eval;
                };

                using Eval = typename bind<
                    alpha::Eval,
                    _pIdentifier_1_1_1
                >::Eval;
            };

            using Eval = typename token<_pIdentifier_1_1::Eval>::Eval;
        };

        template <typename S>
        struct _pIdentifier_2 {
            using Eval = typename ret<typename Var<S>::Eval>::Eval;
        };

        using Eval = typename bind<
            _pIdentifier_1::Eval,
            _pIdentifier_2
        >::Eval;
    };

    using Eval = typename alt<
        pParen::Eval,
        typename alt<
            pInteger::Eval,
            pIdentifier::Eval
        >::Eval
    >::Eval;
};

// pBinopFactor :: Parser Binop
struct pBinopFactor {
    // pMul :: Parser Binop
    struct pMul {
        constexpr static char mulSymbol[] = "*";

        template <typename _1>
        struct _pMul_1 {
            using Eval = typename ret<Mul::Eval>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<mulSymbol>::Eval>::Eval,
            _pMul_1
        >::Eval;
    };

    // pDiv :: Parser Binop
    struct pDiv {
        constexpr static char divSymbol[] = "/";

        template <typename _1>
        struct _pDiv_1 {
            using Eval = typename ret<Div::Eval>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<divSymbol>::Eval>::Eval,
            _pDiv_1
        >::Eval;
    };

    // pMod :: Parser Binop
    struct pMod {
        constexpr static char modSymbol[] = "%";

        template <typename _1>
        struct _pMod_1 {
            using Eval = typename ret<Mod::Eval>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<modSymbol>::Eval>::Eval,
            _pMod_1
        >::Eval;
    };

    using Eval = typename alt<
        pMul::Eval,
        typename alt<
            pDiv::Eval,
            pMod::Eval
        >::Eval
    >::Eval;
};

// pOpFactor :: Parser [(Binop, Expr)]
struct pOpFactor {
    struct _pOpFactor_1 {
        template <typename B>
        struct _pOpFactor_1_1;

        using Eval = typename bind<
            pBinopFactor::Eval,
            _pOpFactor_1_1
        >::Eval;
    };

    using Eval = typename alt<
        _pOpFactor_1::Eval,
        typename ret<Nil::Eval>::Eval
    >::Eval;
};

template <typename B>
struct pOpFactor::_pOpFactor_1::_pOpFactor_1_1 {
    template <typename F>
    struct _pOpFactor_1_1_1 {
        template <typename O>
        struct _pOpFactor_1_1_1_1 {
            using Eval = typename ret<typename Cons<Tuple<B, F>, O>::Eval>::Eval;
        };

        using Eval = typename bind<
            pOpFactor::Eval,
            _pOpFactor_1_1_1_1
        >::Eval;
    };

    using Eval = typename bind<
        pFactor::Eval,
        _pOpFactor_1_1_1
    >::Eval;
};

// pTerm :: Parser Expr
struct pTerm {
    template <typename F>
    struct _pTerm_1;

    using Eval = typename bind<
        pFactor::Eval,
        _pTerm_1
    >::Eval;
};

template <typename F>
struct pTerm::_pTerm_1 {
    template <typename O>
    struct _pTerm_1_1 {
        template <typename L, typename R>
        struct _pTerm_1_1_1 {
            using Eval = typename Bin<typename fst<R>::Eval, L, typename snd<R>::Eval>::Eval;
        };

        using Eval = typename ret<typename foldl<
            _pTerm_1_1_1,
            F,
            O
        >::Eval>::Eval;
    };

    using Eval = typename bind<
        pOpFactor::Eval,
        _pTerm_1_1
    >::Eval;
};

// pBinopTerm :: Parser Binop
struct pBinopTerm {
    // pAdd :: Parser Binop
    struct pAdd {
        constexpr static char addSymbol[] = "+";

        template <typename _1>
        struct _pAdd_1 {
            using Eval = typename ret<Add::Eval>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<addSymbol>::Eval>::Eval,
            _pAdd_1
        >::Eval;
    };

    // pSub :: Parser Binop
    struct pSub {
        constexpr static char subSymbol[] = "-";

        template <typename _1>
        struct _pSub_1 {
            using Eval = typename ret<Sub::Eval>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<subSymbol>::Eval>::Eval,
            _pSub_1
        >::Eval;
    };

    using Eval = typename alt<
        pAdd::Eval,
        pSub::Eval
    >::Eval;
};

// pOpTerm :: Parser [(Binop, Expr)]
struct pOpTerm {
    struct _pOpTerm_1 {
        template <typename B>
        struct _pOpTerm_1_1;

        using Eval = typename bind<
            pBinopTerm::Eval,
            _pOpTerm_1_1
        >::Eval;
    };

    using Eval = typename alt<
        _pOpTerm_1::Eval,
        typename ret<Nil::Eval>::Eval
    >::Eval;
};

template <typename B>
struct pOpTerm::_pOpTerm_1::_pOpTerm_1_1 {
    template <typename T>
    struct _pOpTerm_1_1_1 {
        template <typename O>
        struct _pOpTerm_1_1_1_1 {
            using Eval = typename ret<typename Cons<Tuple<B, T>, O>::Eval>::Eval;
        };

        using Eval = typename bind<
            pOpTerm::Eval,
            _pOpTerm_1_1_1_1
        >::Eval;
    };

    using Eval = typename bind<
        pTerm::Eval,
        _pOpTerm_1_1_1
    >::Eval;
};

// pExpr :: Parser Expr
struct pExpr {
    template <typename T>
    struct _pExpr_1 {
        template <typename O>
        struct _pExpr_1_1 {
            template <typename L, typename R>
            struct _pExpr_1_1_1 {
                using Eval = typename Bin<typename fst<R>::Eval, L, typename snd<R>::Eval>::Eval;
            };

            using Eval = typename ret<typename foldl<
                _pExpr_1_1_1,
                T,
                O
            >::Eval>::Eval;
        };

        using Eval = typename bind<
            pOpTerm::Eval,
            _pExpr_1_1
        >::Eval;
    };

    using Eval = typename bind<
        pTerm::Eval,
        _pExpr_1
    >::Eval;
};

// pFactor::pParen::_pParen_1
template <typename _1>
struct pFactor::pParen::_pParen_1 {
    template <typename E>
    struct _pParen_1_1 {
        template <typename _2>
        struct _pParen_1_1_1 {
            using Eval = typename ret<E>::Eval;
        };

        using Eval = typename bind<
            typename symbol<typename Literal<closeParenSymbol>::Eval>::Eval,
            _pParen_1_1_1
        >::Eval;
    };

    using Eval = typename bind<
        pExpr::Eval,
        _pParen_1_1
    >::Eval;
};

// Evaluators

// evalBinop :: Binop -> Int -> Int -> Maybe Int
template <typename B, typename X, typename Y>
struct evalBinop {
    using Eval = struct evalBinop_Error {};
};

template <int X, int Y>
struct evalBinop<Binop<Add>, Int<X>, Int<Y>> {
    using Eval = typename Just<typename add<Int<X>, Int<Y>>::Eval>::Eval;
};

template <int X, int Y>
struct evalBinop<Binop<Sub>, Int<X>, Int<Y>> {
    using Eval = typename Just<typename sub<Int<X>, Int<Y>>::Eval>::Eval;
};

template <int X, int Y>
struct evalBinop<Binop<Mul>, Int<X>, Int<Y>> {
    using Eval = typename Just<typename mul<Int<X>, Int<Y>>::Eval>::Eval;
};

template <int X, int Y>
struct evalBinop<Binop<Div>, Int<X>, Int<Y>> {
    using Eval = typename IfThenElse<
        typename eq<Int<Y>, Int<0>>::Eval,
        typename Nothing::Eval,
        typename Just<typename _div<Int<X>, Int<Y>>::Eval>::Eval
    >::Eval;
};

template <int X, int Y>
struct evalBinop<Binop<Mod>, Int<X>, Int<Y>> {
    using Eval = typename IfThenElse<
        typename eq<Int<Y>, Int<0>>::Eval,
        typename Nothing::Eval,
        typename Just<typename mod<Int<X>, Int<Y>>::Eval>::Eval
    >::Eval;
};

// eval :: Env -> Expr -> Maybe Int
template <typename E, typename ES>
struct eval {
    using Eval = struct eval_Error {};
};

template <typename E, typename ES, typename X>
struct eval<List<Cons<E, ES>>, Expr<Val<X>>> {
    using Eval = typename Just<X>::Eval;
};

template <typename E, typename ES, typename X>
struct eval<List<Cons<E, ES>>, Expr<Var<X>>> {
    using Eval = typename lookup<X, List<Cons<E, ES>>>::Eval;
};

template <typename E, typename ES, typename B, typename L, typename R>
struct eval<List<Cons<E, ES>>, Expr<Bin<B, L, R>>> {
    using Eval = typename evalBinop<
        B,
        typename unwrap<typename eval<
            typename Env<E, List<Cons<E, ES>>>::Eval, L
        >::Eval>::Eval,
        typename unwrap<typename eval<
            typename Env<E, List<Cons<E, ES>>>::Eval, R
        >::Eval>::Eval
    >::Eval;
};

// Inputs
constexpr char inp[] = "pi * r * r / 2 + (a + b) * h / 2";
constexpr char pi[] = "pi";
using InpStr = typename Literal<inp>::Eval;
using InpEnv = typename Env<
    Tuple<typename Literal<pi>::Eval, Int<3>>,
    typename Env<
        Tuple<typename String<Char<'r'>, Nil>::Eval, Int<2>>,
        typename Env<
            Tuple<typename String<Char<'a'>, Nil>::Eval, Int<1>>,
            typename Env<
                Tuple<typename String<Char<'b'>, Nil>::Eval, Int<5>>,
                typename Env<
                    Tuple<typename String<Char<'h'>, Nil>::Eval, Int<4>>,
                    Nil::Eval
                >::Eval
            >::Eval
        >::Eval
    >::Eval
>::Eval;

// Outputs
using OutParse = typename parse<pExpr::Eval, InpStr>::Eval;
using OutEval = typename eval<
    InpEnv,
    typename fst<typename unwrap<OutParse>::Eval>::Eval
>::Eval;

// Main
int main() {
    std::cout << "  InpStr: " << show<InpStr>::to_string() << std::endl;
    std::cout << "  InpEnv: " << show<InpEnv>::to_string() << std::endl;

    std::cout << "OutParse: " << show<OutParse>::to_string() << std::endl;
    std::cout << " OutEval: " << show<OutEval>::to_string() << std::endl;

    return 0;
}