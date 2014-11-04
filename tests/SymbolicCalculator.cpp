/**
 * @file This file defines a symbolic calculator taking an 
 *       SymbolicSMFunc expression 
 *       as input and compute the derivative value of the 
 *       expression.
 * @author Allan Sapucaia Barboza
 */


/* broken tests:
   'trace(X*2)' , 'trace(X*trace(X*S))' - should work
 */ 
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include <AMD/AMD.hpp>
#include <stack>
#include <vector>
#include <boost/assign/list_of.hpp>

#define DEBUG

#define ROW 128 
#define COL 128 

namespace AMD{

/* Typedef for SymbolicMatrixMatlab and SymbolicScalarMatlab. */
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::SymbolicScalarMatlab symbolic_scalar_type;
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
        //exception: empty_input
        std::cerr << "Empty Input" << std::endl;
        exit(-1);
      }
      if(row <= 0 || col <= 0) {
        std::cerr << "Row and Col must be positive" << std::endl;
        exit(-1);
      }
      /* Get the infix notation. */
      infix = stringParser(exprNoSpace);
      /* Get the reverse Polish notation. */
      rpn = infix2rpn(infix);
      /* Create the ScalarMatrxiFunc. */
      func = boost::make_shared<SymbolicSMFunc>(computeSMF(rpn, row, col));               
    }

    
    /**
     * @brief     Return the function value.
     * @return    string-type function value. 
     */
    std::string functionStr() {
      return func->functionVal.getString();
    }
    /**
     * @brief     Return the derivative value.
     * @return    string-type derivative value. 
     */
    std::string derivativeStr() {
      return func->derivativeVal.getString();
    }
  
    /**
     * @brief     Return a shared_pointer to the computational tree.
     * @return    shared_ptr<SymbolicSMFunc>-type root of the tree
     */
  //does not have a pointer to other functions
  boost::shared_ptr<SymbolicSMFunc> getComputationalTree() {
    return func;
  }
  private:
    int row;  /**< row number of matrices. */
    int col;  /**< colume number of matrices. */ 
    std::string expr; /**< SMF expression. */
    std::vector<std::string> infix; /**< expression infix order.*/
    std::vector<std::string> rpn;   /**< expression Reverse Polish Order.*/
    boost::shared_ptr<SymbolicSMFunc> func;  /**< result of the expression. */
  
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

    /* Return the operator string. */
    std::string getOpStr(char op);
    /* Decide if c is an operator. */
    bool isOp(char c);    
    /* Decide if str[pos:] is an operator. */
    bool isOpStr(std::string& str,int pos = 0); 
    /* Return the priority of operator. */
    int priority(char c);
    /* Return the priority of operator. */
    int priorityStr(std::string& str);

  
    const static std::vector<char> opList; /**< vector of operators (char).*/
    /**< vector of operators (str). */
    const static std::vector<std::string>  opStrList; 
    /** vector of priorities of operators. **/ 
    const static std::vector<int> opPriorityList;
};

/*initialize constant static data */
const std::vector<char> Calculator::opList = 
                     boost::assign::list_of('+')('-')('*')('/')('i')
                     ('t')('(')(')')('.');

const std::vector<std::string> Calculator::opStrList = 
                     boost::assign::list_of("+")("-")("*")("/")("inv")
                     ("transpose")("(")(")")(".*");

const std::vector<int> Calculator::opPriorityList = 
                     boost::assign::list_of(1)(1)(2)(2)(3)(3)(4)(4)(2);

/* return the operator string */
std::string Calculator::getOpStr(char op) {
  for(int i = 0; i < opList.size();i++) {
    if(op == opList[i])
      return opStrList[i];
  }
  return "";
}
/* return if c is operator. */
bool Calculator::isOp(char c) {
  for(int i = 0; i < opList.size();i++) {
    if(opList[i] == c)
      return true;
  }
  return false;
}

bool Calculator::isOpStr(std::string& str,int pos) {
  for(int i = 0; i < opStrList.size();i++) {
    if(str.compare(pos,opStrList[i].size(),opStrList[i]) == 0)
       return true;
  }
  return false;  
}
/* return the priority of operator. */
int Calculator::priority(char c) {
  for(int i = 0; i < opList.size();i++) {
    if(opList[i] == c)
      return opPriorityList[i];
  }
  return 0; 
}

