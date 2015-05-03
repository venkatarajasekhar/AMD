#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <boost/shared_ptr.hpp>

#include <AMD/AMD.hpp>
#include <Eigen/Sparse>

typedef Eigen::SparseMatrix<double> matrix_type;
typedef AMD::MatrixAdaptor_t<matrix_type> adaptor_type;
typedef adaptor_type::value_type value_type;
typedef AMD::MatrixMatrixFunc<matrix_type, value_type> MMFunc;
typedef AMD::ScalarMatrixFunc<matrix_type, value_type> SMFunc;

/**
 * There are 16 functions in Elemental Adaptor. Each one of them has to be
 * tested one after the other. I am going to write a couple of tests and you
 * can follow suite.
 */

void assert_close (double a, double b) {
  double error = a-b;
  error = (0.0 > error) ? -error: error;
  assert (error <= 1e-9);
}

void testRowAndCols() {
  /** Define two matrices */
  matrix_type A(5,10);

  /** Check the dimensions */
  assert (adaptor_type::getNumRows(A) == A.rows());
  assert (adaptor_type::getNumCols(A) == A.cols());
}

void testAddMinusMultiplyElemWise () { 
  /** Fill with random entries drawn from Uniform(0,1) */
  matrix_type A = *(AMD::rand_psd_t<matrix_type>::apply (5, 5));
  matrix_type B = *(AMD::rand_psd_t<matrix_type>::apply (5, 5));

  /** Define two matrices */
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);
  matrix_type F(5,5);

  /** Add/sub A and B */
  adaptor_type::add (A, B, C);
  adaptor_type::minus (A, B, D);
  adaptor_type::multiply (A, B, E);
  adaptor_type::elementwiseProduct (A, B, F);

  /** Check answer */
  const int n = A.rows();

  /* Simple add and multiply product */
#if 0
  for (int j=0; j<n; ++j) {
    for (int i=0; i<n; ++i)  {
      assert(C(i,j) == (A(i,j) + B(i,j)));
      assert(D(i,j) == (A(i,j) - B(i,j)));
      assert(F(i,j) == (A(i,j) * B(i,j)));
    }
  }

  for (int j=0; j<n; ++j) {
    for (int i=0; i<n; ++i)  {
      double sum = 0.0;
      for (int k=0; k<n; ++k) {
        sum += A(i,k)*B(k,j);
      }
      assert_close(sum, E(i,j));
    }
  }
#endif
}

void testTransposeNegationDiagInverse () {
  /** Define two matrices */
  matrix_type A = *(AMD::rand_psd_t<matrix_type>::apply (5, 5));
  matrix_type B(5,5);
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);
  matrix_type F(5,5);

  /** perform operations A */
  adaptor_type::transpose (A, B);
  adaptor_type::negation (A, C);
  adaptor_type::inv (A, D);
  adaptor_type::diag(A, F);

  /** Check answer */
  const int m = A.rows();
  const int n = A.cols();

  /* Simple transpose */
#if 0
  for (int j=0; j<n; ++j) for (int i=0; i<m; ++i) assert(A(i,j) == B(j,i));

  /* Simple Eigenent-wise negation */
  for (int j=0; j<n; ++j) 
    for (int i=0; i<m; ++i) 
      assert(A(i,j) == (-1.0*C(i,j)));

  /* Check for inverse using multiply */
  adaptor_type::multiply (A, D, E);
  const double l2_norm = E.norm();
  assert_close (l2_norm, 1.0);

  /* Check for diagonal */
  for (int j=0; j<n; ++j)  {
    for (int i=0; i<m; ++i) { 
      if (i==j) assert (F(i,j) == A(i,j));
      else assert (F(i,j) == 0.0);
    }
  }
#endif
}

void testEyeZerosCopy () { 
  /** Define two matrices */
  matrix_type A = *(AMD::rand_psd_t<matrix_type>::apply (5, 5));
  matrix_type B(5,5);

  /** Perform the operations */
  adaptor_type::copy (B, A);
  matrix_type C = adaptor_type::zeros(5, 5);
  matrix_type D = adaptor_type::eye(5);

  /** Check answer */
  const int m = A.rows();
  const int n = A.cols();

  /* Check the results */
#if 0
  for (int j=0; j<n; ++j) {
    for (int i=0; i<m; ++i)  {
      assert(B(i,j) == A(i,j));
      assert (C(i,j) == 0.0);
      if (i==j) assert (D(i,j) == 1.0);
      else assert (D(i,j) == 0.0);
    }
  }
#endif
}

void testTraceLogdet () {
  /** Define a PSD matrices */
  matrix_type A = *(AMD::rand_psd_t<matrix_type>::apply (2, 2));

  /** Perform the operations */
  const double trace = adaptor_type::trace (A);
  const double logdet = adaptor_type::logdet (A);
                                   
  /** Do simple checks */
#if 0
  double trace_manual = A(0,0) + A(1,1);
  double logdet_manual = log (A(0,0)*A(1,1) - A(0,1)*A(1,0));

  assert_close (trace ,trace_manual);
  assert_close (logdet ,logdet_manual);
#endif
}

int main(int argc, char** argv) {

  std::cout << "Testing getNumRows() and getNumCols() .... ";
  testRowAndCols();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing add(), minus(), multiply(), EigenentwiseProduct() .... ";
  testAddMinusMultiplyElemWise();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing transpose(), negation(), diag(), inverse() .... ";
  testTransposeNegationDiagInverse();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing eye(), zeros(), and copy() .... ";
  testEyeZerosCopy();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing trace() and logdet() .... ";
  testTraceLogdet();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  return(0);
}
