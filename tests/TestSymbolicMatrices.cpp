#include <iostream>
#include <string>
#include <cstdio>
#include <assert.h>
#include <AMD/AMD.hpp>
#include "boost/shared_ptr.hpp"

/**
 * TODO: Add more extensize tests and ensure that the results are correct.
 */ 
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

/**
 * TODO: Add more extensive tests and ensure that the results are correct.
 */ 
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


int main() {
  std::cout << "Testing SymbolicScalarMatlab .... ";
  testSymbolicScalarMatlab();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing SymbolicScalarMatlab .... ";
  testSymbolicMatrixMatlab();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  return(0);
}
