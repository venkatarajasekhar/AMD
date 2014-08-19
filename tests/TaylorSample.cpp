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



/** An sample for taylor expansion of logdet(X). */
void taylorSample() {

  std::string ans;
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;
  // Initialize the matrices.
  symbolic_matrix_type X("X", ROW, COL);
  symbolic_matrix_type X0("X0", ROW, COL);
  symbolic_matrix_type Delta("(X-X0)", ROW, COL);
    
  AMD::SymbolicScalarMatlab a2("1/2!");
  AMD::SymbolicScalarMatlab a3("1/3!"); 
  SymbolicSMFunc r2(a2,ROW,COL);
  SymbolicSMFunc r3(a3,ROW, COL);

  // Initialize MatrixMatrixFunction. 
  SymbolicMMFunc fX(X, false);
  SymbolicMMFunc fX0(X0, false);
  SymbolicMMFunc fDelta(Delta, true);

  // Compute Taylor series iteratively. 
  SymbolicSMFunc f0 =  logdet(fX0);
  SymbolicSMFunc f1 = trace(fDelta * transpose(*f0.derivativeFuncVal));
  SymbolicSMFunc f2 = trace(fDelta * transpose(*f1.derivativeFuncVal));
  SymbolicSMFunc f3 = trace(fDelta * transpose(*f2.derivativeFuncVal));
  // Taylor Expansion. 
  SymbolicSMFunc func = f0 + f1 + r2*f2 + r3*f3;

  std::cout<<"The first 4 terms of Taylor Expansion for logdet(X) around X0 is:";
  std::cout << std::endl;
  std::cout << func.functionVal.getString() << std::endl;

}



int main(int argc, char** argv) {
  // Test taylor expansion.
  taylorSample();
  return(0);
}
