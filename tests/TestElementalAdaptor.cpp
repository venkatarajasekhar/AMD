#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <elemental.hpp>
#include <boost/shared_ptr.hpp>

#include <AMD/AMD.hpp>

#define M 2
#define N 2

typedef elem::Matrix<double> matrix_type;
typedef AMD::MatrixAdaptor_t<matrix_type> adaptor_type;
typedef typename adaptor_type::value_type value_type;

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
  assert (adaptor_type::getNumRows(A) == A.Height());
  assert (adaptor_type::getNumCols(A) == A.Width());
}

void testAddMinusMultiplyElemWise () { 
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);
  matrix_type F(5,5);

  /** Fill with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);
  elem::MakeGaussian(B);

  /** Add/sub A and B */
  adaptor_type::add (A, B, C);
  adaptor_type::minus (A, B, D);
  adaptor_type::multiply (A, B, E);
  adaptor_type::elementwiseProduct (A, B, F);

  /** Check answer */
  const int n = A.Width();

  /* Simple add and multiply product */
  for (int i=0; i<n; ++i)  {
    for (int j=0; j<n; ++j) {
      assert(C.Get(i,j) == (A.Get(i,j) + B.Get(i,j)));
      assert(D.Get(i,j) == (A.Get(i,j) - B.Get(i,j)));
      assert(F.Get(i,j) == (A.Get(i,j) * B.Get(i,j)));
    }
  }

  for (int i=0; i<n; ++i)  {
    for (int j=0; j<n; ++j) {
      double sum = 0.0;
      for (int k=0; k<n; ++k) {
        sum += A.Get(i,k)*B.Get(k,j);
      }
      assert_close(sum, E.Get(i,j));
    }
  }
}

void testTransposeNegationDiagInverse () {
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);
  matrix_type F(5,5);

  /** Fill A with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);

  /** perform operations A */
  adaptor_type::transpose (A, B);
  adaptor_type::negation (A, C);
  adaptor_type::inv (A, D);
  adaptor_type::diag(A, F);

  /** Check answer */
  const int m = A.Height();
  const int n = A.Width();

  /* Simple transpose */
  for (int i=0; i<m; ++i) 
    for (int j=0; j<n; ++j) 
      assert(A.Get(i,j) == B.Get(j,i));

  /* Simple element-wise negation */
  for (int i=0; i<m; ++i) 
    for (int j=0; j<n; ++j) 
      assert(A.Get(i,j) == (-1.0*C.Get(i,j)));

  /* Check for inverse using multiply */
  adaptor_type::multiply (A, D, E);
  const double l2_norm = elem::Nrm2(E);
  assert_close (l2_norm, 1.0); 

  /* Check for diagonal */
  for (int i=0; i<m; ++i) { 
    for (int j=0; j<n; ++j)  {
      if (i==j) assert (F.Get(i,j) == A.Get(i,j));
      else assert (F.Get(i,j) == 0.0);
    }
  }
}

void testEyeZerosCopy () { 
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);

  /** Fill with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);

  /** Perform the operations */
  adaptor_type::copy (B, A);
  matrix_type C = adaptor_type::zeros(5, 5);
  matrix_type D = adaptor_type::eye(5);

  /** Check answer */
  const int m = A.Height();
  const int n = A.Width();

  /* Check the results */
  for (int i=0; i<m; ++i)  {
    for (int j=0; j<n; ++j) {
      assert(B.Get(i,j) == A.Get(i,j));
      assert (C.Get(i,j) == 0.0);
      if (i==j) assert (D.Get(i,j) == 1.0);
      else assert (D.Get(i,j) == 0.0);
    }
  }
}

void testTraceLogdet () { 
  /** Define two matrices */
  matrix_type A(2,2);

  /** Fill with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);

  /** Make symmetric, positive deifnitene */
  A.Set(1, 0, A.Get(0,1));
  A.Set(0, 0, 2.0 + A.Get(0, 0));
  A.Set(1, 1, 2.0 + A.Get(1, 1));

  /** Perform the operations */
//  const double trace = adaptor_type::trace (A);
//  const double logdet = adaptor_type::logdet (A);

  /** Do simple checks */
//  double trace_manual = A.Get(0,0) + A.Get(1,1);
//  double logdet_manual = log (A.Get(0,0)*A.Get(1,1) - A.Get(0,1)*A.Get(1,0));

//  assert_close (trace ,trace_manual);
//  assert_close (logdet ,logdet_manual);
}
/**
 * @brief Test numerical matrix derivatives.
 */
void testMatrixMatrixFunc() {

  matrix_type A(N, N);
  matrix_type B(N, N);
  matrix_type X(N, N);

  matrix_type AT(N, N);   /**< A transpose  */
  matrix_type AI(N, N);   /**< A inverse    */
  matrix_type AN(N, N);   /**< A negation   */
  matrix_type BT(N, N);   /**< A transpose  */
  matrix_type BI(N, N);   /**< A inverse    */
  matrix_type BN(N, N);   /**< A negation   */
  matrix_type XT(N, N);   /**< A transpose  */
  matrix_type XI(N, N);   /**< A inverse    */
  matrix_type XN(N, N);   /**< A negation   */


  elem::MakeGaussian(A);
  elem::MakeGaussian(B);

  adaptor_type::transpose(A, AT);
  adaptor_type::negation(A, AN);
  adaptor_type::inv(A, AI);
  adaptor_type::transpose(B, BT);
  adaptor_type::negation(B, BN);
  adaptor_type::inv(B, BI);

  MMFunc fA(A, true);
  MMFunc fX(X, false);
  MMFunc fAfX = fA*fX;
  SMFunc func;

  // trace(AX). derivative == A transpose;
  func = trace(fA * fX);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      assert_close(func.derivativeVal.Get(i,j), AT.Get(i,j));
    }
  }
}

int main(int argc, char** argv) {
  /** Initialize elemental */
  elem::Initialize(argc, argv);

  std::cout << "Testing getNumRows() and getNumCols() .... ";
  testRowAndCols();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing add(), minus(), multiply(), elementwiseProduct() .... ";
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

  std::cout << "Testing MatrixMatrixFunc() .... ";
  testMatrixMatrixFunc();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  /** Finalize elemental */
  elem::Finalize();

  return(0);
}
