#ifndef MATRIX_MATRIX_ADAPATOR_HPP
#define MATRIX_MATRIX_ADAPATOR_HPP

#include <ostream>
#include <elemental.hpp>

#include "MatrixAdaptor.hpp"

namespace AMD {

template <typename T>
struct MatrixAdaptor_t<elem::Matrix<T> > {

  /**< define the type of the values */
  typedef T value_type;

  /**< typedef the matrix so that we can reuse it */
  typedef elem::Matrix<T> matrix_type;

  /**
   * 0. 
   * @brief Create a matrix of the requested size and name it if possible.
   * @param[in] m The number of rows
   * @param[in] n The number of cols
   * @return A matrix of dimension (m,n)
   */
  static int createMatrix (int m, int n, std::string name="") {
    return matrix_type (m,n);
  }

  /**
   * 1. 
   * @brief A function to return the number of rows in a matrix
   * @param[in] A The matrix for which we need the number of rows.
   * @return number of rows in A
   */
  static int getNumRows (const matrix_type& A) { return A.Height(); }

  /**
   * 2.
   * @brief A function to return the number of cols in a matrix
   * @param[in] A The matrix for which we need the number of cols.
   * @return number of cols in A
   */
  static int getNumCols (const matrix_type& A) { return A.Width(); }

  /** 
   * 3. 
   * @brief Add two matrices 
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A+B)
   */
  static void add (const matrix_type& A,
                   const matrix_type& B,
                   matrix_type& C) {

    /** Result matrices should always have sufficient space */
    C.Resize(A.Height(), A.Width());

    /** first copy the matrix over */
    elem::Copy (A, C);

    /** now, subtract the other matrix in */
    elem::Axpy (1.0, B, C);
  }

  /** 
   * 4. 
   * @brief Subtract one matrix from another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A-B)
   */
  static void minus (const matrix_type& A,
                     const matrix_type& B, 
                     matrix_type& C) {

    /** Result matrices should always have sufficient space */
    C.Resize(A.Height(), A.Width());

    /** first copy the matrix over */
    elem::Copy (A, C);

    /** now, subtract the other matrix in */
    elem::Axpy (-1.0, B, C);
  }

  /** 
   * 5. 
   * @brief Multiply one matrix with another.
   * @param[in] A The first matrix
   * @param[in] B The second matrix
   * @param[out] C C is overwritten with (A*B)
   */
  static void multiply (const matrix_type& A,
                        const matrix_type& B,
                        matrix_type& C) {

    /** Result matrices should always have sufficient space */
    C.Resize(A.Height(), B.Width());

    /** We have to do a Gemm */
    elem::Gemm (elem::NORMAL, elem::NORMAL, 1.0, A, B, 0.0, C);
  }

  /** 
   * 6. 
   * @brief Compute the matrix transpose.
   * @param[in] A The matrix to be transposed.
   * @param[out] B B is overwritten with A^{T}
   */
  static void transpose (const matrix_type& A, 
                         matrix_type& B) {

    /** Result matrices should always have sufficient space */
    B.Resize(A.Width(), A.Height());

    /** Compute transpose */
    elem::Transpose (A, B);
  }

  /** 
   * 7. 
   * @brief Compute element-wise negation of a matrix.
   * @param[in] A The matrix to be negated.
   * @param[out] B B is overwritten with -1.0*A
   */
  static void negation (const matrix_type& A,
                        matrix_type& B) {

    /** Result matrices should always have sufficient space */
    B.Resize(A.Height(), A.Width());

    /** Copy over the matrix */
    elem::Copy (A, B);

    /** Multiply by -1.0 */
    elem::Scal(-1.0, B);
  }

  /** 
   * 8. 
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */
  static void inv(const matrix_type& A,
                  matrix_type& B) {

    /** Assume that the matrix has an inverse */
    const int n = A.Height();

    /** First, copy over an identity as the right hand side */
    B = eye(n);

    /** Solve the linear system using LU */
    elem::lu::SolveAfter(elem::NORMAL, A, B);
  }

