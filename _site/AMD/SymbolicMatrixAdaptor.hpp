#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

/**
 * @file SymbolicMatrixAdaptor.hpp
 *
 * @author Anju Kambadur, Peder Olsen, Suyang Zhu
 *
 * @brief This file defines the adaptor to symbolic matrix-matrix function
 * and scalar-matrix function methods.
 */

#include <ostream>
#include <boost/shared_ptr.hpp>
#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD {

  template <>
  struct MatrixAdaptor_t < SymbolicMatrixMatlab > {

    /**< define the type of the scalar values */
    typedef SymbolicScalarMatlab value_type;

    /**< define the type of the matrix */
    typedef SymbolicMatrixMatlab matrix_type;

    /**<
     * -1. Generate a shared_ptr for a matric that is default constructed.
     */                            
    static boost::shared_ptr<matrix_type> defaultConstructMatrix 
                                (int m, int n, std::string name="") {
        return boost::shared_ptr<matrix_type>(new matrix_type(name, m, n));
    }

    /**<
     * 0. Generate a shared_ptr for a matric that is default constructed.
     */                            
    static boost::shared_ptr<matrix_type> copyConstructMatrix 
                               (const matrix_type& original) {
        return boost::shared_ptr<matrix_type>(new matrix_type(original));
    }

    /**
     * 1.
     * @brief A function to return the number of rows in a matrix
     * @param[in] A The matrix for which we need the number of rows.
     * @return number of rows in A
     */
    static int getNumRows(const matrix_type& A) {
      return A.getNumRows();
    }

    /**
     * 2.
     * @brief A function to return the number of cols in a matrix
     * @param[in] A The matrix for which we need the number of cols.
     * @return number of cols in A
     */
    static int getNumCols(const matrix_type& A) {
      return A.getNumCols();
    }

    /**
     * 3.
     * @brief Add two matrices
     * @param[in] A The first matrix
     * @param[in] B The second matrix
     * @param[out] C C is overwritten with (A+B)
     */
    static void add(const matrix_type& A,
      const matrix_type& B,
      matrix_type& C) {
      copy(C, A + B);
    }
    /**
     * 4.
     * @brief Subtract one matrix from another.
     * @param[in] A The first matrix
     * @param[in] B The second matrix
     * @param[out] C C is overwritten with (A-B)
     */
    static void minus(const matrix_type& A,
      const matrix_type& B,
      matrix_type& C) {
      copy(C, A - B);
    }

    /**
     * 5.
     * @brief Multiply one matrix with another.
     * @param[in] A The first matrix
     * @param[in] B The second matrix
     * @param[out] C C is overwritten with (A*B)
     */
    static void multiply(const matrix_type& A,
      const matrix_type& B,
      matrix_type& C) {
      copy(C, A*B);
    }

    /**
     * 6.
     * @brief Compute the matrix transpose.
     * @param[in] A The matrix to be transposed.
     * @param[out] B B is overwritten with A^{T}
     */

    static void transpose(const matrix_type& A,
      matrix_type& B) {
      copy(B, AMD::transpose(A));
    }

    /**
     * 7.
     * @brief Compute negation negation of a matrix.
     * @param[in] A The matrix to be negated.
     * @param[out] B B is overwritten with -1.0*A
     */
    static void negation(const matrix_type& A,
      matrix_type& B) {
      copy(B, -A);
    }

    /**
     * 8.
     * @brief Compute the inverse of a matrix.
     * @param[in] A The (non-singular and square) matrix to be inverted.
     * @param[out] B B is overwritten with A's inverse.
     */
    static void inv(const matrix_type& A,
      matrix_type& B) {
      copy(B, AMD::inv(A));
    }

    /**
     * 9.
     * @brief Compute the trace of a matrix.
     * @param[in] A The (square) matrix whose trace is to be computed.
     * @return Trace of A
     */
    static value_type trace(const matrix_type& A) { return AMD::trace(A); }

    /**
     * 10.
     * @brief Create an identity matrix of the requested size.
     * @param[in] n The dimension of the identity matrix.
     * @return Identity matrix of the required size.
     */
    static matrix_type eye(int n) { return matrix_type::eye(n); }

    /**
     * 11.
     * @brief Create an zero matrix of the requested size.
     * @param[in] n The dimension of the zero matrix.
     * @return Zero matrix of the required size.
     */
    static matrix_type zeros(int m, int n) { return matrix_type::zeros(m, n); }

    /**
     * 12.
     * @brief Compute the logdet of a matrix A.
     * @param[out] A Symmetrix positive definite matrix.
     * @return logdet of A
     */
    static value_type logdet(const matrix_type& A) { return AMD::logdet(A); }

    /**
     * 13.
     * @brief Copy the contents of a matrix.
     * @param[out] A A is overwritten with B.
     * @param[in] B The matrix to be copied.
     */
    static void copy(matrix_type &A, const matrix_type &B) { A.copy(B); }

    /**
     * 14.
     * @brief print out the matrix
     * @param[in] A The matrix to be printed out.
     * @param[out] os Output stream for the printing.
     */
    static void print(const matrix_type& A,
      std::ostream& os = std::cout) {
      A.print(os);
    }

    /**
     * 15.
     * @brief Extract the diagonal elements of a matrix.
     * @param[in] A The (square) matrix whose diagonal is to be extracted.
     * @param[out] B A diagonal matrix containing entries from A.
     */
    static void diag(const matrix_type& A,
      matrix_type& B) {
      copy(B, AMD::diag(A));
    }

    /**
     * 16.
     * @brief Compute the element-wise product of two matrices.
     * @param[in] A the first matrix.
     * @param[in] B the second matrix.
     * @param[out] C the result, which contains A.*B.
     */
    static void elementwiseProduct(const matrix_type& A,
      const matrix_type& B,
      matrix_type& C) {
      copy(C, AMD::elementwiseProduct(A, B));
    }

    /**
     * 17.
     * @brief Multiply one matrix with scalar.
     * @param[in] A The matrix
     * @param[in] b The scalar
     * @param[out] C C is overwritten with (A*b)
     */
    static void multiply(const matrix_type& A,
      const value_type& a,
      matrix_type& B) {
      copy(B, A*a);
    }
  };

}

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
