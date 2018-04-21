#ifndef __TX__QUATERNION_HPP__
#define __TX__QUATERNION_HPP__

#include <boost/math/quaternion.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include "context.hpp"
#include "make_array.hpp"
#include "quaternion_types.hpp"

namespace tx {
template < typename Expr0, typename Expr1, typename Expr2, typename Expr3 > class quaternion {
public:
  typedef boost::math::quaternion< double > result_type;
  typedef Expr0 T0;
  typedef Expr1 T1;
  typedef Expr2 T2;
  typedef Expr3 T3;
  typedef quaternion< T0, T1, T2, T3 > type;
  T0 elm0;
  T1 elm1;
  T2 elm2;
  T3 elm3;

  quaternion(const T0 &a0 = T0(), const T1 &a1 = T1(), const T2 &a2 = T2(), const T3 &a3 = T3())
      : elm0(a0), elm1(a1), elm2(a2), elm3(a3) {}
  template < typename Arg0T, typename Arg1T, typename Arg2T, typename Arg3T >
  quaternion(const Arg0T &a0, const Arg1T &a1, const Arg2T &a2, const Arg3T &a3)
      : elm0(a0), elm1(a1), elm2(a2), elm3(a3) {}

  template < typename Right >
  typename result_of::qadd< type, Right >::type operator+(const Right &r) const {
    return result_of::qadd< type, Right >::call(*this, r);
  }
  template < typename Right >
  typename result_of::qsub< type, Right >::type operator-(const Right &r) const {
    return result_of::qsub< type, Right >::call(*this, r);
  }
  template < typename Right >
  typename result_of::qmul< type, Right >::type operator*(const Right &r) const {
    return result_of::qmul< type, Right >::call(*this, r);
  }
  template < std::size_t N > result_type operator()(const boost::array< double, N > &a) const {
    context< N > ctx(a);
    return result_type(bp::eval(elm0, ctx), bp::eval(elm1, ctx), bp::eval(elm2, ctx),
                       bp::eval(elm3, ctx));
  }

#define TX_QUAT_EVAL_FUNC(z, n, d)                                                                 \
  template < BOOST_PP_ENUM_PARAMS(n, typename A) >                                                 \
  result_type operator()(BOOST_PP_ENUM_BINARY_PARAMS(n, const A, &a)) const {                      \
    boost::array< double, array_size< BOOST_PP_ENUM_PARAMS(n, A) >::size > a;                      \
    make_array(a, BOOST_PP_ENUM_PARAMS(n, a));                                                     \
    return (*this)(a);                                                                             \
  }
  BOOST_PP_REPEAT_FROM_TO(1, TX_MAX_ARITY, TX_QUAT_EVAL_FUNC, d);
};

template < typename Quaternion >
typename result_of::conj< Quaternion >::type conj(const Quaternion &q) {
  return result_of::conj< Quaternion >::call(q);
}

template < typename Expr0, typename Expr1, typename Expr2, typename Expr3 >
typename result_of::qmake< Expr0, Expr1, Expr2, Expr3 >::type
make_quat(const Expr0 &e0, const Expr1 &e1, const Expr2 &e2, const Expr3 &e3) {
  return result_of::qmake< Expr0, Expr1, Expr2, Expr3 >::call(e0, e1, e2, e3);
}
} // namespace tx

#endif /* __TX__QUATERNION_HPP__ */