int Calculator::priorityStr(std::string& str) {
  for(int i = 0; i < opStrList.size();i++)
    if(opStrList[i] == str)
      return opPriorityList[i];
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
  

  for (i = 0; i < size; i++) { //operations
    bool foundSymbol = false;
    for(int j = 0; j < opStrList.size() && !foundSymbol;j++) {
      if(str.compare(i,opStrList[j].size(),opStrList[j]) == 0) {
        result.push_back(opStrList[j]);
        i+=opStrList[j].size()-1;
        foundSymbol = true;
        break;
      }
    }
    if(!foundSymbol) {
      if (str.compare(i,6,"trace(") == 0) {//trace
        i+=6; 
        int cnt = 1;
        std::string traceStr = "trace(";
        while (i < str.size() && cnt != 0) {
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
        //exception: cnt != 0 - parsing_exception
        if(cnt != 0) {
          std::cerr << "Parentheses mismatch! - stringparser" << std::endl;
          exit(-1);
        }
        result.push_back(traceStr);
      } else 
        if (str.compare(i,7,"logdet(")==0) {//logdet
          i+=7; 
          int cnt = 1;
          std::string logdetStr = "logdet(";
          while (i < str.size() && cnt != 0) {
            if (str[i] == '(') {
              cnt++;
            } 
            if (str[i] == ')'){
              cnt--;
            }
            logdetStr = logdetStr + str[i];
            i++;
          }
          i--;
          //exception: cnt != 0 - parsing exception
          if(cnt != 0) {
            std::cerr << "Parentheses mismatch! - stringparser" << std::endl;
            exit(-1);
          }
          result.push_back(logdetStr);
        }else
        if(std::isalpha(str[i])) { // matrices and real constants
          std::string varStr(1,str[i]);
          result.push_back(varStr);
        }else 
        if(std::isdigit(str[i])) {
          std::string varStr = "";
            while(isdigit(str[i])) {
              varStr += str[i];
              i++;
            }
            i--;
          result.push_back(varStr);
        }else { 
          //exception - parsing_exception
          std::cout << "Incorrect Input - stringparser" << std::endl;
          exit(-1);
        }
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
        if(opStack.empty() == true) {
          //exception - parsing_exception
          std::cout << "Incorrect Input - infix2rpn" << std::endl;
          exit(-1);
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
  std::stack<boost::shared_ptr<SymbolicMMFunc> > MMFStack;
  int size = str.size();
  
  std::vector<symbolic_matrix_type> matrices;
  std::vector<boost::shared_ptr<SymbolicMMFunc> > matrixFuncs;
  matrices.resize('Z'-'A'+1);
  matrixFuncs.resize('Z'-'A'+1);

  char identifier[2];
  identifier[1] = 0;

  //TO-DO: make it global
  for(char c = 'A'; c < 'Z';c++) {
    identifier[0] = c;
    //identity and X are created separately
    if(c == 'I' || c == 'X')
      continue;
    matrices[c-'A'] = symbolic_matrix_type(identifier,Row,Col);
    matrixFuncs[c-'A'] = boost::shared_ptr<SymbolicMMFunc>
      (new SymbolicMMFunc(matrices[c-'A'],true));
  }

  matrices['X'-'A'] = symbolic_matrix_type("X",Row,Col);
  matrixFuncs['X'-'A'] = boost::shared_ptr<SymbolicMMFunc>
    (new SymbolicMMFunc(matrices['X'-'A'], false));
 
  matrices['I'-'A'] = symbolic_adaptor_type::eye(Row);
  matrixFuncs['I'-'A'] = boost::shared_ptr<SymbolicMMFunc>
    (new SymbolicMMFunc (matrices['I'-'A'], true));

  matrices['Z'-'A'] =  symbolic_adaptor_type::zeros(Row, Col);
  matrixFuncs['Z'-'A'] = boost::shared_ptr<SymbolicMMFunc>
    (new SymbolicMMFunc (matrices['Z'-'A'], true));
 
  SymbolicSMFunc func;
  for (i = 0; i < size; i++) {
    if(str[i].size() == 1 && isupper(str[i][0])) {
      MMFStack.push(matrixFuncs[str[i][0]-'A']);
    } else 
    if (str[i] == "+") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2;
      f2 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f3 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc((*f2)+(*f1)));
      MMFStack.push(f3);
    } else 
    if (str[i] == "-") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2;
      f2 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f3 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc((*f2)-(*f1)));
      MMFStack.push(f3);
      } else 
    if (str[i] == "*") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2;
      f2 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f3 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc((*f2)*(*f1)));
      MMFStack.push(f3);
    } else 
    if (str[i] == ".*") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2;
      f2 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f3 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc(elementwiseProduct(*f2, *f1)));
      MMFStack.push(f3);
      } else 
    if (str[i] == "inv") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc(inv(*f1)));
      MMFStack.push(f2);
    } else 
    if (str[i] == "transpose") {
      boost::shared_ptr<SymbolicMMFunc> f1;
      f1 = MMFStack.top();
      MMFStack.pop();

      boost::shared_ptr<SymbolicMMFunc> f2 = boost::shared_ptr<SymbolicMMFunc>(new SymbolicMMFunc(transpose(*f1)));
      MMFStack.push(f2);
    } else {
      //exception - parsing_exception
      std::cout << "Incorrect Input" << std::endl;
      exit(-1);
    }
  }
  if (SMFtype == 1) {
    func = trace(*MMFStack.top());
    MMFStack.pop();
  } else 
  if (SMFtype == 2) {
    func = logdet(*MMFStack.top());
    MMFStack.pop();
  } else {
    //exception - parsing_exception
    std::cerr << "ERROR INCORRECT INPUT" << std::endl;
    exit(-1);
  }

  if(MMFStack.empty() == false) {
    //exception - parsing_exception
    std::cerr << "ERROR INCORRECT INPUT - stackMMF" << std::endl;
    exit(-1);
  }
  return func;
}

