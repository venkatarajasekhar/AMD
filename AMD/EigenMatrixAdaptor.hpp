#ifndef EIGEN_MATRIX_ADAPATOR_HPP
#define EIGEN_MATRIX_ADAPATOR_HPP

#include <ostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "MatrixAdaptor.hpp"

namespace AMD {

/** A structure that helps us figure out which Cholesky to use */
template <typename MatrixType> struct eigen_llt_t {};

/** Use Eigen::LLT for dense matrices */
template <typename T>
struct eigen_llt_t <Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > {
  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
  typedef Eigen::LLT<MatrixType> LLTType;
};

/** Use Eigen::SimplicialLLT for sparse matrices */
template <typename T>
struct eigen_llt_t <Eigen::SparseMatrix<double> > {
  typedef Eigen::SparseMatrix<T> MatrixType;
  typedef Eigen::SimplicialLLT<MatrixType> LLTType;
};

template <typename T>
struct MatrixAdaptor_t<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > {

  /**< define the type of the values */
  typedef T value_type;

  /**< typedef the matrix so that we can reuse it */
  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrix_type;

  /**
   * 0. static int createMatrix (int m, int n, std::string name);
   * Create a matrix of the requested size and name it if possible.
   * This should replace all occurences of "new MT" because that assumes
   * there is a default constructor for any matrix type we are using. In 
   * general, we should not call any matrix function directly and always
   * should go through MatrixAdaptor_t.
   */
  static matrix_type createMatrix (int m, int n, std::string name) {
    return matrix_type(m, n);
  }

  /**
   * 1. 
   * @brief A function to return the number of rows in a matrix
   * @param[in] A The matrix for which we need the number of rows.
   * @return number of rows in A
   */
  static int getNumRows (const matrix_type& A) { return A.rows(); }

  /**
   * 2.
   * @brief A function to return the number of cols in a matrix
   * @param[in] A The matrix for which we need the number of cols.
   * @return number of cols in A
   */
  static int getNumCols (const matrix_type& A) { return A.cols(); }

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
    C = A + B;
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
    C = A + B;
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
    C = A * B;
  }

  /** 
   * 6. 
   * @brief Compute the matrix transpose.
   * @param[in] A The matrix to be transposed.
   * @param[out] B B is overwritten with A^{T}
   */
  static void transpose (const matrix_type& A, 
                         matrix_type& B) {
    B = A.transpose();
  }

  /** 
   * 7. 
   * @brief Compute element-wise negation of a matrix.
   * @param[in] A The matrix to be negated.
   * @param[out] B B is overwritten with -1.0*A
   */
  static void negation (const matrix_type& A,
                        matrix_type& B) {
    B = A * -1.0;
  }

  /** 
   * 8. 
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */
  static void inv(const matrix_type& A,
                  matrix_type& B) {
  }

  /** 
   * 9. 
   * @brief Compute the trace of a matrix.
   * @param[in] A The (square) matrix whose trace is to be computed.
   * @return Trace of A
   */
  static value_type trace(const matrix_type& A) { return A.trace(); }

  /** 
   * 10. 
   * @brief Create an identity matrix of the requested size.
   * @param[in] n The dimension of the identity matrix.
   * @return Identity matrix of the required size.
   */
  static matrix_type eye(int n) { return matrix_type::Identity(n,n); }

  /** 
   * 11. 
   * @brief Create an zero matrix of the requested size.
   * @param[in] n The dimension of the zero matrix.
   * @return Zero matrix of the required size.
   */
  static matrix_type zeros(int m, int n) { return matrix_type::zero(m,n); }

  /** 
   * 12. 
   * @brief Compute the logdet of a matrix A.
   * @param[out] A Symmetrix positive definite matrix.
   * @return logdet of A
   */
  static double logdet(const matrix_type& A) {
    /** First, create a copy because we want to preserve A */
    matrix_type A_shdw = matrix_type(A);
 
    /** First compute Cholesky factorization */
    typename eigen_llt_t<matrix_type>::LLTType cholesky_A_shdw(A_shdw);
    A_shdw = cholesky_A_shdw.matrixL();
 
    double log_trace = 0.0;
    for (int i=0; i<LogDetBaseType::n; ++i) 
                  log_trace += log(A_shdw.diagonal()[i]);

    /** Multiply by 2 coz A = L'L */
    log_trace *= 2.0;

    return log_trace;
  }

  /** 
   * 13. 
   * @brief Copy the contents of a matrix.
   * @param[out] A A is overwritten with B.
   * @param[in] B The matrix to be copied.
   */
  static void copy (matrix_type &A, const matrix_type &B) {  A = B; }

  /** 
   * 14. 
   * @brief print out the matrix 
   * @param[in] A The matrix to be printed out.
   * @param[out] os Output stream for the printing.
   */
  static void print (const matrix_type& A, 
                     std::ostream& os=std::cout) { os << A; }

  /** 
   * 15. 
   * @brief Extract the diagonal elements of a matrix.
   * @param[in] A The (square) matrix whose diagonal is to be extracted.
   * @param[out] B A diagonal matrix containing entries from A.
   */
  static void diag(const matrix_type& A,
                   matrix_type& B) {
    /* create a zeros matrix of the right dimension and assign to B */
    const int n = A.rows();
    B = matrix_type::zero(n, n);

    /* Set the diagonals of B */
    for (int i=0; i<n; ++i) B(i,i) = A.diagonal()[i];
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
    const int m = A.rows();
    const int n = A.cols();

    /** Verify that A, B, and C are the same size */

    /** Do the actual multiplication */
    for (int j=0; j<n; ++j) for (int i=0; i<m; ++i) C(i,j) = A(i,j)*B(i,j);
  }

  /**
   * 17.
   * @brief Multiply one scalar with matrix.
   * @param[in] a The scalar.
   * @param[in] B The matrix.
   * @param[out] C C is overwritten with (A*B).
   */
  static void multiply(const value_type& a,
                       const matrix_type& A,
                       matrix_type& B) { B = A * a; }

  /**
   * 18.
   * @brief Multiply one scalar with matrix.
   * @param[in] A The matrix.
   * @param[in] b The scalar.
   * @param[out] C C is overwritten with (A*B).
   */
  static void multiply(const matrix_type& A,
                       const value_type& a,
                       matrix_type& B) { B = A * a; }
};

} 

#endif /** EIGEN_MATRIX_ADAPATOR_HPP */
