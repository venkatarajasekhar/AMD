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
struct eigen_llt_t <Eigen::SparseMatrix<T> > {
  typedef Eigen::SparseMatrix<T> MatrixType;
  typedef Eigen::SimplicialLLT<MatrixType> LLTType;
};

template <typename ScalarType, typename SparseOrDenseMatrixType>
struct EigenMatrixAdaptorBase {
  typedef ScalarType value_type;
  typedef SparseOrDenseMatrixType matrix_type;

  static matrix_type createMatrix (int m, int n, std::string name="") {
    return matrix_type(m, n);
  }

  static int getNumRows (const matrix_type& A) { return A.rows(); }

  static int getNumCols (const matrix_type& A) { return A.cols(); }

  static void add (const matrix_type& A,
                   const matrix_type& B,
                   matrix_type& C) { C = A + B; }

  static void minus (const matrix_type& A,
                     const matrix_type& B, 
                     matrix_type& C) { C = A - B; }

  static void multiply (const matrix_type& A,
                        const matrix_type& B,
                        matrix_type& C) { C = A * B; }

  static void transpose (const matrix_type& A, 
                         matrix_type& B) { B = A.transpose(); }

  static void negation (const matrix_type& A,
                        matrix_type& B) { B = A * -1.0; }

  static double logdet(const matrix_type& A) {
    matrix_type A_shdw = matrix_type(A);
    typename eigen_llt_t<matrix_type>::LLTType cholesky_A_shdw(A_shdw);
    A_shdw = cholesky_A_shdw.matrixL();
 
    double log_trace = 0.0;
    for (int i=0; i<A_shdw.rows(); ++i)log_trace+=log(A_shdw.diagonal()[i]);

    return 2.0*log_trace;
  }

  static void copy (matrix_type &A, const matrix_type &B) {  A = B; }

  static void print (const matrix_type& A, 
                     std::ostream& os=std::cout) { os << A; }

  static void elementwiseProduct(const matrix_type& A,
                                 const matrix_type& B,
                                 matrix_type& C) { C = A.cwiseProduct(B); }

  static void multiply(const matrix_type& A,
                       const value_type& a,
                       matrix_type& B) { B = A * a; }
};

template <typename T>
struct MatrixAdaptor_t<Eigen::Matrix<T, 
                                     Eigen::Dynamic, 
                                     Eigen::Dynamic> > :
public EigenMatrixAdaptorBase<T, Eigen::Matrix<T,
                                            Eigen::Dynamic,
                                            Eigen::Dynamic> > {
  typedef T value_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrix_type;
  typedef EigenMatrixAdaptorBase<value_type, matrix_type> base_type;

  using base_type::createMatrix;
  using base_type::getNumRows;
  using base_type::getNumCols;
  using base_type::add;
  using base_type::minus;
  using base_type::multiply;
  using base_type::transpose;
  using base_type::negation;
  using base_type::logdet;
  using base_type::copy;
  using base_type::print;
  using base_type::elementwiseProduct;

  /** 
   * 8. 
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */
  static void inv(const matrix_type& A,
                  matrix_type& B) { B = A.inverse(); }

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
  static matrix_type zeros(int m, int n) { return matrix_type::Zero(m,n); }

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
    B = matrix_type::Zero(n, n);

    /* Set the diagonals of B */
    for (int i=0; i<n; ++i) B(i,i) = A.diagonal()[i];
  }
};

template <typename T>
struct MatrixAdaptor_t<Eigen::SparseMatrix<T> > :
              public EigenMatrixAdaptorBase<T, Eigen::SparseMatrix<T> > {
  typedef T value_type;
  typedef Eigen::SparseMatrix<T> matrix_type;
  typedef EigenMatrixAdaptorBase<value_type, matrix_type> base_type;

  using base_type::createMatrix;
  using base_type::getNumRows;
  using base_type::getNumCols;
  using base_type::add;
  using base_type::minus;
  using base_type::multiply;
  using base_type::transpose;
  using base_type::negation;
  using base_type::logdet;
  using base_type::copy;
  using base_type::print;

  /** 
   * 8. 
   * @brief Compute the inverse of a matrix.
   * @param[in] A The (non-singular and square) matrix to be inverted.
   * @param[out] B B is overwritten with A's inverse.
   */
  static void inv(const matrix_type& A,
                  matrix_type& B) {
    matrix_type A_shdw = A;
    typename eigen_llt_t<matrix_type>::LLTType cholesky_A_shdw(A_shdw);
    matrix_type rhs = eye(A.rows());
    B = cholesky_A_shdw.solve(rhs);
  }

  /** 
   * 9. 
   * @brief Compute the trace of a matrix.
   * @param[in] A The (square) matrix whose trace is to be computed.
   * @return Trace of A
   */
  static value_type trace(const matrix_type& A) { 
    double trace_val = 0.0;
    for (int i=0; i<A.rows(); ++i) trace_val+=A.diagonal()[i];
    return trace_val;
  }

  /** 
   * 10. 
   * @brief Create an identity matrix of the requested size.
   * @param[in] n The dimension of the identity matrix.
   * @return Identity matrix of the required size.
   */
  static matrix_type eye(int n) { 
    matrix_type A = createMatrix(n,n);
    A.setIdentity();
    return A;
  }

  /** 
   * 11. 
   * @brief Create an zero matrix of the requested size.
   * @param[in] n The dimension of the zero matrix.
   * @return Zero matrix of the required size.
   */
  static matrix_type zeros(int m, int n) { return matrix_type(m,n); }

  /** 
   * 15. 
   * @brief Extract the diagonal elements of a matrix.
   * @param[in] A The (square) matrix whose diagonal is to be extracted.
   * @param[out] B A diagonal matrix containing entries from A.
   */
  static void diag(const matrix_type& A,
                   matrix_type& B) {
    /* create a zeros matrix of the right dimension and assign to B */
    B = eye(A.rows());
    for (int i=0; i<A.rows(); ++i) B.valuePtr()[i] = A.diagonal()[i];
  }
};

} 

#endif /** EIGEN_MATRIX_ADAPATOR_HPP */
