#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD {

template <>
struct MatrixAdaptor_t<SymbolicMatrixMatlab> {

  /**
   * 1. 
   * @brief A function to return the number of rows in a matrix
   * @param[in] A The matrix for which we need the number of rows.
   * @return number of rows in A
   */
  static int getNumRows (const SymbolicMatrixMatlab& A) {
    return A.getNumRows();
  }

  /**
   * 2.
   * @brief A function to return the number of cols in a matrix
   * @param[in] A The matrix for which we need the number of cols.
   * @return number of cols in A
   */
  static int getNumCols (const SymbolicMatrixMatlab& A) {
    return A.getNumCols();
  }

   /** 
   * 3. 
   * @brief Add two matrices 
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A+B)
   */
  static void add (const SymbolicMatrixMatlab& A,
                   const SymbolicMatrixMatlab& B,
                   SymbolicMatrixMatlab& C) {
    copy(C, A+B);
  }

    /** 
   * 4. 
   * @brief Subtract one matrix from another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A-B)
   */
  static void minus (const SymbolicMatrixMatlab& A,
                     const SymbolicMatrixMatlab& B, 
                     SymbolicMatrixMatlab& C) {
    copy(C, A-B);
  }

  /** 
   * 5. 
   * @brief Multiply one matrix with another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A*B)
   */
  static void multiply (const SymbolicMatrixMatlab& A,
                        const SymbolicMatrixMatlab& B,
                        SymbolicMatrixMatlab& C) {
    copy(C, A*B);
  }

   /** 
   * 6. 
   * @brief Compute the matrix transpose.
   * @param[in] A The matrix to be transposed.
   * @param[out] B B is overwritten with A^{T}
   */

  static void transpose (const SymbolicMatrixMatlab& A, 
                         SymbolicMatrixMatlab& B) {
    copy(B, AMD::transpose(A));
  }

   /** 
   * 7. 
   * @brief Compute negation negation of a matrix.
   * @param[in] A The matrix to be negated.
   * @param[out] B B is overwritten with -1.0*A
   */
  static void negation(const SymbolicMatrixMatlab& A,
                       SymbolicMatrixMatlab& B) {
    copy(B, -A);
  }

   /** 
   * 8. 
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */
  static void inv(const SymbolicMatrixMatlab& A,
                  SymbolicMatrixMatlab& B) {
    copy(B, AMD::inv(A));
  }

    /** 
   * 9. 
   * @brief Compute the trace of a matrix.
   * @param[in] A The (square) matrix whose trace is to be computed.
   * @param[out] B The trace of input matrix.
   */
  static void trace(const SymbolicMatrixMatlab& A,
                    SymbolicScalarMatlab& B) {
    B.copy(AMD::trace(A));
  }

  /** 
   * 10. 
   * @brief Create an identity matrix of the requested size.
   * @param[in] A 
   * @param[in] n The dimension of the identity matrix.
   * @param[out] B The newly created identity matrix.
   */
  static void eye(SymbolicMatrixMatlab& A,  
                  int n,
                  SymbolicMatrixMatlab& B) {
    copy(B, A.eye(n));
  }

   /** 
   * 11. 
   * @brief Create an zero matrix of the requested size.
   * @param[in] A The origin matrix.
   * @param[in] m The row number of the zero matrix.
   * @param[in] n The column number of the zero matrix.
   * @param[out] B The newly created zero matrix.
   */ 
  static void zeros(SymbolicMatrixMatlab& A, 
                    int m, 
                    int n,
                    SymbolicMatrixMatlab& B) {
    copy(B, A.zeros(m,n));
  }

  /** 
   * 12. 
   * @brief Compute the logdet of a matrix A.
   * @param[in] A Symmetrix positive definite matrix.
   * @param[out] B The logdet of the input matrix.
   */
  static void logdet(const SymbolicMatrixMatlab& A,
                     SymbolicScalarMatlab& B) {
    B.copy(AMD::logdet(A));
  }

  /** 
   * 13. 
   * @brief Copy the contents of a matrix.
   * @param[out] A A is overwritten with B.
   * @param[in] B The matrix to be copied.
   */
  static void copy (SymbolicMatrixMatlab &A,      /**< target obj */
                    const SymbolicMatrixMatlab &B /**< source obj */ ) {
    // TODO symbol nRows nCols are private variables
    A.copy(B);
  }

  /** 
   * 14. 
   * @brief print out the matrix 
   * @param[in] A The matrix to be printed out.
   * @param[out] os Output stream for the printing.
   */
  static void print (const SymbolicMatrixMatlab& A, 
                     std::ostream& os=std::cout) {
    A.print(os);
  }

  /** 
   * 15. 
   * @brief Extract the diagonal elements of a matrix.
   * @param[in] A The (square) matrix whose diagonal is to be extracted.
   * @param[out] B A diagonal matrix containing entries from A.
   */
  static void diag(const SymbolicMatrixMatlab& A,
                   SymbolicMatrixMatlab& B) {
    copy(B, AMD::diag(A));
  }

  /** 
   * 16. 
   * @brief Compute the element-wise product of two matrices.
   * @param[in] A the first matrix.
   * @param[in] B the second matrix.
   * @param[out] C the result, which contains A.*B.
   */
  static void elementwiseProd(const SymbolicMatrixMatlab& A,
                              const SymbolicMatrixMatlab& B,
                              SymbolicMatrixMatlab& C) {
    copy(C, AMD::elementwiseProd(A, B));
  }
};

} 

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
