#include <iostream>
#include <fstream>
#include <functional>
#include <cstring>
#include <vector>
#include <utility>
#include <cstdio>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#ifdef OPENMP
# include <omp.h>
#endif

#include <AMD/log.hpp>
#include <AMD/load.hpp>
#include <AMD/randommatrices.hpp>

#include "options.hpp"

/** Create somconvenient typedefs */
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> DenseMatrixType;
typedef boost::shared_ptr<DenseMatrixType> DenseMatrixPtrType;

typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef boost::shared_ptr<SparseMatrixType> SparseMatrixPtrType;
typedef Eigen::Triplet<double> IJVTripletType;

int main (int argc, char** argv) {
  /***********************************************************************/

  /* parse everything */
  options_t program_opts (argc, argv, true/**this is the root*/);
  if (program_opts.exit_on_return) { return -1; }
  if (1<program_opts.verbosity) program_opts.pretty_print(/**always root*/true);

  /***********************************************************************/

  /** Set the logger verbosity */
  AMD::setLogLevel(program_opts.verbosity);

  /***********************************************************************/

  /** Initialize eigen parallelism */
  Eigen::initParallel();

  /***********************************************************************/

  /** Do all the processing in functions that are specific sparsity */
  if (0<program_opts.nnz ||
      program_opts.input_fileformat.find("SPARSE") != std::string::npos) {
    /** Get the matrices one way or the other */
    SparseMatrixPtrType A;
    if (program_opts.use_random) {
      LOG_DEBUG << "Using a random matrix for the experiment";
      A = AMD::rand_psd_t<SparseMatrixType>::apply(program_opts.n,
                                                   program_opts.nnz);
    } else {
      LOG_DEBUG << "Using a sparse matrix for the experiment";
      A = AMD::load_sparse (program_opts.input_filename,
                            program_opts.input_fileformat,
                            program_opts.n,
                            program_opts.nnz);
    }

    if (program_opts.gen_and_dump) std::cout << *A;

  } else {
    /** Get the matrices one way or the other */
    DenseMatrixPtrType A;
    if (program_opts.use_random) {
      LOG_DEBUG << "Using a random matrix for the experiment";
      A = AMD::rand_psd_t<DenseMatrixType>::apply(program_opts.n);
    } else {
      LOG_DEBUG << "Using a dense matrix for the experiment";
      A = AMD::load_dense (program_opts.input_filename,
                           program_opts.input_fileformat,
                           program_opts.n);
    }

    if (program_opts.gen_and_dump) std::cout << *A;

  }

  /***********************************************************************/
}
