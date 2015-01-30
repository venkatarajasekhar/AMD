#ifndef AMD_MATRIX_VISITOR_HPP
#define AMD_MATRIX_VISITOR_HPP

#include <AMD/config.h>
#include <boost/shared_ptr.hpp>

namespace AMD {

/**
 * There are lot of operations where we have to visit each individual element
 * of a distributed matrix. This visitor is a uniform means of achieving that.
 */ 
template <typename MatrixType,
          typename FunctorType>
struct matrix_visitor_t {
  /** 
   * Provide apply method
   * void apply(boost::shared_ptr<MatrixType>, FunctorType&);
   */ 
};

#if AMD_HAVE_ELEMENTAL==1

#include <elemental.hpp>

template <typename FunctorType>
struct matrix_visitor_t <elem::DistMatrix<double>, FunctorType> {
  typedef elem::DistMatrix<double> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;

  static void apply (MatrixPtrType A, 
                     FunctorType& functor) {
    const int j_max = A->LocalWidth();
    const int i_max = A->LocalHeight();
    const int r_shift = A->RowShift();
    const int r_stride = A->RowStride();
    const int c_shift = A->ColShift();
    const int c_stride = A->ColStride();
    for (int j_lcl=0; j_lcl<j_max; ++j_lcl) {
      const int j = r_shift + r_stride*j_lcl;
      for(int i_lcl=0; i_lcl<i_max; ++i_lcl) {
        const int i = c_shift + c_stride*i_lcl;
        functor.apply (A, i, j, i_lcl, j_lcl);
      }
    }
  }
};

#endif /** AMD_HAVE_ELEMENTAL */

#if AMD_HAVE_EIGEN==1

#include <Eigen/Dense>
#include <Eigen/Sparse>

template <typename FunctorType>
struct matrix_visitor_t <Eigen::Matrix<double,
                                       Eigen::Dynamic,
                                       Eigen::Dynamic>, 
                         FunctorType> {
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;

  static void apply (MatrixPtrType A, 
                     FunctorType& functor) {
    const int m = A->rows();
    const int n = A->cols();
    for (int j=0; j<n; ++j) 
      for (int i=0; i<m; ++i) 
        functor.apply (A, i, j);
  }
};

template <typename FunctorType>
struct matrix_visitor_t <Eigen::Matrix<double,
                                       Eigen::Dynamic,
                                       1/*vector*/>, 
                         FunctorType> {
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorType;
  typedef boost::shared_ptr<VectorType> VectorPtrType;

  static void apply (VectorPtrType x, 
                     FunctorType& functor) {
    const int m = x->rows();
    for (int i=0; i<m; ++i) functor.apply (x, i);
  }
};

#endif /** AMD_HAVE_EIGEN */

}

#endif /** AMD_MATRIX_VISITOR_HPP */
