#ifndef MATRIX_ADAPTOR_HPP
#define MATRIX_ADAPTOR_HPP
/**
 * @file MatrixAdaptor.hpp
 *
 * @author Anju Kambadur Peder Olsen, Allan Sapucaia
 *
 * @brief This file defines a adaptor interface. The user has two options
 * for matrix computation currently. One option is symbolic computation 
 * using SymbolicMatrixMatlab and SymbolicScalarMatlab methonds. Another 
 * option is calling the Elemental-083 APIs.
 */

#include "AMD/config.h"

namespace AMD {

template <typename MatrixType>
struct MatrixAdaptor_t {

  /** define value_type */

  /** define matrix_type */

  /**
   * static boost::shared_ptr<matrix_type> defaultConstructMatrix 
   *                            (int m, int n, std::string name="");
   * static boost::shared_ptr<matrix_type> copyConstructMatrix 
   *                            (const matrix_type& original);
   *
   * Create a matrix of the requested size and name it if possible.
   * This should replace all occurences of "new MT" because that assumes
   * there is a default constructor for any matrix type we are using. In 
   * general, we should not call any matrix function directly and always
   * should go through MatrixAdaptor_t.
   */

  /**
   * 1. static int getNumRows (const matrix_type& A);
   * @brief A function to return the number of rows in a matrix
   * @param[in] A The matrix for which we need the number of rows.
   * @return number of rows in A
   */

  /**
   * 2. static int getNumCols (const matrix_type& A);
   * @brief A function to return the number of cols in a matrix
   * @param[in] A The matrix for which we need the number of cols.
   * @return number of cols in A
   */

  /** 
   * 3.  static void add (const matrix_type& A,
   *                      const matrix_type& B,
   *                      matrix_type& C);
   * @brief Add two matrices 
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A+B)
   */

  /** 
   * 4.  static void minus(const matrix_type& A,
   *                       const matrix_type& B,
   *                       matrix_type& C);
   * @brief Subtract one matrix from another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A-B)
   */

  /** 
   * 5.  static void multiply (const matrix_type& A,
   *                           const matrix_type& B,
   *                           matrix_type& C);
   * @brief Multiply one matrix with another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A*B)
   */

  /** 
   * 6.  static void transpose (const matrix_type& A, 
   *                            matrix_type& B);
   * @brief Compute the matrix transpose.
   * @param[in] A The matrix to be transposed.
   * @param[out] B B is overwritten with A^{T}
   */

  /** 
   * 7.  static void negation(const matrix_type& A,
   *                          matrix_type& B);
   * @brief Compute element-wise negation of a matrix.
   * @param[in] A The matrix to be negated.
   * @param[out] B B is overwritten with -1.0*A
   */

  /** 
   * 8.  static void inv(const matrix_type& A,
   *                     matrix_type& B);
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */


  /** 
   * 9.  static value_type trace(const matrix_type& A);
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (square) matrix whose trace is to be computed.
   * @return Trace of A
   */

  /** 
   * 10. static matrix_type eye(int n);
   * @brief Create an identity matrix of the requested size.
   * @param[in] n The dimension of the identity matrix.
   * @return Identity matrix of the required size.
   */

  /** 
   * 11. static matrix_type zeros(int m, int n);
   * @brief Create an zero matrix of the requested size.
   * @param[in] n The dimension of the zero matrix.
   * @return Zero matrix of the required size.
   */

  /** 
   * 12. static value_type logdet(const matrix_type& A);
   * @brief Compute the logdet of a matrix A.
   * @param[out] A Symmetrix positive definite matrix.
   * @return logdet of A
   */

  /** 
   * 13. static void copy (matrix_type &A, const matrix_type &B);
   * @brief Copy the contents of a matrix.
   * @param[out] A A is overwritten with B.
   * @param[in] B The matrix to be copied.
   */


  /** 
   * 14. static void print (const matrix_type& A, 
   *                        std::ostream& os=std::cout);
   * @brief print out the matrix 
   * @param[in] A The matrix to be printed out.
   * @param[out] os Output stream for the printing.
   */

  /** 
   * 15.  static void diag(const matrix_type& A,
   *                       matrix_type& B);
   * @brief Extract the diagonal elements of a matrix.
   * @param[in] A The (square) matrix whose diagonal is to be extracted.
   * @param[out] B A diagonal matrix containing entries from A.
   */

  /** 
   * 16. static void elementwiseProduct(const matrix_type& A,
   *                                    const matrix_type& B,
   *                                    matrix_type& C);
   * @brief Compute the element-wise product of two matrices.
   * @param[in] A the first matrix.
   * @param[in] B the second matrix.
   * @param[out] C the result, which contains A.*B.
   */ 

  /**
   * 17. static void multiply(const matrix_type& A,
   *                          const value_type& a,
   *                          matrix_type& B);
   * @brief Multiply a matrix with a scalar
   * @param[in] A the matrix.
   * @param[in] a the scalar.
   * @param[out] B the result, which contains A*a.
   */
};

}

#include "SymbolicMatrixAdaptor.hpp"

#if AMD_HAVE_EIGEN
  #include "EigenMatrixAdaptor.hpp"
#endif

#endif /** MATRIX_ADAPTOR_HPP */
