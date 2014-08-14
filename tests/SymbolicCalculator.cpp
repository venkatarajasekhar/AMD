/**
 * @file This file defines a symbolic calculator taking an 
 *       SymbolicSMFunc expression 
 *       as input and compute the derivative value of the 
 *       expression.
 */

#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include <AMD/AMD.hpp>
#include <stack>
#include <vector>

#define DEBUG

#define ROW 128 
#define COL 128 
/**
 * @brief A useer-defined stack container only for
 *        MatrixMatrixFunc class.(because of deepCopy) 
 *        The stack uses a array for internal 
 *        storage of object. The maximum size 
 *        is 100.
 */
namespace AMD{
template<typename T> 
class Stack {
  public:
    /**
     * A constructor
     * The constructor set the top-curser at the
     * bottom of the stack.  
     */
    Stack() {head = 0;}
    /**
     *  @brief Push an object into stack. 
     *  @param element The reference to the object. 
     */
    void push(T& element) {
      array[head].deepCopy(element);
      head++;
    }
    /**
     *  @brief Return the top object in stack. 
     *  @return The reference to the top object in stack. 
     */
    T& top() {
      return array[head-1];
    }
    /**
     * @brief Pop the stack.
     */
    void pop() {
      head--;
    }
    /**
     * @brief Return the stack size.
     * @return Stack size.
     */
    int size() {
      return head;
    }
  private:
    /* Internal storage data structure. The max size is 100. */
    T array[100];
    /* Top cursor. */
    int head;
};

/**
 * @brief A useer-defined stack container.
 *        The stack uses a array for internal 
 *        storage of object. The maximum size 
 *        is 100.
 */
template<typename T> 
class Stack2 {
  public:

    /**
     * A constructor
     * The constructor set the top-curser at the
     * bottom of the stack.  
     */
    Stack2() {head = 0;}

    /**
     *  @brief Push an object into stack. 
     *  @param element The reference to the object. 
     */
    void push(T& element) {
      array[head] = (element);
      head++;
    }
    /**
     *  @brief Return the top object in stack. 
     *  @return The reference to the top object in stack. 
     */
    T& top() {
      return array[head-1];
    }
    /**
     * @brief Pop the stack.
     */
    void pop() {
      head--;
    }
    /**
     * @brief Return the stack size.
     * @return Stack size.
     */
    int size() {
      return head;
    }
  private:
    /* Internal storage data structure. The max size is 100. */
    T array[100];
    /* Top cursor. */
    int head;
};
/* Typedef for SymbolicMatrixMatlab and SymbolicScalarMatlab. */
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef symbolic_adaptor_type::value_type symbolic_value_type;

typedef AMD::MatrixMatrixFunc<symbolic_matrix_type,
                      				symbolic_value_type> SymbolicMMFunc;
typedef AMD::ScalarMatrixFunc<symbolic_matrix_type,
                       				symbolic_value_type> SymbolicSMFunc;  
class Calculator {
  public:
    /* Default constructor. */
    Calculator(){};
    /* Default destructor. */
    ~Calculator(){};
    /**
     * @brief Construction the Calculator with an SMF
     *        expression. Compute the derivative value
     *        and function value of the expression.
     * @param expr the SMF expression.
     * @param row  matrix row.
     * @param col  matrix col.
     */
    Calculator(std::string expr, int row=4, int col=4) :
              expr(expr), row(row), col(col) {

      std::string exprNoSpace = "";
      for (std::string::iterator it = expr.begin(); it != expr.end(); it++) {
        if (*it != ' ') 
          exprNoSpace += *it;
      }
      if (exprNoSpace.size() == 0) {
        std::cerr << "Empty Input" << std::endl;
        exit;
      }
      /* Get the infix notation. */
      infix = stringParser(exprNoSpace);
      /* Get the reverse Polish notation. */
      rpn = infix2rpn(infix);
      /* Create the ScalarMatrxiFunc. */
      func = computeSMF(rpn, row, col);               
    }

    /**
     * @brief     An infix notation parser.
     * @param str The input infix notation.
     * @return    A vector<string> contains the operands and 
     *            operators in a infix-order.
     */
    std::vector<std::string> stringParser(std::string& str);
    /**
     * @brief     Translate infix to reverse polish notation.
     * @param infix The infix notation.
     * @return    A vector<string> contains the operands and
     *            operators in a reverse polish order.
     */
    std::vector<std::string> infix2rpn(std::vector<std::string>& infix);
    /**
     * @brief     Compute the function value and derivative value
     *            for a single ScalarMatrixFunction.
     * @param str The reverse polish notation of the the 
     *            Matrix Matrix Function within the parentheses of
     *            the Scalar-Matrix Function.
     * @param SMFtype The type of the SMF. trace or logdet.
     * @param Row Row number of the matrices.
     * @param Col Colume number of the matrices.
     * @return The Symbolic ScalarMatrixFunction object.
     */
    SymbolicSMFunc computeSingleSMF (std::vector<std::string>& str,
                                   int SMFtype,
                                   int Row,
                                   int Col);
    /**
     * @brief     Compute the function value and derivative value 
     *            for the SMF expression.
     * @param str The RPN of the operands and operators.
     * @param Row The row number of the matrices.
     * @param Col The colume number of the matrices.
     * @return The sum of the SMF expression. 
     */
    SymbolicSMFunc computeSMF (std::vector<std::string>& str,
                                     int Row,
                                     int Col);
    /**
     * @brief     Return the function value.
     * @return    string-type function value. 
     */
    std::string functionStr() {
      return func.functionVal.getString();
    }
    /**
     * @brief     Return the derivative value.
     * @return    string-type derivative value. 
     */
    std::string derivativeStr() {
      return func.derivativeVal.getString();
    }
   
