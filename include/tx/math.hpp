#ifndef __TX__MATH_HPP__
#define __TX__MATH_HPP__

#include <cmath>
#include <string>

#include <boost/type_traits.hpp>

#include <tx/core.hpp>

#define TX_UNARY_FUNCTION_IMPL_DEFINITION(impl, func, name)                                        \
  struct impl {                                                                                    \
    typedef double result_type;                                                                    \
    result_type operator()(const double v) const { return (func)(v); }                             \
    friend std::ostream &operator<<(std::ostream &ost, const impl &) { return ost << (name); }     \
  }

#define TX_RESULT_OF_UNARY_FUNCTION_DEFINITION(result, impl)                                       \
  template < typename Expr > struct result {                                                       \
  private:                                                                                         \
    typedef typename bp::domain_of< Expr >::type Domain;                                           \
    typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, impl >::type TermImpl;   \
                                                                                                   \
  public:                                                                                          \
    typedef                                                                                        \
        typename bp::result_of::make_expr< bp::tag::function, TermImpl, const Expr & >::type type; \
    static type call(const Expr &expr) {                                                           \
      typename TermImpl::proto_base_expr implbase = {{}};                                          \
      typename type::proto_base_expr ret = {TermImpl(implbase), expr};                             \
      return ret;                                                                                  \
    }                                                                                              \
  }

#define TX_UNARY_FUNCTION_DEFINITION(func, result)                                                 \
  template < typename Expr > typename result< Expr >::type func(const Expr &expr) {                \
    return result< Expr >::call(expr);                                                             \
  }

namespace tx {
// impl
TX_UNARY_FUNCTION_IMPL_DEFINITION(sqrt_impl, std::sqrt, "sqrt");
TX_UNARY_FUNCTION_IMPL_DEFINITION(sin_impl, std::sin, "sin");
TX_UNARY_FUNCTION_IMPL_DEFINITION(cos_impl, std::cos, "cos");
TX_UNARY_FUNCTION_IMPL_DEFINITION(tan_impl, std::tan, "tan");

namespace result_of {
TX_RESULT_OF_UNARY_FUNCTION_DEFINITION(sqrt, sqrt_impl);
TX_RESULT_OF_UNARY_FUNCTION_DEFINITION(sin, sin_impl);
TX_RESULT_OF_UNARY_FUNCTION_DEFINITION(cos, cos_impl);
TX_RESULT_OF_UNARY_FUNCTION_DEFINITION(tan, tan_impl);

template < typename Impl, typename Expr > struct diff_function_impl;

template < typename Expr > struct diff_function_impl< sqrt_impl, Expr > {
private:
  typedef typename bp::domain_of< Expr >::type Domain;
  typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, double >::type DLit;
  typedef DLit Upper;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, DLit, Expr >::type Lower;

public:
  typedef typename bp::result_of::make_expr< bp::tag::divides, Upper, Lower >::type type;
  static type call(const Expr &expr) {
    typename Upper::proto_base_expr upper = {1.};
    typename DLit::proto_base_expr llower = {2.};
    typename Lower::proto_base_expr lower = {DLit(llower), expr};
    typename type::proto_base_expr ret = {Upper(upper), Lower(lower)};
    return ret;
  }
};

template < typename Expr > struct diff_function_impl< sin_impl, Expr > {
private:
  typedef typename bp::domain_of< Expr >::type Domain;
  typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, cos_impl >::type CosFunc;
  typedef typename bp::result_of::right< Expr >::type Arg;

public:
  typedef typename bp::result_of::make_expr< bp::tag::function, CosFunc, Arg >::type type;
  static type call(const Expr &expr) {
    typename CosFunc::proto_base_expr cfunc = {{}};
    typename type::proto_base_expr ret = {CosFunc(cfunc), bp::right(expr)};
    return ret;
  }
};

template < typename Expr > struct diff_function_impl< cos_impl, Expr > {
private:
  typedef typename bp::domain_of< Expr >::type Domain;
  typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, sin_impl >::type SinFunc;
  typedef typename bp::result_of::right< Expr >::type Arg;
  typedef typename bp::result_of::make_expr< bp::tag::function, SinFunc, Arg >::type SinExpr;

public:
  typedef typename bp::result_of::make_expr< bp::tag::negate, SinExpr >::type type;
  static type call(const Expr &expr) {
    typename SinFunc::proto_base_expr sfunc = {{}};
    typename SinExpr::proto_base_expr sexpr = {SinFunc(sfunc), bp::right(expr)};
    typename type::proto_base_expr ret = {SinExpr(sexpr)};
    return ret;
  }
};

template < typename Expr > struct diff_function_impl< tan_impl, Expr > {
private:
  typedef typename bp::domain_of< Expr >::type Domain;
  typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, double >::type Upper;
  typedef typename bp::result_of::make_expr< bp::tag::terminal, Domain, cos_impl >::type CosFunc;
  typedef typename bp::result_of::right< Expr >::type Arg;
  typedef typename bp::result_of::make_expr< bp::tag::function, CosFunc, Arg >::type CosExpr;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, CosExpr, CosExpr >::type Lower;

public:
  typedef typename bp::result_of::make_expr< bp::tag::divides, Upper, Lower >::type type;
  static type call(const Expr &expr) {
    typename Upper::proto_base_expr upper = {1.};
    typename CosFunc::proto_base_expr cfunc = {{}};
    typename CosExpr::proto_base_expr cexpr = {CosFunc(cfunc), bp::right(expr)};
    typename Lower::proto_base_expr lower = {cexpr, cexpr};
    typename type::proto_base_expr ret = {Upper(upper), Lower(lower)};
    return ret;
  }
};
} // namespace result_of

// func
TX_UNARY_FUNCTION_DEFINITION(sqrt, result_of::sqrt);
TX_UNARY_FUNCTION_DEFINITION(sin, result_of::sin);
TX_UNARY_FUNCTION_DEFINITION(cos, result_of::cos);
TX_UNARY_FUNCTION_DEFINITION(tan, result_of::tan);
} // namespace tx

#endif /* __TX__MATH_HPP__ */
