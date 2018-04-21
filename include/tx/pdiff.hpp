#ifndef __TX__PDIFF_HPP__
#define __TX__PDIFF_HPP__

#include <tx/math.hpp>
#include <tx/simplify.hpp>
#include <tx/utility.hpp>

namespace tx {
namespace result_of {
//
// result type of pdiff function
//
template < typename Tag, typename Expr, typename PHolder > struct pdiff_impl;

template < typename Expr, typename PHolder >
struct pdiff : public pdiff_impl< typename bp::tag_of< Expr >::type, Expr, PHolder > {};

//
// traits
//
template < typename Expr, typename PHolder > struct pdiff_traits_l {
  typedef typename bp::result_of::left< Expr >::type Left;
  typedef pdiff< Left, PHolder > LResult;
  typedef typename LResult::type LPDiff;
};

template < typename Expr, typename PHolder > struct pdiff_traits_r {
  typedef typename bp::result_of::right< Expr >::type Right;
  typedef pdiff< Right, PHolder > RResult;
  typedef typename RResult::type RPDiff;
};

template < typename Expr, typename PHolder >
struct pdiff_traits_lr : public pdiff_traits_l< Expr, PHolder >,
                         public pdiff_traits_r< Expr, PHolder > {};

//
// implementations of pdiff function
//
template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::plus, Expr, PHolder > {
private:
  typedef pdiff_traits_lr< Expr, PHolder > LR;
  typedef typename bp::result_of::make_expr< bp::tag::plus, typename LR::LPDiff,
                                             typename LR::RPDiff >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename PDiff::proto_base_expr ret = {LR::LResult::call(bp::left(expr)),
                                           LR::RResult::call(bp::right(expr))};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::minus, Expr, PHolder > {
private:
  typedef pdiff_traits_lr< Expr, PHolder > LR;
  typedef typename bp::result_of::make_expr< bp::tag::minus, typename LR::LPDiff,
                                             typename LR::RPDiff >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename PDiff::proto_base_expr ret = {LR::LResult::call(bp::left(expr)),
                                           LR::RResult::call(bp::right(expr))};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder >
struct pdiff_impl< bp::tag::multiplies, Expr, PHolder > {
private:
  typedef pdiff_traits_lr< Expr, PHolder > LR;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, typename LR::LPDiff,
                                             typename LR::Right >::type First;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, typename LR::Left,
                                             typename LR::RPDiff >::type Second;
  typedef typename bp::result_of::make_expr< bp::tag::plus, First, Second >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename First::proto_base_expr first = {LR::LResult::call(bp::left(expr)), bp::right(expr)};
    typename Second::proto_base_expr second = {bp::left(expr), LR::RResult::call(bp::right(expr))};
    typename PDiff::proto_base_expr ret = {First(first), Second(second)};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder > struct pdiff_divides {
private:
  typedef pdiff_traits_lr< Expr, PHolder > LR;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, typename LR::LPDiff,
                                             typename LR::Right >::type UFirst;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, typename LR::Left,
                                             typename LR::RPDiff >::type USecond;
  typedef typename bp::result_of::make_expr< bp::tag::minus, UFirst, USecond >::type Upper;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, typename LR::Right,
                                             typename LR::Right >::type Lower;
  typedef typename bp::result_of::make_expr< bp::tag::divides, Upper, Lower >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename UFirst::proto_base_expr uf = {LR::LResult::call(bp::left(expr)), bp::right(expr)};
    typename USecond::proto_base_expr us = {bp::left(expr), LR::RResult::call(bp::right(expr))};
    typename Upper::proto_base_expr upper = {UFirst(uf), USecond(us)};
    typename Lower::proto_base_expr lower = {bp::right(expr), bp::right(expr)};
    typename PDiff::proto_base_expr ret = {Upper(upper), Lower(lower)};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder > struct pdiff_divides_left {
private:
  typedef pdiff_traits_lr< Expr, PHolder > LR;
  typedef typename bp::result_of::make_expr< bp::tag::divides, typename LR::LPDiff,
                                             typename LR::Right >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename PDiff::proto_base_expr ret = {LR::LResult::call(bp::left(expr)), bp::right(expr)};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::divides, Expr, PHolder > {
private:
  typedef pdiff_traits_r< Expr, PHolder > R;
  typedef typename boost::mpl::if_< is_int< typename R::RPDiff, 0 >,
                                    pdiff_divides_left< Expr, PHolder >,
                                    pdiff_divides< Expr, PHolder > >::type Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) { return Result::call(expr); }
};

template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::negate, Expr, PHolder > {
private:
  typedef pdiff_traits_l< Expr, PHolder > L;
  typedef typename bp::result_of::make_expr< bp::tag::negate, typename L::LPDiff >::type PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename PDiff::proto_base_expr ret = {L::LResult::call(bp::left(expr))};
    return Result::call(PDiff(ret));
  }
};

template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::terminal, Expr, PHolder > {
private:
  typedef typename bp::domain_of< Expr >::type Domain;
  typedef typename bp::result_of::value< PHolder >::type PVal;
  typedef typename bp::result_of::value< Expr >::type EVal;

public:
  typedef typename boost::mpl::if_<
      boost::is_same< typename remove_rcv< PVal >::type, typename remove_rcv< EVal >::type >,
      typename int_expr< Domain, 1 >::type, typename int_expr< Domain, 0 >::type >::type type;
  static type call(const Expr &expr) {
    typename type::proto_base_expr ret = {{}};
    return ret;
  }
};

template < typename Expr, typename PHolder > struct pdiff_impl< bp::tag::function, Expr, PHolder > {
private:
  typedef typename bp::result_of::left< Expr >::type Func;
  typedef typename bp::result_of::value< Func >::type FuncImpl;
  typedef typename remove_rcv< FuncImpl >::type UnRCVFuncImpl;
  typedef diff_function_impl< UnRCVFuncImpl, Expr > LResult;
  typedef typename LResult::type LPDiff;
  typedef pdiff_traits_r< Expr, PHolder > R;
  typedef typename bp::result_of::make_expr< bp::tag::multiplies, LPDiff, typename R::RPDiff >::type
      PDiff;
  typedef simplify< PDiff > Result;

public:
  typedef typename Result::type type;
  static type call(const Expr &expr) {
    typename PDiff::proto_base_expr ret = {LResult::call(expr), R::RResult::call(bp::right(expr))};
    return Result::call(PDiff(ret));
  }
};
} // namespace result_of

template < typename PHolder, typename Expr >
typename result_of::pdiff< Expr, PHolder >::type pdiff(const Expr &expr) {
  return result_of::pdiff< Expr, PHolder >::call(expr);
}
} // namespace tx

#endif /* __TX__PDIFF_HPP__ */
