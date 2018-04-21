#ifndef __TX__CORE_FWD_HPP__
#define __TX__CORE_FWD_HPP__

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/proto/proto.hpp>

#ifndef TX_MAX_ARITY
#define TX_MAX_ARITY 8
#endif

namespace tx {
namespace bp = boost::proto;

template < unsigned int I, int C = '\0' > struct placeholder;
template < typename Expr > struct tx;
template < typename Expr0, typename Expr1, typename Expr2, typename Expr3 > struct quaternion;

#define TX_PLACEHOLDER_TYPEDEF(z, i, d)                                                            \
  typedef const tx< bp::terminal< placeholder< i > >::type > placeholder##i##_type;

BOOST_PP_REPEAT(TX_MAX_ARITY, TX_PLACEHOLDER_TYPEDEF, d);

typedef quaternion< tx< bp::literal< double > >, tx< bp::literal< double > >,
                    tx< bp::literal< double > >, tx< bp::literal< double > > >
    quaternion_type;

#define TX_QUATERNION_TYPEDEF(z, i, d)                                                             \
  typedef quaternion< tx< bp::terminal< placeholder< i * 4 > >::type >,                            \
                      tx< bp::terminal< placeholder< i * 4 + 1 > >::type >,                        \
                      tx< bp::terminal< placeholder< i * 4 + 2 > >::type >,                        \
                      tx< bp::terminal< placeholder< i * 4 + 3 > >::type > >                       \
      quaternion##i##_type;

BOOST_PP_REPEAT(TX_MAX_ARITY, TX_QUATERNION_TYPEDEF, d);
} // namespace tx

#define TX_PLACEHOLDER_TYPE(i, c) tx::tx< boost::proto::terminal< tx::placeholder< i, c > >::type >
#define TX_INT_TYPE(i) tx::tx< boost::proto::literal< boost::mpl::int_< i > > >
#define TX_REAL_TYPE tx::tx< boost::proto::literal< double > >

#endif /* __TX__CORE_FWD_HPP__ */
