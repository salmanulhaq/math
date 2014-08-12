#include <stan/math/matrix/dot_product.hpp>
#include <gtest/gtest.h>

TEST(MathMatrix, dot_product) {        
  Eigen::VectorXd v1(3);
  v1 << 10.2, 1, 1.1;
  
  Eigen::VectorXd v2(2);
  v2 << 1, 6;  
  
  Eigen::RowVectorXd rv1(3);
  rv1 << 0.1, 0.2, 2.0;
  
  Eigen::RowVectorXd rv2(3);
  rv2 << 10.1, 10,-12;  
        
  std::vector<double> v3(11, 2.1);
  v3[7] = 5;
        
  using stan::math::dot_product;
    
  EXPECT_DOUBLE_EQ(10.1*10.2 + 10 - 12*1.1, dot_product(rv2, v1));
  EXPECT_DOUBLE_EQ(37, dot_product(v2, v2));
  EXPECT_DOUBLE_EQ(10.2*0.1 + 0.2 + 1.1*2, dot_product(rv1, v1));
  EXPECT_DOUBLE_EQ(2.1*2.1*10 + 25, dot_product(v3, v3));
}

