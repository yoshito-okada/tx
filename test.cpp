#include <iostream>

#include <boost/math/quaternion.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/proto/proto_typeof.hpp>

#include "core.hpp"
#include "least_squares.hpp"
#include "math.hpp"
#include "pdiff.hpp"
#include "quaternion.hpp"

class pred {
  typedef boost::numeric::ublas::vector< double > vec_type;
  unsigned int i_, end_;

public:
  pred(unsigned int end) : i_(0), end_(end) {}
  bool operator()(const vec_type &x0, const vec_type &r) {
    bool do_continue(i_ < end_);
    if (do_continue) {
      /*
      std::cout << "  repetition #" << i_ << '\n'
                << "    x0: " << x0 << '\n'
                << "     r: " << r << std::endl;
      */
    }
    ++i_;
    return do_continue;
  }
};

void test_core() {
  std::cout << "[a test on lambda evaluation]" << std::endl;
  const double A(1.), B(2.), C(3.), D(4.), TH(0.5);
  TX_PLACEHOLDER_TYPE(0, 'a') a;
  TX_PLACEHOLDER_TYPE(1, 'b') b;
  TX_PLACEHOLDER_TYPE(2, 'c') c;
  TX_PLACEHOLDER_TYPE(3, 'd') d;
  TX_PLACEHOLDER_TYPE(4, 'th') th;
  BOOST_PROTO_AUTO(expr, tx::sqrt(a + b) * (c - d) - tx::sin(th) * tx::cos(th));
  std::cout << "    lambda operation: " << expr.str() << '=' << expr(A, B, C, D, TH) << std::endl;
  std::cout << "  standard operation: " << std::sqrt(A + B) * (C - D) - std::sin(TH) * std::cos(TH)
            << std::endl;
}

void test_quaternion() {
  std::cout << "[a test on lambda quaternion extraction]" << std::endl;
  const double A(1.), B(2.), C(3.), D(4.);
  boost::math::quaternion< double > X(0.5, 1., 1.5, 2.), Y(A, B, C, D),
      Z(std::sqrt(A * A + B * B + C * C), A, B, C);
  tx::quaternion_type x(0.5, 1., 1.5, 2.);
  tx::quaternion0_type y;
  tx::placeholder4_type z1;
  tx::placeholder5_type z2;
  tx::placeholder6_type z3;
  BOOST_AUTO(z, make_quat(sqrt(z1 * z1 + z2 * z2 + z3 * z3), z1, z2, z3));
  std::cout << "    lambda operation: " << ((x * y - tx::conj(y) + x) * y + x * (y + z))(Y, A, B, C)
            << std::endl;
  std::cout << "  standard operation: " << (X * Y - boost::math::conj(Y) + X) * Y + X * (Y + Z)
            << std::endl;
}

void test_pdiff() {
  std::cout << "[a test on lambda p-differenatiation]" << std::endl;
  {
    const double A(1.), B(2.), C(3.), D(4.);
    TX_PLACEHOLDER_TYPE(0, 'a') a;
    TX_PLACEHOLDER_TYPE(1, 'b') b;
    TX_PLACEHOLDER_TYPE(2, 'c') c;
    TX_PLACEHOLDER_TYPE(3, 'd') d;
    BOOST_PROTO_AUTO(
        pd, tx::pdiff< typeof(a) >(-a / b * d - (a * c * a + d) * (a - c) + sqrt(a * a + b * b)));
    std::cout << "    lambda operation: " << pd.str() << '=' << pd(A, B, C, D) << '\n'
              << "  standard operation: "
              << -1. / B * D - (2 * A * C * (A - C) + (A * A * C + D)) +
                     A / std::sqrt(A * A + B * B)
              << std::endl;
  }
  std::cout << std::endl;
  {
    const double TH(0.5);
    TX_PLACEHOLDER_TYPE(0, 'th') th;
    BOOST_PROTO_AUTO(s, tx::sin(th));
    BOOST_PROTO_AUTO(c, tx::pdiff< typeof(th) >(s));
    BOOST_PROTO_AUTO(ns, tx::pdiff< typeof(th) >(c));
    BOOST_PROTO_AUTO(nc, tx::pdiff< typeof(th) >(ns));
    BOOST_PROTO_AUTO(nns, tx::pdiff< typeof(th) >(nc));
    std::cout << "  " << s.str() << '=' << s(TH) << '\n'
              << "  -> " << c.str() << '=' << c(TH) << '\n'
              << "  -> " << ns.str() << '=' << ns(TH) << '\n'
              << "  -> " << nc.str() << '=' << nc(TH) << '\n'
              << "  -> " << nns.str() << '=' << nns(TH) << std::endl;
  }
}

void test_least_squares() {
  std::cout << "[a test on least squares method with lambda expression]" << std::endl;

  tx::placeholder0_type a;
  tx::placeholder1_type b;
  tx::placeholder2_type c;
  tx::placeholder3_type x;
  tx::placeholder4_type y;

  {
    std::cout << "  fitting to the quadratic function:" << std::endl;

    tx::least_squares< 3, 2 > ls(y - (a * x * x + b * x + c));

    boost::array< double, 3 > x0 = {10, 10, 10};
    boost::array< double, 2 > x1[5] = {{-2, 4}, {-1, 1}, {0, 0}, {1, 1}, {2, 4}};

    std::cout << "    points: " << std::flush;
    for (std::size_t i = 0; i < 5; ++i) {
      std::cout << '(' << x1[i][0] << ',' << x1[i][1] << ") " << std::flush;
    }
    std::cout << std::endl;

    ls.optimize(x0, x1, x1 + 5, pred(30));

    std::cout << "    result: y = " << x0[0] << "*x^2 + " << x0[1] << "*x + " << x0[2] << std::endl;
  }
  std::cout << std::endl;
  {
    std::cout << "  solving a system of equations:" << std::endl;

    tx::least_squares2< 3 > ls;
    BOOST_PROTO_AUTO(eq1, a * a + b + c - 18);
    ls.push(eq1);
    BOOST_PROTO_AUTO(eq2, a - b * b + c + 8);
    ls.push(eq2);
    BOOST_PROTO_AUTO(eq3, a + b - c * c + 18);
    ls.push(eq3);

    std::cout << "    eq1: " << eq1.str() << '\n'
              << "    eq2: " << eq2.str() << '\n'
              << "    eq3: " << eq3.str() << std::endl;

    boost::array< double, 3 > x = {10, 10, 10};

    ls.optimize(x, pred(20));

    std::cout << "    res: " << x[0] << ", " << x[1] << ", " << x[2] << std::endl;
  }
}

int main() {

  test_core();

  std::cout << "\n==================================================\n" << std::endl;

  test_quaternion();

  std::cout << "\n==================================================\n" << std::endl;

  test_pdiff();

  std::cout << "\n==================================================\n" << std::endl;

  test_least_squares();

  return 0;
}
