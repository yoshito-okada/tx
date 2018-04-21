#ifndef __TX__QUATERNION_TYPES_HPP__
#define __TX__QUATERNION_TYPES_HPP__

#include <tx/core_fwd.hpp>

#define TX_ADDT(a, b) bp::result_of::make_expr< bp::tag::plus, a, b >::type
#define TX_SUBT(a, b) bp::result_of::make_expr< bp::tag::minus, a, b >::type
#define TX_MULT(a, b) bp::result_of::make_expr< bp::tag::multiplies, a, b >::type
#define TX_NEGT(a) bp::result_of::make_expr< bp::tag::negate, a >::type

namespace tx {
namespace result_of {
template < typename Left, typename Right > struct qadd {
  typedef typename TX_ADDT(typename Left::T0, typename Right::T0) T0;
  typedef typename TX_ADDT(typename Left::T1, typename Right::T1) T1;
  typedef typename TX_ADDT(typename Left::T2, typename Right::T2) T2;
  typedef typename TX_ADDT(typename Left::T3, typename Right::T3) T3;
  typedef quaternion< T0, T1, T2, T3 > type;
  static type call(const Left &l, const Right &r) {
    typename T0::proto_base_expr t0 = {l.elm0, r.elm0};
    typename T1::proto_base_expr t1 = {l.elm1, r.elm1};
    typename T2::proto_base_expr t2 = {l.elm2, r.elm2};
    typename T3::proto_base_expr t3 = {l.elm3, r.elm3};
    return type(T0(t0), T1(t1), T2(t2), T3(t3));
  }
};

template < typename Left, typename Right > struct qsub {
  typedef typename TX_SUBT(typename Left::T0, typename Right::T0) T0;
  typedef typename TX_SUBT(typename Left::T1, typename Right::T1) T1;
  typedef typename TX_SUBT(typename Left::T2, typename Right::T2) T2;
  typedef typename TX_SUBT(typename Left::T3, typename Right::T3) T3;
  typedef quaternion< T0, T1, T2, T3 > type;
  static type call(const Left &l, const Right &r) {
    typename T0::proto_base_expr t0 = {l.elm0, r.elm0};
    typename T1::proto_base_expr t1 = {l.elm1, r.elm1};
    typename T2::proto_base_expr t2 = {l.elm2, r.elm2};
    typename T3::proto_base_expr t3 = {l.elm3, r.elm3};
    return type(T0(t0), T1(t1), T2(t2), T3(t3));
  }
};

template < typename Left, typename Right > struct qmul {
private:
  typedef typename TX_MULT(typename Left::T0, typename Right::T0) T0_TERM0;
  typedef typename TX_MULT(typename Left::T1, typename Right::T1) T0_TERM1;
  typedef typename TX_MULT(typename Left::T2, typename Right::T2) T0_TERM2;
  typedef typename TX_MULT(typename Left::T3, typename Right::T3) T0_TERM3;
  typedef typename TX_SUBT(T0_TERM0, T0_TERM1) T0_TERM01;
  typedef typename TX_SUBT(T0_TERM01, T0_TERM2) T0_TERM012;

  typedef typename TX_MULT(typename Left::T0, typename Right::T1) T1_TERM0;
  typedef typename TX_MULT(typename Left::T1, typename Right::T0) T1_TERM1;
  typedef typename TX_MULT(typename Left::T2, typename Right::T3) T1_TERM2;
  typedef typename TX_MULT(typename Left::T3, typename Right::T2) T1_TERM3;
  typedef typename TX_ADDT(T1_TERM0, T1_TERM1) T1_TERM01;
  typedef typename TX_ADDT(T1_TERM01, T1_TERM2) T1_TERM012;

  typedef typename TX_MULT(typename Left::T0, typename Right::T2) T2_TERM0;
  typedef typename TX_MULT(typename Left::T1, typename Right::T3) T2_TERM1;
  typedef typename TX_MULT(typename Left::T2, typename Right::T0) T2_TERM2;
  typedef typename TX_MULT(typename Left::T3, typename Right::T1) T2_TERM3;
  typedef typename TX_SUBT(T2_TERM0, T2_TERM1) T2_TERM01;
  typedef typename TX_ADDT(T2_TERM01, T2_TERM2) T2_TERM012;

  typedef typename TX_MULT(typename Left::T0, typename Right::T3) T3_TERM0;
  typedef typename TX_MULT(typename Left::T1, typename Right::T2) T3_TERM1;
  typedef typename TX_MULT(typename Left::T2, typename Right::T1) T3_TERM2;
  typedef typename TX_MULT(typename Left::T3, typename Right::T0) T3_TERM3;
  typedef typename TX_ADDT(T3_TERM0, T3_TERM1) T3_TERM01;
  typedef typename TX_SUBT(T3_TERM01, T3_TERM2) T3_TERM012;

public:
  typedef typename TX_SUBT(T0_TERM012, T0_TERM3) T0;
  typedef typename TX_SUBT(T1_TERM012, T1_TERM3) T1;
  typedef typename TX_ADDT(T2_TERM012, T2_TERM3) T2;
  typedef typename TX_ADDT(T3_TERM012, T3_TERM3) T3;

private:
  static T0 elm0(const Left &l, const Right &r) {
    typename T0_TERM0::proto_base_expr term0 = {l.elm0, r.elm0};
    typename T0_TERM1::proto_base_expr term1 = {l.elm1, r.elm1};
    typename T0_TERM2::proto_base_expr term2 = {l.elm2, r.elm2};
    typename T0_TERM3::proto_base_expr term3 = {l.elm3, r.elm3};
    typename T0_TERM01::proto_base_expr term01 = {T0_TERM0(term0), T0_TERM1(term1)};
    typename T0_TERM012::proto_base_expr term012 = {T0_TERM01(term01), T0_TERM2(term2)};
    typename T0::proto_base_expr ret = {T0_TERM012(term012), T0_TERM3(term3)};
    return ret;
  }
  static T1 elm1(const Left &l, const Right &r) {
    typename T1_TERM0::proto_base_expr term0 = {l.elm0, r.elm1};
    typename T1_TERM1::proto_base_expr term1 = {l.elm1, r.elm0};
    typename T1_TERM2::proto_base_expr term2 = {l.elm2, r.elm3};
    typename T1_TERM3::proto_base_expr term3 = {l.elm3, r.elm2};
    typename T1_TERM01::proto_base_expr term01 = {T1_TERM0(term0), T1_TERM1(term1)};
    typename T1_TERM012::proto_base_expr term012 = {T1_TERM01(term01), T1_TERM2(term2)};
    typename T1::proto_base_expr ret = {T1_TERM012(term012), T1_TERM3(term3)};
    return ret;
  }
  static T2 elm2(const Left &l, const Right &r) {
    typename T2_TERM0::proto_base_expr term0 = {l.elm0, r.elm2};
    typename T2_TERM1::proto_base_expr term1 = {l.elm1, r.elm3};
    typename T2_TERM2::proto_base_expr term2 = {l.elm2, r.elm0};
    typename T2_TERM3::proto_base_expr term3 = {l.elm3, r.elm1};
    typename T2_TERM01::proto_base_expr term01 = {T2_TERM0(term0), T2_TERM1(term1)};
    typename T2_TERM012::proto_base_expr term012 = {T2_TERM01(term01), T2_TERM2(term2)};
    typename T2::proto_base_expr ret = {T2_TERM012(term012), T2_TERM3(term3)};
    return ret;
  }
  static T3 elm3(const Left &l, const Right &r) {
    typename T3_TERM0::proto_base_expr term0 = {l.elm0, r.elm3};
    typename T3_TERM1::proto_base_expr term1 = {l.elm1, r.elm2};
    typename T3_TERM2::proto_base_expr term2 = {l.elm2, r.elm1};
    typename T3_TERM3::proto_base_expr term3 = {l.elm3, r.elm0};
    typename T3_TERM01::proto_base_expr term01 = {T3_TERM0(term0), T3_TERM1(term1)};
    typename T3_TERM012::proto_base_expr term012 = {T3_TERM01(term01), T3_TERM2(term2)};
    typename T3::proto_base_expr ret = {T3_TERM012(term012), T3_TERM3(term3)};
    return ret;
  }

public:
  typedef quaternion< T0, T1, T2, T3 > type;
  static type call(const Left &l, const Right &r) {
    return type(elm0(l, r), elm1(l, r), elm2(l, r), elm3(l, r));
  }
};

template < typename Type > struct conj {
  typedef typename Type::T0 T0;
  typedef typename TX_NEGT(typename Type::T1) T1;
  typedef typename TX_NEGT(typename Type::T2) T2;
  typedef typename TX_NEGT(typename Type::T3) T3;
  typedef quaternion< T0, T1, T2, T3 > type;
  static type call(const Type &q) {
    typename T1::proto_base_expr t1 = {q.elm1};
    typename T2::proto_base_expr t2 = {q.elm2};
    typename T3::proto_base_expr t3 = {q.elm3};
    return type(T0(q.elm0), T1(t1), T2(t2), T3(t3));
  }
};

template < typename Expr0, typename Expr1, typename Expr2, typename Expr3 > struct qmake {
  typedef typename bp::result_of::deep_copy< Expr0 >::type T0;
  typedef typename bp::result_of::deep_copy< Expr1 >::type T1;
  typedef typename bp::result_of::deep_copy< Expr2 >::type T2;
  typedef typename bp::result_of::deep_copy< Expr3 >::type T3;
  typedef quaternion< T0, T1, T2, T3 > type;
  static type call(const Expr0 &expr0, const Expr1 &expr1, const Expr2 &expr2, const Expr3 &expr3) {
    return type(bp::deep_copy(expr0), bp::deep_copy(expr1), bp::deep_copy(expr2),
                bp::deep_copy(expr3));
  }
};
} // namespace result_of
} // namespace tx

#endif /* __TX__QUATERNION_TYPES_HPP__ */
