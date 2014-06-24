#include <iostream>
#include <string>
#include <assert.h>
#include <elemental.hpp>
#include <boost/shared_ptr.hpp>

#include <AMD/AMD.hpp>

typedef elem::Matrix<double> matrix_type;
typedef AMD::MatrixAdaptor_t<matrix_type> adaptor_type;
typedef typename adaptor_type::value_type value_type;

/**
 * There are 16 functions in Elemental Adaptor. Each one of them has to be
 * tested one after the other. I am going to write a couple of tests and you
 * can follow suite.
 */

void testRowAndCols() {
  /** Define two matrices */
  matrix_type A(5,10);

  /** Check the dimensions */
  assert (adaptor_type::getNumRows() == A.Height());
  assert (adaptor_type::getNumCols() == A.Width());
}

void testAddMinusMultiply () { 
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);

  /** Fill with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);
  elem::MakeGaussian(B);

  /** Add/sub A and B */
  adaptor_type::add (A, B, C);
  adaptor_type::minus (A, B, D);
  adaptor_type::multiply (A, B, E);

  /** Check answer */
  const int m = A.Height();
  const int n = A.Width();

  /* Simple add and multiply product */
  for (int i=0; i<m; ++i)  {
    for (int j=0; j<n; ++j) {
      assert(C.Get(i,j) == (A.Get(i,j) + B.Get(i,j)));
      assert(D.Get(i,j) == (A.Get(i,j) - B.Get(i,j)));
    }
  }

  /* TODO: Suyang check multiply more efficiently */
  for (int i=0; i<m; ++i)  {
    for (int j=0; j<n; ++j) {
      double sum = 0.0;
      for (int k=0; k<n; ++k) {
        sum += A.Get(i,k)*B.Get(k,j);
      }
      assert (sum == C(i,j));
    }
  }
}

void testTransposeNegationInverse () {
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);
  matrix_type C(5,5);
  matrix_type D(5,5);
  matrix_type E(5,5);

  /** Fill A with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);

  /** perform operations A */
  adaptor_type::transpose (A, B);
  adaptor_type::negation (A, C);
  adaptor_type::inv (A, D);

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
  double error = (l2_norm - 1.0); 
  error = (0.0 > error) ? -error: error;
  assert (error <= 1e-9);
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

int main(int argc, char** argv) {
  /** Initialize elemental */
  elem::Initialize(argc, argv);

  std::cout << "Testing getNumRows() and getNumCols() .... ";
  testRowAndCols();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing add(), minus(), and multiply() .... ";
  testAddMinusMultiply();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing transpose(), negation(), and inverse() .... ";
  testTransposeNegationInverse();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing eye(), zeros(), and copy() .... ";
  testEyeZerosCopy();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  /** Finalize elemental */
  elem::Finalize();

  return(0);
}
