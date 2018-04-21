#ifndef __TX__UTILITY_HPP__
#define __TX__UTILITY_HPP__

#include <boost/mpl/int.hpp>
#include <boost/type_traits.hpp>

#include <tx/core_fwd.hpp>

namespace tx {
//
// remover for reference, const, volatile
//
template < typename T > struct remove_rcv {
  typedef typename boost::remove_reference< T >::type UnRef;
  typedef typename boost::remove_cv< UnRef >::type type;
};

//
// generator for terminal of compile-time int
//
template < typename Domain, int I > struct int_expr {
  typedef
      typename bp::result_of::make_expr< bp::tag::terminal, Domain, boost::mpl::int_< I > >::type
          type;
};

template < typename Tag, typename Expr, int I > struct is_int_impl : boost::false_type {};

template < typename Expr, int I >
struct is_int_impl< bp::tag::terminal, Expr, I >
    : boost::is_same< typename remove_rcv< typename bp::result_of::value< Expr >::type >::type,
                      boost::mpl::int_< I > > {};

template < typename Expr, int I >
struct is_int : is_int_impl< typename bp::tag_of< Expr >::type, Expr, I > {};
} // namespace tx

#endif /* __TX__UTILITY_HPP__ */
