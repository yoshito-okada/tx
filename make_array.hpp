#ifndef __TX__MAKE_ARRAY_HPP__
#define __TX__MAKE_ARRAY_HPP__

#include <boost/array.hpp>
#include <boost/math/quaternion.hpp>
#include <boost/mpl/int.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#include "core_fwd.hpp"

namespace tx {
struct null_type {};

template < typename T > struct number_of_elements : public boost::mpl::int_< 1 > {};

template <> struct number_of_elements< null_type > : public boost::mpl::int_< 0 > {};

template <>
template < typename T >
struct number_of_elements< boost::math::quaternion< T > > : public boost::mpl::int_< 4 > {};

#define TX_ARRAY_SIZE_I(z, n, d)                                                                   \
  size##n = BOOST_PP_CAT(size, BOOST_PP_DEC(n)) +                                                  \
            number_of_elements< BOOST_PP_CAT(T, BOOST_PP_DEC(n)) >::value,

template < BOOST_PP_ENUM_BINARY_PARAMS(TX_MAX_ARITY, typename T, = null_type BOOST_PP_INTERCEPT) >
struct array_size {
  enum {
    size0 = 0,
    BOOST_PP_REPEAT_FROM_TO(1, TX_MAX_ARITY, TX_ARRAY_SIZE_I, d) size =
        BOOST_PP_CAT(size, BOOST_PP_DEC(TX_MAX_ARITY)) +
        number_of_elements< BOOST_PP_CAT(T, BOOST_PP_DEC(TX_MAX_ARITY)) >::value
  };
};

template < std::size_t I, typename T > void array_assign(double *a, const T &v) { a[I] = v; }

template < std::size_t I > void array_assign(double *, null_type) {}

template < std::size_t I, typename T >
void array_assign(double *a, const boost::math::quaternion< T > &v) {
  a[I] = v.R_component_1();
  a[I + 1] = v.R_component_2();
  a[I + 2] = v.R_component_3();
  a[I + 3] = v.R_component_4();
}

#define TX_MAKE_ARRAY_I(z, n, d) array_assign< size::size##n >(a.data(), v##n);

#define TX_MAKE_ARRAY(z, n, d)                                                                     \
  template < BOOST_PP_ENUM_PARAMS(n, typename T) >                                                 \
  void make_array(boost::array< double, array_size< BOOST_PP_ENUM_PARAMS(n, T) >::size > &a,       \
                  BOOST_PP_ENUM_BINARY_PARAMS(n, const T, &v)) {                                   \
    typedef array_size< BOOST_PP_ENUM_PARAMS(n, T) > size;                                         \
    BOOST_PP_REPEAT(n, TX_MAKE_ARRAY_I, d);                                                        \
  }

BOOST_PP_REPEAT_FROM_TO(1, TX_MAX_ARITY, TX_MAKE_ARRAY, d);
} // namespace tx

#endif /* __TX__MAKE_ARRAY_HPP__ */
