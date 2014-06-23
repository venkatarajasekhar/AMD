#include <iostream>
#include <string>
#include <cstdio>
#include <assert.h>
#include <AMD/AMD.hpp>
#include "boost/shared_ptr.hpp"

typedef AMD::MatrixMatrixFunc<AMD::SymbolicMatrixMatlab,
                      				AMD::SymbolicScalarMatlab> MMFunc;
typedef AMD::ScalarMatrixFunc<AMD::SymbolicMatrixMatlab,
                       				AMD::SymbolicScalarMatlab> SMFunc;

/** Complete writing out the descriptions */
/** Also, change the name of the constant to be A, instead of Y */
void testMatrixMatrixFunc() {
  std::string ans;

  /** Create a variable X and an identity function */
  AMD::SymbolicMatrixMatlab X("X",3,3);
  MMFunc fx(X,false); 

  /** Create a constant A and an identity function */
  AMD::SymbolicMatrixMatlab Y("Y",3,3);
  MMFunc fy(Y,true); 

  /** Create a scalar-matrix function placeholder */ 
  SMFunc func;

  /** test out the derivative of trace(AX) -- should be A' */
  ans = "Y'";
  func = trace(fx*fy);
  assert(func.derivativeVal.getString()==ans);

  // d/dX trace(X^T*Y^T)=Y^T
  func = trace(transpose(fx)*transpose(fy));
  assert(func.derivativeVal.getString()==ans);

  // d/dX trace((X*Y)^T)=Y^T
  func = trace(transpose(fx*fy));
  assert(func.derivativeVal.getString()==ans);

  ans = "Y";
  // d/dX trace(X*Y^T) = Y
  func = trace(fx*transpose(fy));
  assert(func.derivativeVal.getString()==ans);

  // d/dX trace(Y*X^T) = Y
  func = trace(fy*transpose(fx));
  assert(func.derivativeVal.getString()==ans);

  ans = "eye(3)";
  // d/dX trace(X) = I
  func = trace(fx);
  assert(func.derivativeVal.getString()==ans);

  // d/dX trace(Y+X^T+Y) = I
  func = trace(fy+transpose(fx)+fy);
  assert(func.derivativeVal.getString()==ans);

  func = trace(fy*inv(fx));
  ans = "(((-inv(X))*Y)*inv(X))'";
  assert(func.derivativeVal.getString()==ans);

  assert(func.derivativeVal.getString()==ans);
  func = trace(fy-fx);
  ans = "(-eye(3))";
  assert(func.derivativeVal.getString()==ans);

  func = logdet(fx);
  ans = "inv(X)'";
  assert(func.derivativeVal.getString()==ans);

  func = logdet(transpose(fx));
  assert(func.derivativeVal.getString()==ans);

  func = logdet(fy+fx);
  ans = "inv(Y+X)'";
  assert(func.derivativeVal.getString()==ans);

  func = logdet(fy-fx);
  ans = "(-inv(Y-X))'";
  assert(func.derivativeVal.getString()==ans);

  func = logdet(inv(transpose(fx)));
  ans = "(-inv(X)')";
  assert(func.derivativeVal.getString()==ans);

  std::cout << "d/dX " << func.functionVal.getString() 
  	    << " = " << func.derivativeVal.getString() << std::endl;

}


/** The function names need to be more descriptive; also add comments */
void testMatrixMatrixFunc3 () {

  std::string ans;
  AMD::SymbolicMatrixMatlab X("X", 2, 2);
  AMD::SymbolicMatrixMatlab A("A", 2, 2);
  AMD::SymbolicMatrixMatlab B("B", 2, 2);
  AMD::SymbolicMatrixMatlab C("C", 2, 2);
  MMFunc fX(X, false);
  MMFunc fA(A, true);
  MMFunc fB(B, true);
  MMFunc fC(C, true);

  MMFunc fBX = fB * fX;
  MMFunc fAX1 = fA * fX;
  MMFunc fAXB = fAX1 * fB;
  MMFunc fCX = fC * fX;
  MMFunc fDX = fBX + fCX;
  MMFunc fAXDX = fA * fDX;

  SMFunc func = trace(fBX);

  std::cout << "function" << std::endl;
  std::cout << func.functionVal.getString() << std::endl;
  std::cout << "derivative" << std::endl;
  std::cout << func.derivativeVal.getString() << std::endl;
  std::cout << "---- logdet tests ---- " << std::endl;
  SMFunc func4; 
  func4 = logdet(fX);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  func4 = logdet(fAXB);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  func4 = logdet(transpose(fX) * fA * fX);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  func4 = trace (fA*fX);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  func4 = trace (fA * fX * fB * fX);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  func4 = trace (fA * inv(fX) * fB);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
  // Element-wise Production test cases
  std::cout << "Now lets test some cases for element-wise product" << std::endl
   << std::endl;
  MMFunc eTest0 = AMD::elementwiseProd(fA, fX);
  func4 = trace (eTest0);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = trace(fA * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest1 = AMD::elementwiseProd(fX, fX);
  func4 = trace (eTest1);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest2 = AMD::elementwiseProd(AMD::elementwiseProd(fX, fX), fX);
  func4 = trace (eTest2);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
//  func4 = trace(fX * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  MMFunc eTest3 = AMD::elementwiseProd(inv(fX), inv(fX));
  func4 = trace (eTest3);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
//  func4 = trace(inv(fX) * inv(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest4 = AMD::elementwiseProd(fX, inv(fX));
  func4 = trace (eTest4);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
//  func4 = trace(fX * inv(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest5 = AMD::elementwiseProd(fA, fX);
  func4 = logdet (eTest5);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fA * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest6 = AMD::elementwiseProd(fA, transpose(fX));
  func4 = logdet (eTest6);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fA * transpose(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest7 = AMD::elementwiseProd(fX, fX);
  func4 = logdet (eTest7);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
//  func4 = logdet(fX * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest8 = AMD::elementwiseProd(fX, transpose(fX));
  func4 = logdet (eTest8);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fX * transpose(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
}

int main() {

  std::cout << "Testing basic matrix-matrix functions .... ";
  testMatrixMatrixFunc();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing advanced matrix-matrix functions .... ";
  testMatrixMatrixFunc3();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  return(0);
}
