#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include <AMD/AMD.hpp>
#include <stack>

#define DEBUG

#define ROW 128 
#define COL 128 

template<typename T> 
class Stack {
  public:
    Stack() {head = 0;}
    void push(T& element) {
      array[head].deepCopy(element);
      head++;
    }
    // return the reference;
    T& top() {
      return array[head-1];
    }
    void pop() {
      head--;
    }
    int size() {
      return head;
    }
  private:
    T array[100];
    int head;
};

template<typename T> 
class Stack2 {
  public:
    Stack2() {head = 0;}
    void push(T& element) {
      array[head] = (element);
      head++;
    }
    // return the reference;
    T& top() {
      return array[head-1];
    }
    void pop() {
      head--;
    }
    int size() {
      return head;
    }
  private:
    T array[100];
    int head;
};
// Typedef for SymbolicMatrixMatlab and SymbolicScalarMatlab.
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef symbolic_adaptor_type::value_type symbolic_value_type;

typedef AMD::MatrixMatrixFunc<symbolic_matrix_type,
                      				symbolic_value_type> SymbolicMMFunc;
typedef AMD::ScalarMatrixFunc<symbolic_matrix_type,
                       				symbolic_value_type> SymbolicSMFunc;


/* return if c is operator. */
bool isOp(char c) {
  if (c == '+' || c == '-' ||
      c == '*' || c == '/' ||
      c == 'i' || c == 't' ||
      c == '(' || c == ')')
    return true;
  else 
    return false;
}

bool isOpStr(std::string& str) {
  if ( str == "+"   || str == "-"         ||
       str == "*"   || str == "/"         ||
       str == "inv" || str == "transpose" ||
       str == "("   || str == ")")
    return true;
  else 
    return false;  
}
/* return the priority of operator. */
int priority(char c) {
  if (c == '+' || c == '-') 
    return 1;
  else if (c == '*' || c == '/') 
    return 2;
  else if (c == 'i' || c == 't') /**< i is inv. t is transpose. */
    return 3;
  else if (c == '(' || c == ')')
    return 4;
  return 0; 
}

int priorityStr(std::string& str) {
  if (str == "+" || str == "-") 
    return 1;
  else if (str == "*" || str == "/") 
    return 2;
  else if (str == "inv" || str == "transpose") /**< i is inv. t is transpose. */
    return 3;
  else if (str == "(" || str == ")")
    return 4;
  return 0; 
}
std::vector<std::string> stringParser(std::string& str) {
  std::vector<std::string> result;
  int size = str.size();
  int i;
  for (i = 0; i < size; i++) {
    if (str[i] == '+') {
      result.push_back("+");
    } else 
    if (str[i] == '-') {
      result.push_back("-");
    } else 
    if (str[i] == '*') {
      result.push_back("*");
    } else 
    if (str[i] == '(') {
      result.push_back("(");
    } else 
    if (str[i] == ')') {
      result.push_back(")");
    } else 
    if (str[i] == 'i') {
      result.push_back("inv");
      i += 2;
    } else 
    if (str[i] == 't' && str[i+3] == 'n') {
      result.push_back("transpose");
      i += 4;
    } else 
    if (str[i] == 't' && str[i+3] == 'c') {
      while (str[i] != '(') {
        i++;
      }
      i++; 
      int cnt = 1;
      std::string traceStr = "trace(";
      while (cnt != 0) {
        if (str[i] == '(') {
          cnt++;
        } 
        if (str[i] == ')'){
          cnt--;
        }
        traceStr = traceStr + str[i];
        i++;
      }
      i--;
      result.push_back(traceStr);
    } else 
    if (str[i] == 'l' && str[i+1] == 'o') {
      while (str[i] != '(') {
        i++;
      }
      i++; 
      int cnt = 1;
      std::string traceStr = "logdet(";
      while (cnt != 0) {
        if (str[i] == '(') {
          cnt++;
        } 
        if (str[i] == ')'){
          cnt--;
        }
        traceStr = traceStr + str[i];
        i++;
      }
      i--;
      result.push_back(traceStr);
    } else {
      std::string varStr = "";
      while (!isOp(str[i]) && i != size) {
        varStr = varStr + str[i];
        i++;
      }
      i--;
      result.push_back(varStr);
    }
  }
  return result;
}