  private:
    int row;  /**< row number of matrices. */
    int col;  /**< colume number of matrices. */ 
    std::string expr; /**< SMF expression. */
    std::vector<std::string> infix; /**< expression infix order.*/
    std::vector<std::string> rpn;   /**< expression Reverse Polish Order.*/
    SymbolicSMFunc func;  /**< result of the expression. */
    /* Decide if c is an operator. */
    bool isOp(char c);    
    /* Decide if str is an operator. */
    bool isOpStr(std::string& str); 
    /* Return the priority of operator. */
    int priority(char c);
    /* Return the priority of operator. */
    int priorityStr(std::string& str);
};
/* return if c is operator. */
bool Calculator::isOp(char c) {
  if (c == '+' || c == '-' ||
      c == '*' || c == '/' ||
      c == 'i' || c == 't' ||
      c == '(' || c == ')')
    return true;
  else 
    return false;
}

bool Calculator::isOpStr(std::string& str) {
  if ( str == "+"   || str == "-"         ||
       str == "*"   || str == "/"         ||
       str == "inv" || str == "transpose" ||
       str == "("   || str == ")")
    return true;
  else 
    return false;  
}
/* return the priority of operator. */
int Calculator::priority(char c) {
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

int Calculator::priorityStr(std::string& str) {
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

/*
 * @brief   parse the infix expression and put each part 
 *          seperately in a vector. 
 * @param   str input infix string 
 * @return  The vector that contains each parts of the expression.  
 */ 
std::vector<std::string> Calculator::stringParser(std::string& str) {
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
std::vector<std::string> Calculator::infix2rpn(std::vector<std::string>& infix) {
  std::vector<std::string> result;
  std::stack<std::string> opStack;
  int size = infix.size();
  int i;
  for (i = 0; i < size; i++) {
    if (!isOpStr(infix[i])) {
      result.push_back(infix[i]);
      continue;
    } else {
      if (infix[i] == ")") {
        while (!opStack.empty() && opStack.top() != "(") {
        result.push_back(opStack.top());
        opStack.pop();
        }
        opStack.pop();
      } else
      if (opStack.empty()       || 
          opStack.top() == "("  ||
          priorityStr(infix[i]) > priorityStr(opStack.top())) {
        opStack.push(infix[i]);
      } else {
        while (!opStack.empty() && opStack.top() != "(" &&
               priorityStr(infix[i]) <= priorityStr(opStack.top())) {
          result.push_back(opStack.top());
          opStack.pop();
        }
        opStack.push(infix[i]);
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
SymbolicSMFunc Calculator::computeSingleSMF(std::vector<std::string>& str,
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
SymbolicSMFunc Calculator::computeSMF(std::vector<std::string>& str, 
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
      SymbolicSMFunc funcResult = (computeSingleSMF(rpn, type, Row , Col));
      SMFStack.push(funcResult);
    } else
    if (str[i][0] == 'l') {
      std::string exp = str[i].substr(6, str[i].size() -6);
      int type = 2;
      std::vector<std::string> par = stringParser(exp);
      std::vector<std::string> rpn = infix2rpn(par);
      
      SymbolicSMFunc funcResult = (computeSingleSMF(rpn, type, Row , Col));
      SMFStack.push(funcResult);
    } else {
      AMD::SymbolicScalarMatlab num(str[i]);
      SymbolicSMFunc constantSMFunc(num, Row, Row);
      SMFStack.push(constantSMFunc);
    }
  }
  func = SMFStack.top();
  return func;
}
}

int main(int argc, char** argv) {
  std::string str ;
  std::string rowStr;
  std::string colStr;
  int row;
  int col;
  int i; 
  if ( argc <=1) {
    std::cout << "./SymbolicCalculator 'express' row=? col=?" << std::endl;
    return -1;
  }
  if (argc == 2) {
    str = argv[1];
    row = 4;
    col = 4;
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
  if (row <=0 || col <=0) {
    std::cerr << "Row and Col must be positive" << std::endl;
    return -1;
  }
  if (str.size() == 0) {
    std::cerr << "Empty Expression" << std::endl;
    return -1;
  }
  
  AMD::Calculator cal(str, row, col);
  std::cout << "Function:   " << cal.functionStr() << std::endl;
  std::cout << "Derivative: " << cal.derivativeStr() << std::endl;
  return 0;
}

