#ifndef __TX__CORE_HPP__
#define __TX__CORE_HPP__

#include <sstream>

#include <boost/mpl/string.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include <tx/context.hpp>
#include <tx/core_fwd.hpp>
#include <tx/make_array.hpp>

namespace tx {
template < unsigned int I, int C > struct placeholder {
  friend std::ostream &operator<<(std::ostream &ost, const placeholder< I, C > &) {
    return ost << boost::mpl::c_str< boost::mpl::string< C > >::value;
  }
};

template < unsigned int I > struct placeholder< I, '\0' > {
  friend std::ostream &operator<<(std::ostream &ost, const placeholder< I, '\0' > &) {
    return ost << '<' << I << '>';
  }
};

struct domain : public bp::domain< bp::generator< tx > > {};

template < typename Expr > struct tx : public bp::extends< Expr, tx< Expr >, domain > {
  typedef double result_type;
  typedef bp::extends< Expr, tx< Expr >, domain > base_type;

  tx(const Expr &e = Expr()) : base_type(e) {}
  template < std::size_t N > result_type operator()(const boost::array< double, N > &a) const {
    return bp::eval(*this, context< N >(a));
  }

#define TX_EXPR_EVAL_FUNC(z, n, d)                                                                 \
  template < BOOST_PP_ENUM_PARAMS(n, typename A) >                                                 \
  double operator()(BOOST_PP_ENUM_BINARY_PARAMS(n, const A, &a)) const {                           \
    boost::array< double, array_size< BOOST_PP_ENUM_PARAMS(n, A) >::size > a;                      \
    make_array(a, BOOST_PP_ENUM_PARAMS(n, a));                                                     \
    return (*this)(a);                                                                             \
  }
  BOOST_PP_REPEAT_FROM_TO(1, TX_MAX_ARITY, TX_EXPR_EVAL_FUNC, d);

  std::string str() const {
    std::ostringstream oss;
    const print_context pcontext(oss);
    bp::eval(*this, pcontext);
    return oss.str();
  }
};
} // namespace tx

#endif /* __TX__CORE_HPP__ */
