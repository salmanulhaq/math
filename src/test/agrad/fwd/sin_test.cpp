#include <gtest/gtest.h>
#include <stan/agrad/fvar.hpp>
#include <stan/agrad/var.hpp>
#include <test/agrad/util.hpp>

TEST(AgradFvar, sin) {
  using stan::agrad::fvar;
  using std::sin;
  using std::cos;

  fvar<double> x(0.5,1.0);
  
  fvar<double> a = sin(x);
  EXPECT_FLOAT_EQ(sin(0.5), a.val_);
  EXPECT_FLOAT_EQ(cos(0.5), a.d_);

  fvar<double> b = 2 * sin(x) + 4;
  EXPECT_FLOAT_EQ(2 * sin(0.5) + 4, b.val_);
  EXPECT_FLOAT_EQ(2 * cos(0.5), b.d_);

  fvar<double> c = -sin(x) + 5;
  EXPECT_FLOAT_EQ(-sin(0.5) + 5, c.val_);
  EXPECT_FLOAT_EQ(-cos(0.5), c.d_);

  fvar<double> d = -3 * sin(x) + 5 * x;
  EXPECT_FLOAT_EQ(-3 * sin(0.5) + 5 * 0.5, d.val_);
  EXPECT_FLOAT_EQ(-3 * cos(0.5) + 5, d.d_);

  fvar<double> y(-0.5,1.0);
  fvar<double> e = sin(y);
  EXPECT_FLOAT_EQ(sin(-0.5), e.val_);
  EXPECT_FLOAT_EQ(cos(-0.5), e.d_);

  fvar<double> z(0.0,1.0);
  fvar<double> f = sin(z);
  EXPECT_FLOAT_EQ(sin(0.0), f.val_);
  EXPECT_FLOAT_EQ(cos(0.0), f.d_);
}

TEST(AgradFvarVar, sin) {
  using stan::agrad::fvar;
  using stan::agrad::var;
  using std::sin;
  using std::cos;

  fvar<var> x(1.5,1.3);
  fvar<var> a = sin(x);

  EXPECT_FLOAT_EQ(sin(1.5), a.val_.val());
  EXPECT_FLOAT_EQ(1.3 * cos(1.5), a.d_.val());

  AVEC y = createAVEC(x.val_);
  VEC g;
  a.val_.grad(y,g);
  EXPECT_FLOAT_EQ(cos(1.5), g[0]);

  y = createAVEC(x.d_);
  a.d_.grad(y,g);
  EXPECT_FLOAT_EQ(0, g[0]);
}

TEST(AgradFvarFvar, sin) {
  using stan::agrad::fvar;
  using std::sin;
  using std::cos;

  fvar<fvar<double> > x;
  x.val_.val_ = 1.5;
  x.val_.d_ = 2.0;

  fvar<fvar<double> > a = sin(x);

  EXPECT_FLOAT_EQ(sin(1.5), a.val_.val_);
  EXPECT_FLOAT_EQ(2.0 * cos(1.5), a.val_.d_);
  EXPECT_FLOAT_EQ(0, a.d_.val_);
  EXPECT_FLOAT_EQ(0, a.d_.d_);

  fvar<fvar<double> > y;
  y.val_.val_ = 1.5;
  y.d_.val_ = 2.0;

  a = sin(y);
  EXPECT_FLOAT_EQ(sin(1.5), a.val_.val_);
  EXPECT_FLOAT_EQ(0, a.val_.d_);
  EXPECT_FLOAT_EQ(2.0 * cos(1.5), a.d_.val_);
  EXPECT_FLOAT_EQ(0, a.d_.d_);
}
