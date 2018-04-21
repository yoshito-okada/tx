#include <iostream>

#include <boost/math/quaternion.hpp>
#include <boost/proto/proto_typeof.hpp>

#include <tx/core.hpp>
#include <tx/math.hpp>
#include <tx/pdiff.hpp>
#include <tx/quaternion.hpp>
#include <tx/simplify.hpp>

TX_PLACEHOLDER_TYPE(0, 'wx') wx;
TX_PLACEHOLDER_TYPE(1, 'wy') wy;
TX_PLACEHOLDER_TYPE(2, 'wz') wz;
TX_PLACEHOLDER_TYPE(3, 'q1') q1;
TX_PLACEHOLDER_TYPE(4, 'q2') q2;
TX_PLACEHOLDER_TYPE(5, 'q3') q3;
TX_PLACEHOLDER_TYPE(6, 'q4') q4;

template < typename Expr > void print_pdiff(const Expr &e) {
  std::cout << "  " << tx::pdiff< typeof(wx) >(e).str() << "  " << tx::pdiff< typeof(wy) >(e).str()
            << "  " << tx::pdiff< typeof(wz) >(e).str() << "  " << tx::pdiff< typeof(q1) >(e).str()
            << "  " << tx::pdiff< typeof(q2) >(e).str() << "  " << tx::pdiff< typeof(q3) >(e).str()
            << "  " << tx::pdiff< typeof(q4) >(e).str() << std::endl;
}

int main(int argc, char *argv[]) {
  TX_INT_TYPE(1) one;
  TX_INT_TYPE(0) zero;
  BOOST_AUTO(z, tx::make_quat(zero, zero, zero, -one));
  BOOST_AUTO(w, tx::make_quat(one, wx, wy, wz));
  BOOST_AUTO(q, tx::make_quat(q1, q2, q3, q4));

  BOOST_AUTO(a, w * q);
  std::cout << "w*q:\n"
            << "  " << tx::simplify(a.elm0).str() << '\n'
            << "  " << tx::simplify(a.elm1).str() << '\n'
            << "  " << tx::simplify(a.elm2).str() << '\n'
            << "  " << tx::simplify(a.elm3).str() << std::endl;
  std::cout << "Jacobian of w*q:" << std::endl;
  print_pdiff(a.elm0);
  print_pdiff(a.elm1);
  print_pdiff(a.elm2);
  print_pdiff(a.elm3);

  BOOST_AUTO(b, q * z * tx::conj(q));
  std::cout << "q*z*q^(-1):\n"
            << "  " << tx::simplify(b.elm1).str() << '\n'
            << "  " << tx::simplify(b.elm2).str() << '\n'
            << "  " << tx::simplify(b.elm3).str() << std::endl;
  std::cout << "Jacobian of q*z*q^(-1):" << std::endl;
  print_pdiff(b.elm1);
  print_pdiff(b.elm2);
  print_pdiff(b.elm3);

  return 0;
}
