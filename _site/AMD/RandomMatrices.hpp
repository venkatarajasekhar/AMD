#ifndef AMD_RANDOM_MATRICES_HPP
#define AMD_RANDOM_MATRICES_HPP

#include <AMD/config.h>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <fstream>
#include <vector>

#include "utility.hpp"
#include "MatrixVisitor.hpp"

namespace AMD {

template <typename MatrixType>
struct rand_psd_t {
  /**
   * Define apply function thusly.
   * static void apply (int);
   */ 
};

#if AMD_HAVE_ELEMENTAL==1

#include <elemental.hpp>

template <>
struct rand_psd_t<elem::DistMatrix<double> > {
  typedef elem::DistMatrix<double> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;

  /** A functor that adds n to each diagonal element */
  struct nI_plus_A_t {
    double n;

    nI_plus_A_t (double n) : n(n) {}

    void apply (MatrixPtrType A, 
                int i, 
                int j, 
                int i_lcl, 
                int j_lcl) {
      if (i==j) A->SetLocal(i_lcl,j_lcl,A->GetLocal(i_lcl,j_lcl)+n);
    }
  };


  static MatrixPtrType apply(int n) {
    /** 1. Generate a uniformly random ([0,1]) matrix A */
    MatrixPtrType A = MatrixPtrType(new MatrixType(n,n));
    MatrixPtrType A_trans = MatrixPtrType(new MatrixType(n,n));
    elem::MakeUniform(*A, 0., 1.);

    /** 2. Compute A = A+A^T, which makes A symmetric  */
    elem::Transpose (*A, *A_trans);
    elem::Axpy(1.0, *A_trans, *A);

    /** 3. As A(i,j) is originally (0,1), we can add n*I to make it PSD */ 
    nI_plus_A_t nI_plus_A(n);
    matrix_visitor_t<MatrixType,nI_plus_A_t>::apply(A, nI_plus_A);

    return A;
  }
};

#endif /** AMD_HAVE_ELEMENTAL==1 */

#if AMD_HAVE_EIGEN==1

template <>
struct rand_psd_t<Eigen::Matrix<double, 
                                Eigen::Dynamic, 
                                Eigen::Dynamic> > {
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;
  typedef boost::mt19937 engine_type;
  typedef boost::uniform_01<double> uni_real_type;
  typedef boost::variate_generator<engine_type, 
                                   uni_real_type> uni_real_prng_type;

  static MatrixPtrType apply(int n,
                             int seed=-1) {

    /** 0. If there is no good seed given, use a good random seed */
    if (0>seed) seed = detail::generate_seed();

    /** 1. Generate a uniformly random ([0,1]) matrix A */
    /** 
     * Note: There is a random function (MatrixXd::Random(m,n), but I don't
     * know what kind of random numbers they are using. So, we will use
     * boost::random to fill in the entries. Change it later when we 
     * understand * Random() better.
     */
    uni_real_prng_type prng ((engine_type(seed)), (uni_real_type()));
    MatrixPtrType A = MatrixPtrType(new MatrixType(n,n));
    for (int j=0; j<n; ++j) { 
      for (int i=0; i<n; ++i) { 
        A->operator()(i,j) = prng(); 
      }
    }
    MatrixPtrType A_trans = MatrixPtrType(new MatrixType(A->transpose()));

    /** 2. Compute A = A+A^T, which makes A symmetric  */
    (*A) += (*A_trans);

    /** 3. As A(i,j) is originally (0,1), we can add n*I to make it PSD */ 
    for (int i=0; i<A->rows(); ++i) A->operator()(i,i) += n;

    return A;
  }
};

template <>
struct rand_psd_t<Eigen::SparseMatrix<double> > {
  typedef Eigen::SparseMatrix<double> SparseMatrixType;
  typedef boost::shared_ptr<SparseMatrixType> SparseMatrixPtrType;
  typedef Eigen::Matrix
         <double, Eigen::Dynamic, Eigen::Dynamic> DenseMatrixType;
  typedef boost::shared_ptr<DenseMatrixType> DenseMatrixPtrType;
  typedef boost::mt19937 engine_type;
  typedef boost::bernoulli_distribution<double> bernoulli_real_type;
  typedef boost::variate_generator
      <engine_type, bernoulli_real_type> bernoulli_real_prng_type;
  typedef Eigen::Triplet<double> TripletType;

  static SparseMatrixPtrType apply(int n, int nnz, int seed=-1) {

    /** If there is no good seed given, use a good random seed */
    if (0>seed) seed = detail::generate_seed();

    /** 0. Generate the dense matrix */
    DenseMatrixPtrType A_dense = rand_psd_t<DenseMatrixType>::apply(n, seed);

    /**
     * 1. We will generate a sparse matrix in the following way:
     *  - Leave the diagonal as is --- this takes care of n entries. So we 
     *  have *  (n-nnz) non-zero entries left. This means that we have 
     *  n*n-n+nnz entries to zero out. 
     *  - Since we are dealing with symmetric matrices, exactly half of these
     *  entries should be in the UPPER triangle and then we can simply reflect
     *  everything about the diagonal. 
     *  - Go through each element in the UPPER triangle and choose to keep it
     *  with a probability of (nnz)/(n*n-n).
     *
     *    Caveat: 
     *    All this assumes the following: (nnz>n and nnz<(n*n))
     */
    if (nnz<n) {
      /** error */
    } else if (nnz>(n*n)) {
      /** error */
    }

    const double p = static_cast<double>(nnz-n) / 
                     static_cast<double>((n*n) - n);
    int true_nnz = n;
    bernoulli_real_prng_type prng ((engine_type(seed)), 
                                   (bernoulli_real_type(p)));
    std::vector<TripletType> nnz_entries;

    for (int j=0; j<n; ++j) {
      for (int i=j; i<n; ++i) { 
        if (i==j) nnz_entries.push_back
                    (TripletType(i,j,A_dense->operator()(i,j)));
        else if (prng()) {
          nnz_entries.push_back(TripletType(i,j,A_dense->operator()(i,j)));
          nnz_entries.push_back(TripletType(j,i,A_dense->operator()(i,j)));
          ++true_nnz;
          ++true_nnz;
        } else {
          /** zeroed out */
        }
      }
    }

    /** 
     * Now, simply create the SparseMatrix that we need from this DenseMatrix 
     */
    SparseMatrixPtrType A(new SparseMatrixType(n,n));
    A->setFromTriplets(nnz_entries.begin(), nnz_entries.end());
    A->makeCompressed();

    return A;
  }
};

#endif /** AMD_HAVE_EIGEN==1 */

} /** namespace AMD */

#endif /** AMD_RANDOM_MATRICES_HPP */
