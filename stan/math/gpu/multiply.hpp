#ifndef STAN_MATH_GPU_MULTIPLY_HPP
#define STAN_MATH_GPU_MULTIPLY_HPP
#ifdef STAN_OPENCL
#include <stan/math/gpu/matrix_gpu.hpp>
#include <stan/math/gpu/kernels/scalar_mul.hpp>
#include <stan/math/gpu/kernels/matrix_multiply.hpp>
#include <Eigen/Dense>

namespace stan {
namespace math {
/**
 * Multiplies the specified matrix on the GPU
 * with the specified scalar.
 *
 * @param A matrix
 * @param scalar scalar
 * @return matrix multipled with scalar
 */
inline matrix_gpu multiply(const matrix_gpu& A, const double scalar) {
  matrix_gpu temp(A.rows(), A.cols());
  if (A.size() == 0)
    return temp;
  try {
    opencl_kernels::scalar_mul(cl::NDRange(A.rows(), A.cols()), temp.buffer(),
                               A.buffer(), scalar, A.rows(), A.cols());
  } catch (const cl::Error& e) {
    check_opencl_error("multiply scalar", e);
  }
  return temp;
}

/**
 * Multiplies the specified matrix on the GPU
 * with the specified scalar.
 *
 * @param scalar scalar
 * @param A matrix
 * @return matrix multipled with scalar
 */
inline matrix_gpu multiply(const double scalar, const matrix_gpu& A) {
  return multiply(A, scalar);
}

/**
 * Computes the product of the specified GPU matrices.
 *
 * Computes the matrix multiplication C[M, K] = A[M, N] x B[N, K]
 *
 * @param A first matrix
 * @param B second matrix
 * @return the product of the first and second matrix
 *
 * @throw <code>std::invalid_argument</code> if the
 *   number of columns in A and rows in B do not match
 */
inline matrix_gpu multiply(const matrix_gpu& A, const matrix_gpu& B) {
  check_size_match("multiply (GPU)", "A.cols()", A.cols(), "B.rows()",
                   B.rows());
  matrix_gpu temp(A.rows(), B.cols());
  if (A.size() == 0 || B.size() == 0)
    return temp;
  int local = opencl_kernels::matrix_multiply.make_functor.get_opts().at(
      "THREAD_BLOCK_SIZE");
  int Mpad = ((A.rows() + local - 1) / local) * local;
  int Npad = ((B.cols() + local - 1) / local) * local;
  int Kpad = ((A.cols() + local - 1) / local) * local;
  // padding the matrices so the dimensions are divisible with local
  // improves performance and readability because we can omit
  // if statements in the
  // multiply kernel
  matrix_gpu tempPad(Mpad, Npad);
  matrix_gpu Apad(Mpad, Kpad);
  matrix_gpu Bpad(Kpad, Npad);
  opencl_kernels::zeros(cl::NDRange(Mpad, Kpad), Apad.buffer(), Mpad, Kpad,
                        TriangularViewGPU::Entire);
  opencl_kernels::zeros(cl::NDRange(Kpad, Npad), Bpad.buffer(), Kpad, Npad,
                        TriangularViewGPU::Entire);
  Apad.sub_block(A, 0, 0, 0, 0, A.rows(), A.cols());
  Bpad.sub_block(B, 0, 0, 0, 0, B.rows(), B.cols());
  int wpt = opencl_kernels::matrix_multiply.make_functor.get_opts().at(
      "WORK_PER_THREAD");
  try {
    opencl_kernels::matrix_multiply(
        cl::NDRange(Mpad, Npad / wpt), cl::NDRange(local, local / wpt),
        Apad.buffer(), Bpad.buffer(), tempPad.buffer(), Apad.rows(),
        Bpad.cols(), Bpad.rows());
  } catch (cl::Error& e) {
    check_opencl_error("multiply", e);
  }
  // unpadding the result matrix
  temp.sub_block(tempPad, 0, 0, 0, 0, temp.rows(), temp.cols());
  return temp;
}
}  // namespace math
}  // namespace stan

#endif
#endif