  /** 
   * 9. 
   * @brief Compute the trace of a matrix.
   * @param[in] A The (square) matrix whose trace is to be computed.
   * @return Trace of A
   */
  static value_type trace(const matrix_type& A) {
    value_type traceVal = 0.0;
    for (int i=0; i<A.Height(); ++i) traceVal += A.Get(i,i);

    return traceVal;
  }

  /** 
   * 10. 
   * @brief Create an identity matrix of the requested size.
   * @param[in] n The dimension of the identity matrix.
   * @return Identity matrix of the required size.
   */
  static matrix_type eye(int n) {
    matrix_type I = zeros(n,n);
    for (int i=0; i<n; ++i) I.Set(i,i,1.0);
    return I;
  }

  /** 
   * 11. 
   * @brief Create an zero matrix of the requested size.
   * @param[in] n The dimension of the zero matrix.
   * @return Zero matrix of the required size.
   */
  static matrix_type zeros(int m, int n) {
    matrix_type zeroMat(m,n);
    elem::Zero(zeroMat);

    return zeroMat;
  }

  /** 
   * 12. 
   * @brief Compute the logdet of a matrix A.
   * @param[out] A Symmetrix positive definite matrix.
   * @return logdet of A
   */
  static double logdet(const matrix_type& A) {
    /** First, create a copy because we want to preserve A */
    matrix_type L(A);

    /** We compute the logdet by first computing the Cholesky factorization */
    elem::Cholesky(elem::UPPER, L);

    /** The above step fails if the matrix is not PSD */
    const int n = L.Height();
    value_type result = 0.0;
    for (int i=0; i<n; ++i) result += log(L.Get(i,i));

    /** Multiply by 2 coz A = L'L */
    result *= 2.0;

    return result;
  }

  /** 
   * 13. 
   * @brief Copy the contents of a matrix.
   * @param[out] A A is overwritten with B.
   * @param[in] B The matrix to be copied.
   */
  static void copy (matrix_type &A, const matrix_type &B) { 

    /** Result matrices should always be of the right size */
    A.Resize (B.Height(), B.Width());

    /** This one is pretty simple, but the order is different */
    elem::Copy (B, A);
  }

  /** 
   * 14. 
   * @brief print out the matrix 
   * @param[in] A The matrix to be printed out.
   * @param[out] os Output stream for the printing.
   */
  static void print (const matrix_type& A, 
                     std::ostream& os=std::cout) {
    elem::Print(A, "", os);
  }

  /** 
   * 15. 
   * @brief Extract the diagonal elements of a matrix.
   * @param[in] A The (square) matrix whose diagonal is to be extracted.
   * @param[out] B A diagonal matrix containing entries from A.
   */
  static void diag(const matrix_type& A,
                   matrix_type& B) {
    /* create a zeros matrix of the right dimension and assign to B */
    const int n = A.Width();
    B = zeros(n, n);

    /* Set the diagonals of B */
    for (int i=0; i<n; ++i) B.Set(i, i, A.Get(i,i));
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

    /** Result matrices should always be of the right size */
    C.Resize (A.Height(), A.Width());

    /* Get the matrix dimensions */
    const int m = A.Height();
    const int n = A.Width();

    /* Simple element-wise product */
    for (int i=0; i<m; ++i) 
      for (int j=0; j<n; ++j) 
        C.Set(i, j, (A.Get(i,j)*B.Get(i,j)));
  }

  /**
   * 17.
   * @brief Multiply one scalar with matrix.
   * @param[in] A The matrix.
   * @param[in] a The scalar.
   * @param[out] B B is overwritten with (A*B).
   */
  static void multiply(const matrix_type& A,
                       const value_type& a,
                       matrix_type& B) {
    /** Result matrices should always be of the right size */
    B.Resize(A.Height(), A.Width());

    /** Get the matrix dimensions. */
    const int m = A.Height();
    const int n = A.Width();
   
    /** Simple scalar-matrix product. */ 
    for (int i=0; i<m; ++i)
      for (int j=0; j<n; ++j)
        B.Set(i, j, (a*A.Get(i, j)));
  }
};

} 

#endif /** MATRIX_MATRIX_ADAPATOR_HPP */
