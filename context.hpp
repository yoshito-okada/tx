#ifndef __TX__CONTEXT_HPP__
#define __TX__CONTEXT_HPP__

#include <iostream>
#include <string>
#include <vector>

#include <boost/array.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/string.hpp>

#include "core_fwd.hpp"

namespace tx {
template < std::size_t N > class context : public bp::callable_context< const context< N > > {
  const boost::array< double, N > &args_;

public:
  typedef double result_type;
  context(const boost::array< double, N > &args) : args_(args) {}

  template < unsigned int I, int C >
      typename boost::enable_if_c <
      I< N, double >::type operator()(bp::tag::terminal, placeholder< I, C >) const {
    return args_[I];
  }
  template < int I > double operator()(bp::tag::terminal, boost::mpl::int_< I >) const { return I; }
};

namespace print_context_detail {
enum Precedence {
  NEGATE = 0,
  PLUS = 1,
  MINUS = 1,
  MULTIPLIES = 2,
  DIVIDES = 2,
  FUNCTION = 3,
  TERMINAL = 3,
  FUNCTION1 = 4
};

template < int Op, int Prec0 > struct unary_op_traits {
  typedef boost::mpl::string< Op > op;
  typedef boost::mpl::int_< Prec0 > p0;
};

template < int Op, int Prec0, int Prec1 > struct binary_op_traits {
  typedef boost::mpl::string< Op > op;
  typedef boost::mpl::int_< Prec0 > p0;
  typedef boost::mpl::int_< Prec1 > p1;
};

template < typename Tag > struct op_traits {};
template <> struct op_traits< bp::tag::plus > : binary_op_traits< '+', PLUS, PLUS > {};
template <> struct op_traits< bp::tag::minus > : binary_op_traits< '-', MINUS, MINUS + 1 > {};
template <>
struct op_traits< bp::tag::multiplies > : binary_op_traits< '*', MULTIPLIES, MULTIPLIES > {};
template <> struct op_traits< bp::tag::divides > : binary_op_traits< '/', DIVIDES, DIVIDES + 1 > {};
template <>
struct op_traits< bp::tag::function > : binary_op_traits< '\0', FUNCTION, FUNCTION1 > {};
template <> struct op_traits< bp::tag::negate > : unary_op_traits< '-', NEGATE > {};
template <> struct op_traits< bp::tag::terminal > : unary_op_traits< '\0', TERMINAL > {};
} // namespace print_context_detail

struct print_context : public bp::callable_context< const print_context > {
  typedef std::ostream &result_type;

private:
  result_type os_;

public:
  print_context(result_type os) : os_(os) {}
  template < int I > result_type operator()(bp::tag::terminal, const boost::mpl::int_< I >) const {
    return os_ << I;
  }
  template < typename T > result_type operator()(bp::tag::terminal, const T &t) const {
    return os_ << t;
  }
  template < typename UnaryTag, typename Left >
  result_type operator()(UnaryTag, const Left &l) const {
    using namespace print_context_detail;
    typedef op_traits< UnaryTag > op;

    os_ << boost::mpl::c_str< typename op::op >::value;

    typedef op_traits< typename bp::tag_of< Left >::type > lop;
    typedef typename boost::mpl::less< typename lop::p0, typename op::p0 >::type left_bracket_flag;
    call(left_bracket_flag(), l);

    return os_;
  }
  template < typename BinaryTag, typename Left, typename Right >
  result_type operator()(BinaryTag, const Left &l, const Right &r) const {
    using namespace print_context_detail;
    typedef op_traits< BinaryTag > op;

    typedef op_traits< typename bp::tag_of< Left >::type > lop;
    typedef typename boost::mpl::less< typename lop::p0, typename op::p0 >::type left_bracket_flag;
    call(left_bracket_flag(), l);

    os_ << boost::mpl::c_str< typename op::op >::value;

    typedef op_traits< typename bp::tag_of< Right >::type > rop;
    typedef typename boost::mpl::less< typename rop::p0, typename op::p1 >::type right_bracket_flag;
    call(right_bracket_flag(), r);

    return os_;
  }

private:
  template < typename Expr > void call(boost::mpl::true_, const Expr &e) const {
    os_ << '(';
    bp::eval(e, *this);
    os_ << ')';
  }
  template < typename Expr > void call(boost::mpl::false_, const Expr &e) const {
    bp::eval(e, *this);
  }
};
} // namespace tx

#endif /* __TX__CONTEXT_HPP__ */
