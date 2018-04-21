#ifndef __TX__LEAST_SQUARES_HPP__
#define __TX__LEAST_SQUARES_HPP__

#include <algorithm>
#include <vector>

#include <boost/function.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/utility/enable_if.hpp>

#include "pdiff.hpp"

namespace tx {
namespace least_squares_detail {
template < std::size_t X0_SIZE, std::size_t X1_SIZE > struct derivatives {
  typedef boost::array< double, X0_SIZE + X1_SIZE > x_type;
  typedef boost::function< double(const x_type &) > C0_type;
  typedef boost::numeric::ublas::vector< C0_type > C1_type;
  typedef boost::numeric::ublas::symmetric_matrix< C0_type > C2_type;
  C0_type C0;
  C1_type C1;
  C2_type C2;

  derivatives() {}
  template < typename Expr > derivatives(const Expr &expr) { assign(expr); }
  template < typename Expr > void assign(const Expr &expr) {
    C0 = bp::deep_copy(expr);
    C1.resize(X0_SIZE);
    C2.resize(X0_SIZE, X0_SIZE);
    assign_C1_C2< 0 >(expr);
  }

private:
  template < std::size_t I, typename Expr >
  typename boost::enable_if_c< I != X0_SIZE - 1 >::type assign_C1_C2(const Expr &expr) {
    typedef tx< typename bp::terminal< placeholder< I > >::type > ph_type;
    C1(I) = bp::deep_copy(pdiff< ph_type >(expr));
    assign_C2< I, I >(expr);
    assign_C1_C2< I + 1 >(expr);
  }
  template < std::size_t I, typename Expr >
  typename boost::enable_if_c< I == X0_SIZE - 1 >::type assign_C1_C2(const Expr &expr) {
    typedef tx< typename bp::terminal< placeholder< I > >::type > ph_type;
    C1(I) = bp::deep_copy(pdiff< ph_type >(expr));
    assign_C2< I, I >(expr);
  }
  template < std::size_t I, std::size_t J, typename Expr >
  typename boost::enable_if_c< J != X0_SIZE - 1 >::type assign_C2(const Expr &expr) {
    typedef tx< typename bp::terminal< placeholder< I > >::type > ph1_type;
    typedef tx< typename bp::terminal< placeholder< J > >::type > ph2_type;
    C2(I, J) = bp::deep_copy(pdiff< ph1_type >(pdiff< ph2_type >(expr)));
    assign_C2< I, J + 1 >(expr);
  }
  template < std::size_t I, std::size_t J, typename Expr >
  typename boost::enable_if_c< J == X0_SIZE - 1 >::type assign_C2(const Expr &expr) {
    typedef tx< typename bp::terminal< placeholder< I > >::type > ph1_type;
    typedef tx< typename bp::terminal< placeholder< J > >::type > ph2_type;
    C2(I, J) = bp::deep_copy(pdiff< ph1_type >(pdiff< ph2_type >(expr)));
  }
};
} // namespace least_squares_detail

template < std::size_t X0_SIZE, std::size_t X1_SIZE > class least_squares {
public:
  typedef boost::array< double, X0_SIZE > x0_type;
  typedef boost::array< double, X1_SIZE > x1_type;
  typedef boost::numeric::ublas::vector< double > vec_type;

private:
  typedef boost::numeric::ublas::matrix< double > mat_type;
  typedef least_squares_detail::derivatives< X0_SIZE, X1_SIZE > d_type;
  typedef typename d_type::x_type x_type;
  d_type d_;

public:
  template < typename Expr > least_squares(const Expr &expr) : d_(expr) {}
  template < class Pred, typename It >
  void optimize(x0_type &x0_array, It x1b, It x1e, Pred pred, double lambda = 0.001,
                const double scale = 10.) const {
    namespace bnu = boost::numeric::ublas;
    vec_type x0(X0_SIZE);
    std::copy(x0_array.begin(), x0_array.end(), x0.begin());
    const std::size_t n_x(std::distance(x1b, x1e));
    vec_type r(n_x); // residual
    double sq_error(eval_r(&r, x0, x1b, x1e));
    mat_type J(n_x, X0_SIZE); // Jacobian

    while (pred(x0, r)) {
      eval_J(&J, x0, x1b, x1e);
      const vec_type g(bnu::prod(bnu::trans(J), r)); // gradient
      mat_type H(X0_SIZE, X0_SIZE);                  // Hessian
      eval_H(&H, r, J, x0, x1b, x1e);
      const bnu::banded_adaptor< mat_type > D(H); // diagonal of H

      while (true) {
        const mat_type H2(H + lambda * D);
        const vec_type dx0(bnu::solve(H2, g, bnu::lower_tag()));
        x0 -= dx0;

        vec_type r2(n_x);
        const double sq_error2(eval_r(&r2, x0, x1b, x1e));

        if (sq_error2 <= sq_error) {
          r.data().swap(r2.data());
          sq_error = sq_error2;
          lambda /= scale;
          break;
        } else {
          x0 += dx0;
          lambda *= scale;
          continue;
        }
      }
    }

    std::copy(x0.begin(), x0.end(), x0_array.begin());
  }

private:
  template < typename It > double eval_r(vec_type *r, const vec_type &x0, It x1b, It x1e) const {
    x_type x;
    std::copy(x0.begin(), x0.end(), x.begin());
    std::size_t i(0);
    for (It x1 = x1b; x1 != x1e; ++x1, ++i) {
      std::copy(x1->begin(), x1->end(), x.begin() + X0_SIZE);
      (*r)(i) = d_.C0(x);
    }
    return boost::numeric::ublas::inner_prod(*r, *r);
  }
  template < typename It > void eval_J(mat_type *J, const vec_type &x0, It x1b, It x1e) const {
    x_type x;
    std::copy(x0.begin(), x0.end(), x.begin());
    std::size_t i(0);
    for (It x1 = x1b; x1 != x1e; ++x1, ++i) {
      std::copy(x1->begin(), x1->end(), x.begin() + X0_SIZE);
      for (std::size_t j = 0; j < X0_SIZE; ++j) {
        (*J)(i, j) = d_.C1(j)(x);
      }
    }
  }
  template < typename It >
  void eval_H(mat_type *H, const vec_type &r, const mat_type &J, const vec_type &x0, It x1b,
              It x1e) const {
    namespace bnu = boost::numeric::ublas;
    *H = bnu::prod(bnu::trans(J), J);
    x_type x;
    std::copy(x0.begin(), x0.end(), x.begin());
    std::size_t i(0);
    for (It x1 = x1b; x1 != x1e; ++x1, ++i) {
      std::copy(x1->begin(), x1->end(), x.begin() + X0_SIZE);
      bnu::symmetric_matrix< double > tmp(X0_SIZE, X0_SIZE);
      for (std::size_t j = 0; j < X0_SIZE; ++j) {
        for (std::size_t k = j; k < X0_SIZE; ++k) {
          tmp(j, k) = r(i) * d_.C2(j, k)(x);
        }
      }
      *H += tmp;
    }
  }
};

template < std::size_t X0_SIZE, std::size_t X1_SIZE = 0 > class least_squares2 {
public:
  typedef boost::numeric::ublas::vector< double > vec_type;

private:
  typedef boost::numeric::ublas::matrix< double > mat_type;
  typedef least_squares_detail::derivatives< X0_SIZE, X1_SIZE > d_type;

public:
  typedef typename d_type::x_type x_type;

private:
  std::vector< d_type > d_;

public:
  least_squares2() {}
  template < typename Expr > least_squares2(const Expr &expr) { push(expr); }
  template < typename Expr > void push(const Expr &expr) {
    d_.push_back(d_type());
    d_.back().assign(expr);
  }
  template < class Pred >
  void optimize(x_type &x, Pred pred, double lambda = 0.001, const double scale = 10.) const {
    namespace bnu = boost::numeric::ublas;
    vec_type x0(X0_SIZE);
    std::copy(x.begin(), x.begin() + X0_SIZE, x0.begin());
    vec_type r(d_.size()); // residual
    double sq_error(eval_r(&r, x));
    mat_type J(d_.size(), X0_SIZE); // Jacobian

    while (pred(x0, r)) {
      eval_J(&J, x);
      const vec_type g(bnu::prod(bnu::trans(J), r)); // gradient
      mat_type H(X0_SIZE, X0_SIZE);                  // Hessian
      eval_H(&H, r, J, x);
      const bnu::banded_adaptor< mat_type > D(H); // diagonal of H

      while (true) {
        const mat_type H2(H + lambda * D);
        const vec_type dx0(bnu::solve(H2, g, bnu::lower_tag()));
        x0 -= dx0;
        std::copy(x0.begin(), x0.end(), x.begin());

        vec_type r2(d_.size());
        const double sq_error2(eval_r(&r2, x));

        if (sq_error2 <= sq_error) {
          r.data().swap(r2.data());
          sq_error = sq_error2;
          lambda /= scale;
          break;
        } else {
          x0 += dx0;
          std::copy(x0.begin(), x0.end(), x.begin());
          lambda *= scale;
          continue;
        }
      }
    }
  }

private:
  double eval_r(vec_type *r, const x_type &x) const {
    for (std::size_t i = 0; i < d_.size(); ++i) {
      (*r)(i) = d_[i].C0(x);
    }
    return boost::numeric::ublas::inner_prod(*r, *r);
  }
  void eval_J(mat_type *J, const x_type &x) const {
    for (std::size_t i = 0; i < d_.size(); ++i) {
      for (std::size_t j = 0; j < X0_SIZE; ++j) {
        (*J)(i, j) = d_[i].C1(j)(x);
      }
    }
  }
  void eval_H(mat_type *H, const vec_type &r, const mat_type &J, const x_type &x) const {
    namespace bnu = boost::numeric::ublas;
    *H = bnu::prod(bnu::trans(J), J);
    for (std::size_t i = 0; i < d_.size(); ++i) {
      bnu::symmetric_matrix< double > tmp(X0_SIZE, X0_SIZE);
      for (std::size_t j = 0; j < X0_SIZE; ++j) {
        for (std::size_t k = j; k < X0_SIZE; ++k) {
          tmp(j, k) = r(i) * d_[i].C2(j, k)(x);
        }
      }
      *H += tmp;
    }
  }
};
} // namespace tx

#endif /* __TX__LEAST_SQUARES_HPP__ */
