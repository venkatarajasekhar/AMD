#include <iostream>
#include <string>
#include <cstdio>
#include <assert.h>
#include <AMD/AMD.hpp>
#include "boost/shared_ptr.hpp"

void testSymbolicScalarMatlab() {
  AMD::SymbolicScalarMatlab a("a");
  AMD::SymbolicScalarMatlab b("b");
  AMD::SymbolicScalarMatlab c("c");
  AMD::SymbolicScalarMatlab d = AMD::sqrt(a+b*a/c-c);
  std::string ans = std::string("sqrt((a+((b*a)/c))-c)");
  assert( d.getString() == ans );
  AMD::SymbolicScalarMatlab e(1.2);
  ans = std::string("1.2");
  assert(e.getString() == ans);
}

void testSymbolicMatrixMatlab() {
  AMD::SymbolicMatrixMatlab a("A");
  AMD::SymbolicMatrixMatlab b("B");
  AMD::SymbolicMatrixMatlab c("C");
  AMD::SymbolicMatrixMatlab d = AMD::inv(a+b*(AMD::transpose(a)-c));
  std::string ans = std::string("inv(A+(B*(A'-C)))");
  assert(d.getString() == ans);

  AMD::SymbolicScalarMatlab e = AMD::trace(AMD::transpose(a+b));
  ans = "trace((A+B)')";
  assert(e.getString() == ans);

  e = AMD::logdet(a-b*c);
  ans = "log(det(A-(B*C)))";
  assert(e.getString() == ans);

  e = AMD::fnorm(a+AMD::elementwiseProd(b,c));
  ans = "norm(A+(B.*C),'fro')";
  assert( e.getString() == ans );

  AMD::SymbolicScalarMatlab f("f");
  b = f*a+a/f;
  ans = "((f.*A)+(A./f))";
  assert(b.getString() == ans);

  b = a*f;
  ans = "(f.*A)";
  assert(b.getString() == ans);

  // check sizes
  AMD::SymbolicMatrixMatlab x("X",2,3);
  AMD::SymbolicMatrixMatlab y("B",3,5);
  AMD::SymbolicMatrixMatlab z("C",2,5);
  AMD::SymbolicMatrixMatlab u = transpose(x*y+z);
  assert(u.getNumRows()==5 && u.getNumCols()==2);
}

void testMatrixMatrixFunc() {
  typedef AMD::MatrixMatrixFunc<AMD::SymbolicMatrixMatlab,
				 AMD::SymbolicScalarMatlab> MMFunc;
  typedef AMD::ScalarMatrixFunc<AMD::SymbolicMatrixMatlab,
				AMD::SymbolicScalarMatlab> SMFunc;
  std::string ans;
  AMD::SymbolicMatrixMatlab x("X",3,3);
  MMFunc fx(x,false); // f_x(X) = X
  fx.println();
  AMD::SymbolicMatrixMatlab y("Y",3,3);
  MMFunc fy(y); // f_y(X) = Y
  fy.println();

  SMFunc func;

  ans = "Y'";
  // d/dX trace(X*Y)=Y^T
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

void testMatrixMatrixFunc3 () {
  typedef AMD::MatrixMatrixFunc<AMD::SymbolicMatrixMatlab, 
                                AMD::SymbolicScalarMatlab> MMFunc;
  typedef AMD::ScalarMatrixFunc<AMD::SymbolicMatrixMatlab, 
                                AMD::SymbolicScalarMatlab> SMFunc;

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

  MMFunc eTest1 = AMD::elementwiseProd(fX, fX);
  func4 = trace (eTest1);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest2 = AMD::elementwiseProd(AMD::elementwiseProd(fX, fX), fX);
  func4 = trace (eTest2);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
  MMFunc eTest3 = AMD::elementwiseProd(inv(fX), inv(fX));
  func4 = trace (eTest3);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
  MMFunc eTest4 = AMD::elementwiseProd(fX, inv(fX));
  func4 = trace (eTest4);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
  MMFunc eTest5 = AMD::elementwiseProd(fA, fX);
  func4 = logdet (eTest5);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  MMFunc eTest6 = AMD::elementwiseProd(fA, transpose(fX));
  func4 = logdet (eTest6);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  MMFunc eTest7 = AMD::elementwiseProd(fX, fX);
  func4 = logdet (eTest7);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
}

int main() {
  testSymbolicScalarMatlab();
  testSymbolicMatrixMatlab();
  testMatrixMatrixFunc();
  testMatrixMatrixFunc3();
  std::cout << "All tests passed." << std::endl;
  return(0);
}
