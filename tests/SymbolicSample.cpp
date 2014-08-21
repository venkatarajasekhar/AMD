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

#define ROW 128 
#define COL 128 

// Typedef for SymbolicMatrixMatlab and SymbolicScalarMatlab.
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef symbolic_adaptor_type::value_type symbolic_value_type;

typedef AMD::MatrixMatrixFunc<symbolic_matrix_type,
                      				symbolic_value_type> SymbolicMMFunc;
typedef AMD::ScalarMatrixFunc<symbolic_matrix_type,
                       				symbolic_value_type> SymbolicSMFunc;


void symbolicSample() {

  std::string ans;
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;
  /** Create a variable X and an identity function */
  symbolic_matrix_type X("X", ROW, COL);
  SymbolicMMFunc fX(X, false);

  /** Create constants A,B and C and identity functions */
  symbolic_matrix_type A("A", ROW, COL);
  SymbolicMMFunc fA(A, true);

  /** Scalar-matrix function placeholder */
  SymbolicSMFunc func; 

  /** Create the scalar-matrix function. */
  func = trace (fA * inv(fX));
  
  /** Output the function value and derivative value. */
  std::cout << "Function Value:   "<<func.functionVal.getString()<<std::endl;
  std::cout << "Derivative Value: "<<func.derivativeVal.getString()<<std::endl;
  
 
  
}


int main(int argc, char** argv) {
  // Sample of symblic scalar-matrix function usage.
  symbolicSample();


  return(0);
}