/* infix (input expression) to reverse polish notation. */
std::vector<std::string> infix2rpn(std::vector<std::string>& inflix) {
  std::vector<std::string> result;
  std::stack<std::string> opStack;
  int size = inflix.size();
  int i;
  for (i = 0; i < size; i++) {
    if (!isOpStr(inflix[i])) {
      result.push_back(inflix[i]);
      continue;
    } else {
      if (inflix[i] == ")") {
        while (!opStack.empty() && opStack.top() != "(") {
        result.push_back(opStack.top());
        opStack.pop();
        }
        opStack.pop();
      } else
      if (opStack.empty()       || 
          opStack.top() == "("  ||
          priorityStr(inflix[i]) > priorityStr(opStack.top())) {
        opStack.push(inflix[i]);
      } else {
        while (!opStack.empty() && opStack.top() != "(" &&
               priorityStr(inflix[i]) <= priorityStr(opStack.top())) {
          result.push_back(opStack.top());
          opStack.pop();
        }
        opStack.push(inflix[i]);
      }
    }
  }
  while (!opStack.empty()) {
    result.push_back(opStack.top());
    opStack.pop();
  }
  return result;
}
/* Compute SymbolicMatrixMatrix Stack. */
SymbolicSMFunc compDerivative(std::vector<std::string> str,
                              int SMFtype, 
                              int Row, 
                              int Col) {
  int i; 
  Stack<SymbolicMMFunc> MMFStack;
  int size = str.size();
  symbolic_matrix_type X("X", Row, Col);
  SymbolicMMFunc fX(X, false);
  symbolic_matrix_type A("A", Row, Col);
  SymbolicMMFunc fA(A, true);
  symbolic_matrix_type B("B", Row, Col);
  SymbolicMMFunc fB(B, true);
  SymbolicSMFunc func;

  symbolic_matrix_type EYE =  symbolic_adaptor_type::eye(Row);
  SymbolicMMFunc fI(EYE, true);

  symbolic_matrix_type ZERO =  symbolic_adaptor_type::zeros(Row, Col);
  SymbolicMMFunc fZ(ZERO, true);
 
  for (i = 0; i < size; i++) {
    if (str[i] == "X" || str[i]=="x") {
      SymbolicMMFunc fXX;
      fXX.deepCopy(fX);
      MMFStack.push(fXX);
    } else 
    if (str[i] == "A") {
      SymbolicMMFunc fAA;
      fAA.deepCopy(fA);
      MMFStack.push(fAA);
    } else 
    if (str[i] == "B") {
      SymbolicMMFunc fBB;
      fBB.deepCopy(fB);
      MMFStack.push(fBB);
    } else 
    if (str[i] == "I") { 
      SymbolicMMFunc fII;
      fII.deepCopy(fI);
      MMFStack.push(fII);
    } else 
    if (str[i] == "Z") {
      SymbolicMMFunc fZZ;
      fZZ.deepCopy(fZ);
      MMFStack.push(fZZ);  
    } else 
    if (str[i] == "+") {
      SymbolicMMFunc f1;
      f1.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f2;
      f2.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f3;
      f3.deepCopy(f2+f1);
      MMFStack.push(f3);
    } else 
    if (str[i] == "-") {
      SymbolicMMFunc f1;
      f1.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f2;
      f2.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f3;
      f3.deepCopy(f2-f1);
      MMFStack.push(f3);
    } else 
    if (str[i] == "*") {
      SymbolicMMFunc f1;
      f1.deepCopy( MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f2;
      f2.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f3;
      f3.deepCopy(f2*f1);
      MMFStack.push(f3);
    } else 
    if (str[i] == "inv") {
      SymbolicMMFunc f1;
      f1.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f2;
      f2.deepCopy(inv(f1));
      MMFStack.push(f2);
    } else 
    if (str[i] == "transpose") {
      SymbolicMMFunc f1;
      f1.deepCopy(MMFStack.top());
      MMFStack.pop();
      SymbolicMMFunc f2;
      f2.deepCopy(transpose(f1));
      MMFStack.push(f2);
    } else {
      std::cout << "Incorrect Input" << std::endl;
      exit;
    }
  }
  if (SMFtype == 1) {
    func = trace(MMFStack.top());
  } else 
  if (SMFtype == 2) {
    func = logdet(MMFStack.top());
  } else {
    std::cerr << "ERROR INCORRECT INPUT" << std::endl;
    exit;
  }
  return func;
}
/* Compute SymbolicScalarMatrix Stack. */
SymbolicSMFunc compSMDerivative(std::vector<std::string> str, 
                                int Row, 
                                int Col) {
  int i; 
  Stack2<SymbolicSMFunc> SMFStack;
  int size = str.size();
  SymbolicSMFunc func;
 
  for (i = 0; i < size; i++) {
    if (str[i] == "+") {
      SymbolicSMFunc f1;
      f1 = SMFStack.top();
      SMFStack.pop();
      SymbolicSMFunc f2;
      f2 = SMFStack.top();
      SMFStack.pop();
      SymbolicSMFunc f3 = f2 + f1;
      SMFStack.push(f3);
    } else 
    if (str[i] == "-") {
      SymbolicSMFunc f1;
      f1 = (SMFStack.top());
      SMFStack.pop();
      SymbolicSMFunc f2;
      f2 = (SMFStack.top());
      SMFStack.pop();
      SymbolicSMFunc f3 = f2 - f1;
      SMFStack.push(f3);
    } else 
    if (str[i] == "*") {
      SymbolicSMFunc f1;
      f1 = ( SMFStack.top());
      SMFStack.pop();
      SymbolicSMFunc f2;
      f2=(SMFStack.top());
      SMFStack.pop();
      SymbolicSMFunc f3 = f2 * f1;
      SMFStack.push(f3);
    } else 
    if (str[i][0] == 't') {
      std::string exp = str[i].substr(5, str[i].size() -5);
      int type = 1;
      std::vector<std::string> par = stringParser(exp);
      std::vector<std::string> rpn = infix2rpn(par);
      SymbolicSMFunc funcResult = (compDerivative(rpn, type, Row , Col));
      SMFStack.push(funcResult);
    } else
    if (str[i][0] == 'l') {
      std::string exp = str[i].substr(6, str[i].size() -6);
      int type = 2;
      std::vector<std::string> par = stringParser(exp);
      std::vector<std::string> rpn = infix2rpn(par);
      
      SymbolicSMFunc funcResult = (compDerivative(rpn, type, Row , Col));
      SMFStack.push(funcResult);
    } 
  }
  func = SMFStack.top();
  return func;
}

int main(int argc, char** argv) {
  std::string str ;
  std::string rowStr;
  std::string colStr;
//  std::string rpn;
  int row;
  int col;
  int i; 
  if ( argc <=1) {
    std::cout << "./SymbolicCalculator 'express' row=? col=?" << std::endl;
    return -1;
  }
  if (argc != 4) {
    std::cout << "./SymbolicCalculator 'express' row=? col=?" << std::endl;
    return -1;
  }
  str = argv[1];
  rowStr = argv[2];
  colStr = argv[3];
  std::string rowStrSub(rowStr.begin() + 4, rowStr.end());
  std::string colStrSub(colStr.begin() + 4, colStr.end());
  row = stoi(rowStrSub);
  col = stoi(colStrSub);
  if (row <=0 || col <=0) {
    std::cerr << "Row and Col must be positive" << std::endl;
    return -1;
  }
  std::string expNoSpace = "";
  int type = 0;
  // Remove the spaces.
  for (std::string::iterator it = str.begin(); it != str.end(); it++) {
    if (*it != ' ') 
      expNoSpace += *it;
  }
  std::vector<std::string> vec = stringParser(expNoSpace);
  std::vector<std::string> result = infix2rpn(vec);
  SymbolicSMFunc func = compSMDerivative(result, row , col);
  std::cout << "Function:   " << func.functionVal.getString() << std::endl;
  std::cout << "Derivative: " << func.derivativeVal.getString() <<std::endl;
  return 0;
}

