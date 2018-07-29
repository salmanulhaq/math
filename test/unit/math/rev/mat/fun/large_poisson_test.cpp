
#include <stan/math/rev/mat.hpp>
#include <gtest/gtest.h>
#include <test/unit/math/rev/mat/fun/util.hpp>
#include <vector>

TEST(large, poisson_lpmf) {
  using stan::math::value_of;
  using stan::math::var;
  using std::vector;

  const std::size_t terms = 10000000;

  using stan::math::vector_v;

  vector_v lambda(terms);
  std::vector<int> n(terms, 3);

  for (int i = 0; i < terms; i++)
    lambda(i) = 1.0;

  var lp = stan::math::poisson_lpmf(n, lambda);

  lp.grad();
}
