#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <AMD/AMD.hpp>
#include <AMD/SymbolicCalculator.hpp>
#include <AMD/Exception.hpp>
#include <stack>
#include <vector>


int main(int argc, char** argv) {
  std::string str ;
  std::string rowStr;
  std::string colStr;
  int row;
  int col;
  int i; 

  bool testMode = false;
  if ( argc <=1) {
    std::cout << "./SymbolicCalculator 'express' row=? col=?" << std::endl;
    std::cout << "or" << std::endl;
    std::cout << "./SymbolicCalculator examples" << std::endl;
    return -1;
  }
  if (argc == 2) {
    str = argv[1]; 
    if (str == "examples") {
      testMode = true;
    }
    else { 
      row = 4;
      col = 4;
    }
  } else 
  if (argc == 4) {
    str = argv[1];
    rowStr = argv[2];
    colStr = argv[3];
    std::string rowStrSub(rowStr.begin() + 4, rowStr.end());
    std::string colStrSub(colStr.begin() + 4, colStr.end());
    row = atoi(rowStrSub.c_str());
    col = atoi(colStrSub.c_str());
  } else {
    std::cout << "./SymbolicCalculator 'express' row=? col=?" << std::endl;
    return -1;
  }
  
  if (testMode) {
    std::vector<std::string> testsInput;

    //valid tests
    testsInput.push_back("trace(X)");
    testsInput.push_back("trace(X*X)");;
    testsInput.push_back("logdet(X*transpose(S*X))");
    testsInput.push_back("2*logdet(X+A)");
    testsInput.push_back("2*logdet(A+X+B)");
    testsInput.push_back("3*logdet(X*transpose(X))*trace(inv(X)+X)");
    testsInput.push_back("d*trace(X) + 3*logdet(X*transpose(X))*trace(inv(X)+X)+d");

    //invalid tests
    testsInput.push_back("amd");
    testsInput.push_back("trace(amd)");
    testsInput.push_back("trc(X)");
    testsInput.push_back("logdet((X+X)");
    testsInput.push_back("trace(X)(");
    testsInput.push_back("X+trace(X)");
    
    
    for (std::vector<std::string>::iterator s = testsInput.begin();
         s != testsInput.end();s++) {
      AMD::Calculator cal;
      std::cout << "Expression: " << *s << std::endl;
      try {
        
        cal.compute(*s);
        std::cout << "Function:   " << cal.functionStr() << std::endl;
        std::cout << "Derivative: " << cal.derivativeStr() << std::endl;
        
      } catch (const AMD::exception & error) {
        std::cout << "An error occurred:"<< std::endl;
        std::cout << "\t" << error.what() << std::endl;
        std::cout << "\t" << error.trace() << std::endl;
      }
      std::cout << std::endl;
    }
    return 0;
  }
  try {
    AMD::Calculator cal(row, col);
    cal.compute(str);
    std::cout << "Function:   " << cal.functionStr() << std::endl;
    std::cout << "Derivative: " << cal.derivativeStr() << std::endl;
    std::cout << "MMF: " << cal.getComputationalTree()->derivativeFuncVal->matrixPtr->getString() << std::endl;
    //std::cout << cal.getComputationalTree()->functionVal.getString() << std::endl;
  } catch (const AMD::exception & error) {
    std::cout << "Your expression is invalid:" << error.what() << std::endl;
    std::cout << "Run \"./SymbolicCalculator examples\" for examples of valid and invalid expressions." << std::endl;
  }

  return 0;
}

