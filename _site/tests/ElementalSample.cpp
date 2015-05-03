#include <iostream>
#include <string>
#include <cmath>
#if AMD_HAVE_ELEMENTAL
#include <elemental.hpp>
#endif
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include <AMD/AMD.hpp>
#include <stack>

#define DEBUG

#define ROW 4
#define COL 4 

#if AMD_HAVE_ELEMENTAL

/** Typedef for Elemental Matrices. */
typedef elem::Matrix<double> elemental_matrix_type;
typedef AMD::MatrixAdaptor_t<elemental_matrix_type> elemental_adaptor_type;
typedef typename elemental_adaptor_type::value_type elemental_value_type;

typedef AMD::MatrixMatrixFunc<elemental_matrix_type,
                      				elemental_value_type> ElementalMMFunc;
typedef AMD::ScalarMatrixFunc<elemental_matrix_type,
                       				elemental_value_type> ElementalSMFunc;


/**
 * @brief An example how to use elemental matrix.
 */
void elementalSample() {
  
  /** Create constant matrices A, B, C, E etc */
  elemental_matrix_type A(ROW, COL);
  elemental_matrix_type X(ROW, COL);
 
  /** Initialize matrices */
  elem::MakeGaussian(A);
  elem::MakeGaussian(X);
  

  /** Create elementa matrix-matrix functions */ 
  ElementalMMFunc fA(A, true);
  ElementalMMFunc fX(X, false);
  ElementalMMFunc fAX = fA*fX;

  /** Elemental scalar-matrix function */
  ElementalSMFunc func;
  /** Create elemental scalar-matrix function. */ 
  func = trace(fA*fX);

  /** Output results. */
  int i, j;
  std::cout << std::endl;
  std::cout << "Matrix A: " << std::endl;
  for (i = 0; i < elemental_adaptor_type::getNumRows(*fA.matrixPtr); i++) {
    for (j = 0; j < elemental_adaptor_type::getNumCols(*fA.matrixPtr); j++) {
      std::cout << fA.matrixPtr->Get(i, j) << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Matrix X: " << std::endl;
  for (i = 0; i < elemental_adaptor_type::getNumRows(*fX.matrixPtr); i++) {
    for (j = 0; j < elemental_adaptor_type::getNumCols(*fX.matrixPtr); j++) {
      std::cout << fX.matrixPtr->Get(i, j) << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Matrix A*X: " << std::endl;
  for (i = 0; i < elemental_adaptor_type::getNumRows(*fAX.matrixPtr); i++) {
    for (j = 0; j < elemental_adaptor_type::getNumCols(*fAX.matrixPtr); j++) {
      std::cout << fAX.matrixPtr->Get(i, j) << "  ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "d/dX(trace(A*X)): " << std::endl;

  for (i = 0; i < elemental_adaptor_type::getNumRows(func.derivativeVal); i++) {
    for (j = 0; j < elemental_adaptor_type::getNumCols(func.derivativeVal); j++) {
      std::cout << func.derivativeVal.Get(i, j) << "  ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;

  std::cout << "d/dX(trace(A*X)) = A'" <<std::endl;
  

}

#endif // AMD_HAVE_ELEMENTAL


int main(int argc, char** argv) {
  std::cout << std::endl;
#if AMD_HAVE_ELEMENTAL
  elem::Initialize(argc, argv); 
  elementalSample();
  elem::Finalize();
#endif

  return(0);
}