/* Compute SymbolicScalarMatrix Stack. */
SymbolicSMFunc Calculator::computeSMF(std::vector<std::string>& str, 
                                int Row, 
                                int Col) {
  int i; 
  std::stack<SymbolicSMFunc> SMFStack;
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
      //exception: (f2.m != f1.m || f2.n != f1.n) -mismatched_dimension
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
      //exception: (f2.m != f1.m || f2.n != f1.n) -mismatched_dimension
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
      //exception: (f2.n != f1.m ) -mismatched_dimension
      SymbolicSMFunc f3 = f2 * f1;
      SMFStack.push(f3);
    } else 
    if (str[i].compare(0,5,"trace") == 0) { //trace
      std::string exp = str[i].substr(5, str[i].size() -5);
      int type = 1;
      std::vector<std::string> par = stringParser(exp);
      std::vector<std::string> rpn = infix2rpn(par);
      SymbolicSMFunc funcResult = (computeSingleSMF(rpn, type, Row , Col));
      SMFStack.push(funcResult);
    } else
    if (str[i].compare(0,6,"logdet") == 0) {
      std::string exp = str[i].substr(6, str[i].size() -6);
      int type = 2;
      std::vector<std::string> par = stringParser(exp);
      std::vector<std::string> rpn = infix2rpn(par);
      
      SymbolicSMFunc funcResult = (computeSingleSMF(rpn, type, Row , Col));
      SMFStack.push(funcResult);    
    } else {
      bool isRealConst = false;
      
      if (str[i].size() == 1 && islower(str[i][0]) && str[i][0] != 'x') //check if it is a real constant
        isRealConst = true;
      
      bool isRealNumber = true;
      for (int j = 0; j < str[i].size();j++) {
        if(!isdigit(str[i][j]))
          isRealNumber = false;

      }    
      if (isRealConst || isRealNumber) {
        symbolic_scalar_type num(str[i]);
        SymbolicSMFunc constantSMFunc(num, Row, Row);
        SMFStack.push(constantSMFunc);
      }
      else {
        //exception - parsing_exception
        std::cerr << "ERROR INCORRECT INPUT - invalid SMF" << std::endl;
        exit(-1);
      }
    }
  }
  func = SMFStack.top();
  SMFStack.pop();
  if(SMFStack.empty() == false) {
    //exception - parsing_exception
    std::cerr << "ERROR INCORRECT INPUT - stackSMF" << std::endl;
    exit(-1);
  }
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
  
  AMD::Calculator cal(str, row, col);
  std::cout << "Function:   " << cal.functionStr() << std::endl;
  std::cout << "Derivative: " << cal.derivativeStr() << std::endl;
  //std::cout << "MMF: " << cal.getComputationalTree()->derivativeFuncVal->matrixPtr->getString() << std::endl;
  //std::cout << cal.getComputationalTree()->functionVal.getString() << std::endl;
  return 0;
}

