#ifndef AMD_RANDOM_MATRICES_HPP
#define AMD_RANDOM_MATRICES_HPP

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <fstream>
#include <vector>

#include <fstream>
#include <iostream>

#include <AMD/log.hpp>

#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace {
  static unsigned int generate_seed () {
    unsigned int r_int;
    LOG_DEBUG << "Generating random seed";
  
    #if 0
    std::ifstream rf("/dev/random", std::ios::binary);
    if(rf.is_open())rf.read(reinterpret_cast<char*>(&r_int),sizeof(r_int));
    else r_int = std::time(NULL);
    #else
    r_int = std::time(NULL);
    #endif
  
    LOG_DEBUG << "DONE (" << r_int << ")";
    return r_int;
  }
}

namespace AMD {

template <typename MatrixType>
struct rand_psd_t {
  /**
   * Define apply function thusly.
   * static void apply (int64_t);
   */ 
};

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

  static MatrixPtrType apply(int64_t n,
                             int seed=-1) {

    /** 0. If there is no good seed given, use a good random seed */
    if (0>seed) seed = generate_seed();

    /** 1. Generate a uniformly random ([0,1]) matrix A */
    /** 
     * Note: There is a random function (MatrixXd::Random(m,n), but I don't
     * know what kind of random numbers they are using. So, we will use
     * boost::random to fill in the entries. Change it later when we understand
     * Random() better.
     */
    LOG_DEBUG << "Creating random dense matrix";

    MatrixPtrType A = MatrixPtrType(new MatrixType(n,n));
    uni_real_prng_type prng(engine_type(seed), (uni_real_type()));
    for (int64_t j=0; j<n; ++j) { 
      for (int64_t i=0; i<n; ++i) {
        A->operator()(i,j) = prng(); 
      }
    }

    LOG_DEBUG << "DONE ";
    LOG_DEBUG << "Computing (A'+A)/2 for symmetricity";

    MatrixPtrType A_trans = MatrixPtrType(new MatrixType(A->transpose()));

    /** 2. Compute A = A+A^T, which makes A symmetric  */
    (*A) += (*A_trans);

    LOG_DEBUG << "DONE";
    LOG_DEBUG << "Making diagonally dominant";

    /** 3. As A(i,j) is originally (0,1), we can add n*I to make it PSD */ 
    for (int64_t i=0; i<A->rows(); ++i) A->operator()(i,i) += n;
    LOG_DEBUG << "DONE";

    return A;
  }
};

template <>
struct rand_psd_t<Eigen::SparseMatrix<double> > {
  typedef Eigen::SparseMatrix<double> SparseMatrixType;
  typedef boost::shared_ptr<SparseMatrixType> SparseMatrixPtrType;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> DenseMatrixType;
  typedef boost::shared_ptr<DenseMatrixType> DenseMatrixPtrType;
  typedef boost::uniform_01<double> uni_real_type;
  typedef boost::mt19937 engine_type;
  typedef boost::variate_generator<engine_type, 
                                   uni_real_type> uni_real_prng_type;
  typedef boost::bernoulli_distribution<double> bernoulli_real_type;
  typedef boost::variate_generator<engine_type, bernoulli_real_type> 
                                                    bernoulli_real_prng_type;

  typedef Eigen::Triplet<double> TripletType;

  static SparseMatrixPtrType apply(int64_t n, int64_t nnz, int seed=-1) {

    /** If there is no good seed given, use a good random seed */
    if (0>seed) seed = generate_seed();

    /**
     * 1. We will generate a sparse matrix in the following way:
     *  - All the diagonal entries are non-zero --- this accounts for n nnz.
     *  - Since we are dealing with symmetric matrices, exactly half of these
     *    entries should be in the UPPER triangle and then we can simply
     *    reflect everything about the diagonal. I.e., we need to figure out 
     *    where to place the remaining (nnz-n)/2 entries. There are (n*n-n)/2
     *    possible slots for these in the UPPER triangle.
     *  - Go through each element in the UPPER triangle and choose to keep it
     *    with a probability of (nnz-n)/(n*n-n).
     *
     *    Caveat: 
     *    All this assumes the following: (nnz>n and nnz<(n*n))
     */
    if (nnz<n) {
      /** error */
      throw std::runtime_error ("NNZ < N --- this case is not supported");
    } else if (nnz>(n*n)) {
      /** error */
      throw std::runtime_error ("NNZ > N^2 --- this is an error");
    }

    LOG_DEBUG << "Creating random sparse SPD triplets";

    const double p = static_cast<double>(nnz-n)/static_cast<double>((n*n)-n);
    uni_real_prng_type val_prng ((engine_type(seed)), (uni_real_type()));
    bernoulli_real_prng_type toss_prng ((engine_type(seed)),
                                        (bernoulli_real_type(p)));
    std::vector<TripletType> nnz_entries;

    /** First push back entries corresponding to the diagonal elements */
    int64_t true_nnz = n;

    /** 
     * Now, we have to figure out where to put (nnz-n)/2 entries. Each entry
     * lies at ([0,n),[0,n)); so simply choose a random number for each of 
     * the coordinates. If there happens to be an entry there already, 
     * hopefully things get added up.
     */
    for (int64_t j=0; j<n; ++j) {
      for (int64_t i=j; i<n; ++i) { 
        if (i==j) {
          nnz_entries.push_back(TripletType(i,j,n+val_prng()));
        } else if (toss_prng()) {
          const double val = val_prng();
          nnz_entries.push_back(TripletType(i,j,val));
          nnz_entries.push_back(TripletType(j,i,val));
          true_nnz += 2;
        } else {
          /** zeroed out */
        }
      }
    }

    LOG_DEBUG << "DONE";
    LOG_DEBUG << "Creating sparse matrix from triplets and compressing";
    
    /** Now, simply create the SparseMatrix that we need from this DenseMatrix*/
    SparseMatrixPtrType A(new SparseMatrixType(n,n));
    A->setFromTriplets(nnz_entries.begin(), nnz_entries.end());
    A->makeCompressed();

    LOG_DEBUG << "DONE";

    return A;
  }
};

} // namespace AMD

#endif /** AMD_RANDOM_MATRICES_HPP */
