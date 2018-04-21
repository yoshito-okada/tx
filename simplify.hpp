#ifndef __TX__SIMPLIFY_HPP__
#define __TX__SIMPLIFY_HPP__

#include "core.hpp"
#include "utility.hpp"

namespace tx {
namespace result_of {
//
// result type of simplify function
//
template < typename Tag, typename Expr > struct simplify_impl;

template < typename Expr >
struct simplify : public simplify_impl< typename bp::tag_of< Expr >::type, Expr > {};

//
// traits
//
template < typename Expr > struct simplify_traits_l {
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef simplify< Left > LResult;
  typedef typename LResult::type LSim;
};

template < typename Expr > struct simplify_traits_r {
  typedef typename bp::result_of::right< Expr >::type Right;
  typedef simplify< Right > RResult;
  typedef typename RResult::type RSim;
};

template < typename Expr >
struct simplify_traits_lr : public simplify_traits_l< Expr >, public simplify_traits_r< Expr > {};

//
// implementations of simplify function
//
template < typename Expr, int I > struct simplify_int {
private:
  typedef typename bp::domain_of< Expr >::type Domain;

public:
  typedef typename int_expr< Domain, I >::type type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {{}};
    return ret;
  }
};

template < typename Expr > struct simplify_binary {
private:
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef simplify< Left > LResult;
  typedef typename bp::result_of::right< Expr >::type Right;
  typedef simplify< Right > RResult;
  typedef typename bp::tag_of< Expr >::type Tag;

public:
  typedef
      typename bp::result_of::make_expr< Tag, typename LResult::type, typename RResult::type >::type
          type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {LResult::call(bp::left(expr)),
                                          RResult::call(bp::right(expr))};
    return ret;
  }
};

template < typename Expr > struct simplify_unary {
private:
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef simplify< Left > LResult;

public:
  typedef typename bp::result_of::make_expr< typename bp::tag_of< Expr >::type,
                                             typename LResult::type >::type type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {LResult::call(bp::left(expr))};
    return ret;
  }
};

template < typename Expr > struct simplify_left {
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef simplify< Left > LResult;

public:
  typedef typename LResult::type type;
  static type call(const Expr &expr) { return LResult::call(bp::left(expr)); }
};

template < typename Expr > struct simplify_right {
private:
  typedef typename bp::result_of::right< Expr >::type Right;
  typedef simplify< Right > RResult;

public:
  typedef typename RResult::type type;
  static type call(const Expr &expr) { return RResult::call(bp::right(expr)); }
};

template < typename Expr > struct simplify_left_left {
private:
  typedef simplify_left< Expr > LResult;
  typedef simplify_left< typename LResult::type > LLResult;

public:
  typedef typename LLResult::type type;
  static type call(const Expr &expr) { return LLResult::call(LResult::call(expr)); }
};

template < typename Expr > struct simplify_neg_right {
private:
  typedef simplify_right< Expr > RResult;

public:
  typedef typename bp::result_of::make_expr< bp::tag::negate, typename RResult::type >::type type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {RResult::call(expr)};
    return ret;
  }
};

//
// selector of implementations of simplify function
//
template < typename Expr > struct simplify_impl< bp::tag::plus, Expr > {
private:
  typedef simplify_traits_lr< Expr > LR;
  typedef typename boost::mpl::if_<
      is_int< typename LR::LSim, 0 >, simplify_right< Expr >,
      typename boost::mpl::if_< is_int< typename LR::RSim, 0 >, simplify_left< Expr >,
                                simplify_binary< Expr > >::type >::type Sim;

public:
  typedef typename Sim::type type;
  static type call(const Expr &expr) { return Sim::call(expr); }
};

template < typename Expr > struct simplify_impl< bp::tag::minus, Expr > {
private:
  typedef simplify_traits_lr< Expr > LR;
  typedef typename boost::mpl::if_<
      is_int< typename LR::RSim, 0 >, simplify_left< Expr >,
      typename boost::mpl::if_< is_int< typename LR::LSim, 0 >, simplify_neg_right< Expr >,
                                simplify_binary< Expr > >::type >::type Sim;

public:
  typedef typename Sim::type type;
  static type call(const Expr &expr) { return Sim::call(expr); }
};

template < typename Expr > struct simplify_impl< bp::tag::multiplies, Expr > {
private:
  typedef simplify_traits_lr< Expr > LR;
  typedef typename boost::mpl::if_<
      is_int< typename LR::LSim, 0 >, simplify_int< Expr, 0 >,
      typename boost::mpl::if_<
          is_int< typename LR::RSim, 0 >, simplify_int< Expr, 0 >,
          typename boost::mpl::if_<
              is_int< typename LR::LSim, 1 >, simplify_right< Expr >,
              typename boost::mpl::if_< is_int< typename LR::RSim, 1 >, simplify_left< Expr >,
                                        simplify_binary< Expr > >::type >::type >::type >::type Sim;

public:
  typedef typename Sim::type type;
  static type call(const Expr &expr) { return Sim::call(expr); }
};

template < typename Expr > struct simplify_impl< bp::tag::divides, Expr > {
private:
  typedef simplify_traits_lr< Expr > LR;
  typedef typename boost::mpl::if_<
      is_int< typename LR::LSim, 0 >, simplify_int< Expr, 0 >,
      typename boost::mpl::if_< is_int< typename LR::RSim, 1 >, simplify_left< Expr >,
                                simplify_binary< Expr > >::type >::type Sim;

public:
  typedef typename Sim::type type;
  static type call(const Expr &expr) { return Sim::call(expr); }
};

template < typename Expr > struct simplify_impl< bp::tag::negate, Expr > {
private:
  typedef simplify_traits_l< Expr > L;
  typedef typename bp::tag_of< typename L::LSim >::type LTag;
  typedef boost::is_same< bp::tag::negate, LTag > LTIsNeg;
  typedef typename boost::mpl::if_< is_int< typename L::LSim, 0 >, simplify_int< Expr, 0 >,
                                    typename boost::mpl::if_< LTIsNeg, simplify_left_left< Expr >,
                                                              simplify_unary< Expr > >::type >::type
      Sim;

public:
  typedef typename Sim::type type;
  static type call(const Expr &expr) { return Sim::call(expr); }
};

template < typename Expr > struct simplify_impl< bp::tag::terminal, Expr > {
public:
  typedef Expr type;
  static type call(const Expr &expr) { return expr; }
};

template < typename Expr > struct simplify_impl< bp::tag::function, Expr > {
private:
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef simplify_right< Expr > RResult;

public:
  typedef typename bp::result_of::make_expr< bp::tag::function, Left, typename RResult::type >::type
      type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {bp::left(expr), RResult::call(expr)};
    return ret;
  }
};
} // namespace result_of

template < typename Expr > typename result_of::simplify< Expr >::type simplify(const Expr &expr) {
  return result_of::simplify< Expr >::call(expr);
}
} // namespace tx

#endif /* __TX__SIMPLIFY_HPP__ */
