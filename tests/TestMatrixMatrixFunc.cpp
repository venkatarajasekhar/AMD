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

#if AMD_HAVE_ELEMENTAL

/** Typedef for Elemental Matrices. */
typedef elem::Matrix<double> elemental_matrix_type;
typedef AMD::MatrixAdaptor_t<elemental_matrix_type> elemental_adaptor_type;
typedef typename elemental_adaptor_type::value_type elemental_value_type;

typedef AMD::MatrixMatrixFunc<elemental_matrix_type,
                      				elemental_value_type> ElementalMMFunc;
typedef AMD::ScalarMatrixFunc<elemental_matrix_type,
                       				elemental_value_type> ElementalSMFunc;

bool assert_close (double a, double b) {
  double error =  a-b;
  error = (0.0 > error) ? -error : error;
  return (error<=1e-9) ? true:false;
//  assert (error <= 1e-9);
}

/** Check if the two matrices are numerically equal. */
#define CHECK_ELEMENTAL_MATRIX(A, B) \
        check_elemental_matrix(A, B, __LINE__);
void check_elemental_matrix(const elemental_matrix_type& A, 
                                  const elemental_matrix_type& B,
                                  unsigned line) {                                                                           
  assert((elemental_adaptor_type::getNumRows(A) ==                            
          elemental_adaptor_type::getNumRows(B)) &&                           
          (elemental_adaptor_type::getNumCols(A) ==                           
          elemental_adaptor_type::getNumCols(B)));                            
  double normA = 0;
  double normDiff = 0;                                                                     
  for (int i = 0; i < elemental_adaptor_type::getNumRows(A); i++) {           
    for (int j = 0; j < elemental_adaptor_type::getNumCols(A); j++) {      
      double tmpA = A.Get(i,j);
      double tmpB = B.Get(i,j);
      normA += tmpA * tmpA;
      normDiff += (tmpA-tmpB)*(tmpA-tmpB); 
    }                                                                         
  }
  normA = sqrt(normA);
  normDiff = sqrt(normDiff);
  if (normDiff > 1.0e-9 * (normA+1.0e-15*normDiff)) {  
    std::cout << std::endl;
    std::cout << "In file " << __FILE__ << std::endl;
    std::cout << "Test failed at line: " << line << std::endl;
    std::cout << "Program aborted" << std::endl;
    exit(-1);
  }
}                                                                            

/**
 * @brief Test numerical matrix derivatives.
 */
void testElementalMatrixMatrixFunc() {
  
  /** Create constant matrices A, B, C, E etc */
  elemental_matrix_type A(ROW, COL);
  elemental_matrix_type B(ROW, COL);
  elemental_matrix_type C(ROW, COL);
  elemental_matrix_type D(ROW, COL);
  elemental_matrix_type E(ROW, COL);
  elemental_matrix_type F(ROW, COL);
  elemental_matrix_type G(ROW, COL);
  elemental_matrix_type H(ROW, COL);
  elemental_matrix_type I(ROW, COL);
  elemental_matrix_type J(ROW, COL);

  /** Create identity matrix EYE */
  elemental_matrix_type EYE = elemental_adaptor_type::eye(ROW); 
  /** Create result matrix */
  elemental_matrix_type RESULT(ROW, COL); /**< Hand calculated matrix result. */

  /** Create variable matrices X */
  elemental_matrix_type X(ROW, COL);
 
  /** Create transpose, inverse, negation for existing
   *  matrices to simplify programming 
   **/
  elemental_matrix_type AT(ROW, COL);     /**< A transpose  */
  elemental_matrix_type AI(ROW, COL);     /**< A inverse    */
  elemental_matrix_type AN(ROW, COL);     /**< A negation   */
  elemental_matrix_type AIT(ROW, COL);    /**< A inverse-transpose   */
  elemental_matrix_type BT(ROW, COL);     /**< B transpose  */
  elemental_matrix_type BI(ROW, COL);     /**< B inverse    */
  elemental_matrix_type BN(ROW, COL);     /**< B negation   */
  elemental_matrix_type BIT(ROW, COL);    /**< B inverse-transpose   */
  elemental_matrix_type XT(ROW, COL);     /**< X transpose  */
  elemental_matrix_type XI(ROW, COL);     /**< X inverse    */
  elemental_matrix_type XN(ROW, COL);     /**< X negation   */
  elemental_matrix_type XIT(ROW, COL);    /**< X inverse-transpose   */

  /** Initialize matrices */
  elem::MakeGaussian(A);
  elem::MakeGaussian(B);
  elem::MakeGaussian(X);
  
  /** Initialize transpose, inverse and negation matrices
   *  based on existing ones
   **/
  elemental_adaptor_type::transpose(A, AT);
  elemental_adaptor_type::negation(A, AN);
  elemental_adaptor_type::inv(A, AI);
  elemental_adaptor_type::transpose(AI, AIT);
  elemental_adaptor_type::transpose(B, BT);
  elemental_adaptor_type::negation(B, BN);
  elemental_adaptor_type::inv(B, BI);
  elemental_adaptor_type::transpose(BI, BIT);
  elemental_adaptor_type::transpose(X, XT);
  elemental_adaptor_type::negation(X, XN);
  elemental_adaptor_type::inv(X, XI);
  elemental_adaptor_type::transpose(XI, XIT);

  /** Create elementa matrix-matrix functions */ 
  ElementalMMFunc fA(A, true);
  ElementalMMFunc fB(B, true);
  ElementalMMFunc fX(X, false);

  /** Create elemental scalar-matrix function */
  ElementalSMFunc func;

  /** 1. d/dx(trace(AX)) = A'; */
  func = trace(fA * fX);
  elemental_adaptor_type::copy(RESULT, AT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);


  /** 2. d/dx(trace(AXBX)) = A'X'B' + B'X'A' */
  func = trace(fA * fX * fB * fX);
  elemental_adaptor_type::multiply (AT, XT, C);
  elemental_adaptor_type::multiply (C, BT, E);
  elemental_adaptor_type::multiply (BT, XT, D);
  elemental_adaptor_type::multiply (D, AT, F);
  elemental_adaptor_type::add (E, F, RESULT); 
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 3. d/dx(trace(AX'B)) = -(X^-1)'A'B'(X^-1)' */
  func = trace(fA * inv(fX) * fB);
  elemental_adaptor_type::multiply (XIT, AT, C);
  elemental_adaptor_type::multiply (C, BT, D);
  elemental_adaptor_type::multiply (D, XIT, E);
  elemental_adaptor_type::negation (E, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 4. d/dx(trace(A.*X)) = A.*eye(N) */
  func = trace(elementwiseProduct (fA, fX));
  elemental_adaptor_type::elementwiseProduct (A, EYE, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 5. d/dx(trace (X.*X)) =  2 * A .* eye(N) */
  func = trace (elementwiseProduct (fX, fX));
  elemental_adaptor_type::elementwiseProduct(X, EYE, C);
  elemental_adaptor_type::add(C, C, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 6. d/dx(trace(X^-1 .* X^-1)) = 
         -2 * X^-T * X^-1 .* EYE * X ^ -T    */
  func = trace (elementwiseProduct(inv(fX), inv(fX)));
  elemental_adaptor_type::elementwiseProduct (XI, EYE, C);
  elemental_adaptor_type::multiply (XIT, C, D);
  elemental_adaptor_type::multiply (D, XIT, E);
  elemental_adaptor_type::add (E, E, F);
  elemental_adaptor_type::negation (F, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 7. d/dx(trace(X .* X^-1)) = 
         X^-1 .* EYE - X^-T * X .* EYE * X^-T */
  func = trace (elementwiseProduct (fX, inv(fX)));
  elemental_adaptor_type::elementwiseProduct (XI, EYE, C);
  elemental_adaptor_type::elementwiseProduct (X, EYE, D);
  elemental_adaptor_type::multiply (XIT, D, E);
  elemental_adaptor_type::multiply (E, XIT, F);
  elemental_adaptor_type::minus (C, F, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  // For logdet test case we pick specific matrices as test cases
  // Reset matrix Matrix to be diagonal matrices with positive 
  // elements.
 
  elem::MakeGaussian (D); 
  elemental_adaptor_type::elementwiseProduct (D, EYE, A);
  for (int i = 0; i < ROW; i++) {
    A.Set(i, i, A.Get(i, i) * A.Get(i, i));
  }
  elem::MakeGaussian (E); 
  elemental_adaptor_type::elementwiseProduct (E, EYE, B);
  for (int i = 0; i < ROW; i++) {
    B.Set(i, i, B.Get(i, i) * B.Get(i, i));
  }
  elem::MakeGaussian (F); 
  elemental_adaptor_type::elementwiseProduct (F, EYE, X);
  for (int i = 0; i < ROW; i++) {
    X.Set(i, i, X.Get(i, i) * X.Get(i, i));
  }
  // Reset transpose, negation, inv and inverse-transpose
  elemental_adaptor_type::transpose(A, AT);
  elemental_adaptor_type::negation(A, AN);
  elemental_adaptor_type::inv(A, AI);
  elemental_adaptor_type::transpose(AI, AIT);
  elemental_adaptor_type::transpose(B, BT);
  elemental_adaptor_type::negation(B, BN);
  elemental_adaptor_type::inv(B, BI);
  elemental_adaptor_type::transpose(BI, BIT);
  elemental_adaptor_type::transpose(X, XT);
  elemental_adaptor_type::negation(X, XN);
  elemental_adaptor_type::inv(X, XI);
  elemental_adaptor_type::transpose(XI, XIT);

  ElementalMMFunc fA1(A, true);
  ElementalMMFunc fB1(B, true);
  ElementalMMFunc fX1(X, false);

  /** 8. d/dx(logdet(X)) == (X^-1)' */
  func = logdet(fX1);
  CHECK_ELEMENTAL_MATRIX(func.derivativeVal, XIT);

  /** 9. d/dx(logdet(AXB)) = A'((AXB)^-1)'B' */
  func = logdet(fA1 * fX1 * fB1);
  elemental_adaptor_type::multiply (A, X, C);
  elemental_adaptor_type::multiply (C, B, D);
  elemental_adaptor_type::inv (D, E);
  elemental_adaptor_type::transpose (E, F);
  elemental_adaptor_type::multiply (AT, F, G);
  elemental_adaptor_type::multiply (G, BT, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT); 

  /** 10. d/dx(X'AX) = AX(X'AX)^-1 + A'X((X'AX)^-1); */
  func = logdet(transpose(fX1) * fA1 * fX1);
  elemental_adaptor_type::multiply (XT, A, C);
  elemental_adaptor_type::multiply (C, X, D);
  elemental_adaptor_type::inv (D, E); // X'AX -1 
  elemental_adaptor_type::transpose (E, F);   // (X'AX)-T
  elemental_adaptor_type::multiply (A, X, G);
  elemental_adaptor_type::multiply (G, E, H);
  elemental_adaptor_type::multiply (AT, X, I);
  elemental_adaptor_type::multiply (I, F, J);
  elemental_adaptor_type::add (H, J, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT); 

  /** 11. d/dx(logdet(A.*X)) = A.*(A.*X)^-T*/
  func = logdet(elementwiseProduct (fA1, fX1));
  elemental_adaptor_type::elementwiseProduct (A, X, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::transpose (D, E);
  elemental_adaptor_type::elementwiseProduct (A, E, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 12. d/dx(logdet(A.*X')) = A'.*(A.*X')^-1 */
  func = logdet(elementwiseProduct(fA1, transpose(fX1)));
  elemental_adaptor_type::elementwiseProduct (A, XT, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::elementwiseProduct (AT, D, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 13. d/dx(logdet(X.*X)) = 2 * X.*(X.*X)'^-1 */
  func = logdet(elementwiseProduct (fX1, fX1));
  elemental_adaptor_type::elementwiseProduct (X, X, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::transpose (D, E);
  elemental_adaptor_type::elementwiseProduct (X, E, F);
  elemental_adaptor_type::add(F, F, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 14. d/dx(logdet(X.*X')) =
          X'.*(X.*X')'^-1 + X'.* (X.*X')^-1 */
  func = logdet(elementwiseProduct (fX1, transpose(fX1)));
  elemental_adaptor_type::elementwiseProduct(X, XT, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::transpose (D, E);
  elemental_adaptor_type::elementwiseProduct (X, XT, F);
  elemental_adaptor_type::inv (F, G);
  elemental_adaptor_type::add (E, G, H);
  elemental_adaptor_type::elementwiseProduct (XT, H, RESULT);
  CHECK_ELEMENTAL_MATRIX (func.derivativeVal, RESULT);

  /** 15. d/dx(trace(x * trace(x))) */
  func = trace(fX * trace(fX));
  /** 16. d/dx(trace(trace(x) * x))*/
  func = trace(trace(fX) * fX);
}

#endif // AMD_HAVE_ELEMENTAL

void testBasicSymbolicMatrixMatrixFunc() {
  std::string ans; /**< SymbolicMatrixMatrixFunc result. */
//  std::string row = std::to_string(ROW); /**< SymbolicMatrix row number. */ 
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;
  /** Create a variable X and an identity function */
  symbolic_matrix_type X("X",ROW,COL);
  SymbolicMMFunc fX(X,false); 

  /** Create a constant A and an identity function */
  symbolic_matrix_type A("A",ROW,COL);
  symbolic_matrix_type Z = symbolic_adaptor_type::zeros(ROW, COL);
  SymbolicMMFunc fA(A,true); 
  SymbolicMMFunc fZ(Z, true);

  /** Create a scalar-matrix function placeholder */ 
  SymbolicSMFunc func;

  /** 1. d/dx(trace(X*A)) = A' */
  ans = "A'";
  func = trace(fX*fA);
  assert(func.derivativeVal.getString()==ans);

  SymbolicSMFunc func2;


  /** 2. d/dx(trace(X' * A')) = A' */
  ans = "A'";
  func = trace(transpose(fX)*transpose(fA));
  assert(func.derivativeVal.getString()==ans);

  /** 3. d/dx(trace((X*A)')) = A' */
  ans = "A'";
  func = trace(transpose(fX*fA));
  assert(func.derivativeVal.getString()==ans);

  /** 4. d/dx(trace(X*A')) = A */
  ans = "A";
  func = trace(fX*transpose(fA));
  assert(func.derivativeVal.getString()==ans);

  /** 5. d/dx(trace(A*X')) = A */
  ans = "A";
  func = trace(fA*transpose(fX));
  assert(func.derivativeVal.getString()==ans);

  /** 6. d/dx(trace(X)) = I */
  ans = "eye(" + row + ")";
  func = trace(fX);
  assert(func.derivativeVal.getString()==ans);

  /** 7. d/dx(trace(A+X'+A)) = I */
  ans = "eye(" + row + ")";
  func = trace(fA+transpose(fX)+fA);
  assert(func.derivativeVal.getString()==ans);

  /** 8. d/dx(trace(A*(X')^-1)) = -(X')^-1*A*X^-1 */
  ans = "(-(inv(X)*(A*inv(X))))'";
  func = trace(fA*inv(fX));
  assert(func.derivativeVal.getString()==ans);

  /** 9. d/dx(trace(A-X)) = I */
  ans = "(-eye(" + row + "))";
  func = trace(fA-fX);
  assert(func.derivativeVal.getString()==ans);

  /** 10. d/dx(logdet(X)) = X^-1 */
  ans = "inv(X)'";
  func = logdet(fX);
  assert(func.derivativeVal.getString()==ans);

  /** 11. d/dx(logdet(X')) = X^-1 */
  ans = "inv(X)'";
  func = logdet(transpose(fX));
  assert(func.derivativeVal.getString()==ans);

  /** 12. d/dx(logdet(A+X)) = (A+X)^-1 */
  ans = "inv(A+X)'";
  func = logdet(fA+fX);
  assert(func.derivativeVal.getString()==ans);
  /** 13. d/dx(logdet(A-X)) = -((A-X)')^-1 */
  ans = "(-inv(A-X))'";
  func = logdet(fA-fX);
  assert(func.derivativeVal.getString()==ans);

  /** 14. d/dx(logdet(X'^-1)) = -X'^-1 */
  ans = "(-inv(X)')";
  func = logdet(inv(transpose(fX)));
  assert(func.derivativeVal.getString()==ans);

  func = func * func;
  std::cout << func.functionVal.getString() << std::endl;
  std::cout << func.derivativeVal.getString() << std::endl;
}
void testDerivativeSymbolicMatrixMatrixFunc() {
  std::string ans; /**< SymbolicMatrixMatrixFunc result. */
//  std::string row = std::to_string(ROW); /**< SymbolicMatrix row number. */ 
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;

  /** Create a variable X and an identity function */
  symbolic_matrix_type X("X",ROW,COL);
  SymbolicMMFunc fX(X,false); 

  /** Create a constant A and an identity function */
  symbolic_matrix_type A("A",ROW,COL);
  symbolic_matrix_type Z = symbolic_adaptor_type::zeros(ROW, COL);
  SymbolicMMFunc fA(A,true); 
  SymbolicMMFunc fZ(Z, true);

  /** Create a scalar-matrix function placeholder */ 
  SymbolicSMFunc func;
  SymbolicSMFunc func2;
  

  func = logdet(elementwiseProduct(fX, fX)); 
  func2 = trace((*func.derivativeFuncVal));
  std::cout << "test func matrix matrix func" << std::endl;
  std::cout << func.derivativeVal.getString() << std::endl;
  std::cout << func.derivativeFuncVal->matrixPtr->getString() << std::endl;
  std::cout << "test if trace(trace.derivativeFuncVal) works" << std::endl;
  std::cout << func2.derivativeVal.getString() << std::endl;
  std::cout << func2.derivativeFuncVal->matrixPtr->getString() << std::endl;
}

/** The function names need to be more descriptive; also add comments */
void testAdvancedSymbolicMatrixMatrixFunc () {

  std::string ans;
//  std::string row = std::to_string(ROW);
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;
  std::cout << std::endl;
  /** Create a variable X and an identity function */
  symbolic_matrix_type X("X", ROW, COL);
  SymbolicMMFunc fX(X, false);

  /** Create constants A,B and C and identity functions */
  symbolic_matrix_type A("A", ROW, COL);
  symbolic_matrix_type B("B", ROW, COL);
  SymbolicMMFunc fA(A, true);
  SymbolicMMFunc fB(B, true);

  /** Create a scalar-matrix function placeholder */
  SymbolicSMFunc func; 

  func = trace(fA*fX*fB*fX);
  std::cout << "Function:   " << func.functionVal.getString() << std::endl;
  std::cout << "Derivative: " << func.derivativeVal.getString() << std::endl;
 
  std::cout << std::endl;
  symbolic_matrix_type C("C", ROW, COL);
  SymbolicMMFunc fC(C, true);

  /** 1. d/dx(logdet(X))=X'^-1 */
  ans = "inv(X)'";
  func = logdet(fX);
  assert(func.derivativeVal.getString() == ans);

  /** 2. d/dx(logdet(AXB))=B*(AXB)^-1*A */
  ans = "((B*inv((A*X)*B))*A)'";
  func = logdet(fA*fX*fB);
  assert(func.derivativeVal.getString() == ans);

  /** 3. d/dx(logdet(X'*A*X)) = A*X*(X'*A*X)^-1 + ((X'*A*X)^-1*X'*A)' */
  ans = "((A*(X*inv((X'*A)*X)))+(inv((X'*A)*X)*(X'*A))')";
  func = logdet(transpose(fX) * fA * fX);
  assert(func.derivativeVal.getString() == ans);

  /** 4. d/dx(trace(A*X)) = A' */
  ans = "A'";
  func = trace (fA*fX);
  assert(func.derivativeVal.getString() == ans);

  /** 5. d/dx(trace(A*X*B*X)) = (B*X*A)' + (A*X*B)' */
  ans = "(((B*X)*A)'+((A*X)*B)')";
  func = trace (fA * fX * fB * fX);
  assert(func.derivativeVal.getString() == ans);

  /** 6. d/dx(trace(A*X^-1*B)) = (-X^-1*B*A*X^-1)' */
  ans = "(-(inv(X)*((B*A)*inv(X))))'";
  func = trace (fA * inv(fX) * fB);
  assert(func.derivativeVal.getString() == ans);
  
  /** 7. d/dx(trace(A*X*X)) = (X*A)' + (A*X) */ 
  ans = "((X*A)'+(A*X)')";
  func = trace (fA * fX * fX);
  assert(func.derivativeVal.getString() == ans);

  // Elemental matrices
  
  /** 8. d/dx(trace(A.*X)) = diag(A) */
  ans = "(A.*eye("+row + "))";
  func = trace (elementwiseProduct(fA, fX));
  assert(func.derivativeVal.getString() == ans);

  /** 9. d/dx(trace(X.*X)) = 2diag(X) */
  ans = "((X.*eye("+ row +"))+(X.*eye("+row+")))";
  func = trace (elementwiseProduct(fX, fX));
  assert(func.derivativeVal.getString() == ans);

  /** 10. d/dx(trace(X^3)) = 3diag(X.*X) */
  ans = "(((X.*(X.*eye("+row+")))+(X.*(X.*eye("+row + "))))+((X.*X).*eye("+ row +")))";
  func = trace (elementwiseProduct(elementwiseProduct(fX, fX), fX));
  assert(func.derivativeVal.getString() == ans);
 
  /** 11. d/dx(trace(X^-1.*X^-1)) = -2X'^-1*diag(X^-1)*X'^-1 */ 
  ans = "((-(inv(X)*((inv(X).*eye("+row+ \
  "))'*inv(X))))'+(-(inv(X)*((inv(X).*eye("+row+"))'*inv(X))))')";
  func = trace (elementwiseProduct(inv(fX), inv(fX)));
  assert(func.derivativeVal.getString() == ans);

  /** 12. d/dx(trace(X.*X^-1)) = diag(X^-1) - X'^-1*diag(X)*X'^-1 */
  ans = "((inv(X).*eye("+row+"))+(-(inv(X)*((X.*eye("+row+"))'*inv(X))))')";
  func = trace (elementwiseProduct(fX, inv(fX)));
  assert(func.derivativeVal.getString() ==ans);
 
  /** 13. d/dx(logdet(A.*X)) = A.*(A.*X)^-1 */ 
  ans = "(A'.*inv(A.*X))'";
  func = logdet (elementwiseProduct(fA, fX));
  assert(func.derivativeVal.getString() == ans);

  /** 14. d/dx(logdet(A * X')) = A'*(A.*X)^-1 */
  ans = "(A'.*inv(A.*X'))";
  func = logdet (elementwiseProduct(fA, transpose(fX)));
  assert(func.derivativeVal.getString() == ans);

  /** 15. d/dx(logdet(X.*X)) = 2*X.*(X.*X)'^-1 */
  ans = "((X'.*inv(X.*X))'+(X'.*inv(X.*X))')";
  func = logdet (elementwiseProduct(fX, fX));
  assert(func.derivativeVal.getString() == ans);
  
  /** 16. d/dx(logdet(X.*X')) = X'.*((X.*X')'^-1 + (X.*X')^-1)*/
  ans = "((X''.*inv(X.*X'))'+(X'.*inv(X.*X')))";
  func = logdet (elementwiseProduct(fX, transpose(fX)));
  assert(func.derivativeVal.getString() == ans);

//  func = logdet (fA + fB*fX*fC);
//  std::cout << func.derivativeVal.getString() << std::endl;
  
//  SymbolicMMFunc f4 = stack.top(); 
//  func = trace(f3);
//  std::cout << func.derivativeVal.getString() << std::endl;
  
  /** 17. d/dx(trace(X * trace(X) + X)) = 2trace(X).*I + I. */
  /*
  ans = "(((trace(X*eye(128)).*eye(128))+(trace(X).*eye(128))')+eye(128))";
  func = trace(fX * trace(fX) + fX);
  assert(func.derivativeVal.getString() == ans);
  */
  /** 18. d/dx(trace(X * trace(X))) = 2trace(X).*I. */
  /*
  ans = "((trace(X*eye(128)).*eye(128))+(trace(X).*eye(128))')";
  func = trace(fX * trace(fX));
  assert(func.derivativeVal.getString() == ans);
  */
  /** 19. d/dx(trace(trace(X) * X)) = 2trace(X).*I. */
  /*
  ans = "((trace(X*eye(128)).*eye(128))+(trace(X).*eye(128))')";
  func = trace(trace(fX) * fX);
  assert(func.derivativeVal.getString() == ans);
  */
  /** 20. d/dx(trace(trace(X) * X * X)) = trace(X*X).*I + 2trace(X).*X'. */
  /*
  ans = "(((trace(X*X).*eye(128)')'+(trace(X).*X)')+(trace(X).*X)')";
  func = trace(trace(fX) * fX * fX);
  assert(func.derivativeVal.getString() == ans);
  */
  /** 21. d/dx(trace(X * trace(X) * X)) = trace(X*X).*I + 2trace(X).*X'. */
  /*
  ans = "(((trace(X*X).*eye(128)')'+(trace(X).*X)')+(trace(X).*X)')";
  func = trace( fX * trace(fX) * fX);
  assert(func.derivativeVal.getString() == ans);
  */
  /** 22. d/dx(trace(X * X * trace(X))) = trace(X*X).*I + 2trace(X).*X'. */
  /*
  ans = "(((trace((X*X)*eye(128)).*eye(128))+(X*(trace(X).*eye(128)))')+((trace(X).*eye(128))*X)')";
  func = trace( fX * fX * trace(fX));
  assert(func.derivativeVal.getString() == ans); 
  */
}


void testTaylorExp() {

  std::string ans;
//  std::string row = std::to_string(ROW);
  char rowChar[5];
  int rowTmp = ROW;
  sprintf(rowChar, "%d", rowTmp);
  std::string row = rowChar;

  /** Create a variable X and an identity function */
  std::string r0 = "1", r1="1/2!", r2= "1/3!";
  symbolic_matrix_type X("X", ROW, COL);
  symbolic_matrix_type X0("X0", ROW, COL);
  symbolic_matrix_type Delta("(X-X0)", ROW, COL);
  SymbolicMMFunc fX(X, false);
  SymbolicMMFunc fX0(X0, false);
  SymbolicMMFunc fDelta(Delta, true);
  
  /* Compute n-order differentiation iteratively. */
  SymbolicSMFunc f0 =  logdet(fX0);
  SymbolicSMFunc f1 = trace(fDelta * transpose(*f0.derivativeFuncVal));
  SymbolicSMFunc f2 = trace(fDelta * transpose(*f1.derivativeFuncVal));
  SymbolicSMFunc f3 = trace(fDelta * transpose(*f2.derivativeFuncVal));

  SymbolicSMFunc func = f0 + f1 + r1*f2 + r2*f3;
  std::cout << "logdet(X)'s first 3 Taylor series is: " << std::endl;
  std::cout << func.functionVal.getString() << std::endl;
/*
  std::cout << "f0 functionVal    :   ";
  std::cout << f0.functionVal.getString() << std::endl;
  std::cout << "f0 derivativeVal  :   ";
  std::cout << f0.derivativeVal.getString() << std::endl;
  std::cout << "f1 functionVal    :";
  std::cout << f1.functionVal.getString() << std::endl;
  std::cout << "f1 derivativeVal  :";
  std::cout << f1.derivativeVal.getString() << std::endl;
  std::cout << "f2 functionVal    :";
  std::cout << f2.functionVal.getString() << std::endl;
  std::cout << "f2 derivativeVal  :";
  std::cout << f2.derivativeVal.getString() << std::endl;
  std::cout << "f3 functionVal    :";
  std::cout << f3.functionVal.getString() << std::endl;
  std::cout << "f3 derivativeVal  :";
  std::cout << f3.derivativeVal.getString() << std::endl;
*/
}

int main(int argc, char** argv) {

#if AMD_HAVE_ELEMENTAL
  elem::Initialize(argc, argv); 
#endif
//  std::cout << "Testing basic matrix-matrix functions .... ";
//  testBasicSymbolicMatrixMatrixFunc();
//  std::cout << "DONE" << std::endl;

//  std::cout << "Testing advanced matrix-matrix functions .... ";
//  testAdvancedSymbolicMatrixMatrixFunc();
//  std::cout << "DONE" << std::endl;

#if AMD_HAVE_ELEMENTAL
//  std::cout << "Testing elemetal matrix-matrix functions .... ";
//  testElementalMatrixMatrixFunc();
//  std::cout << "DONE" << std::endl;
#endif
//  testFXgX() ;

//  std::cout << "Test Taylor Expansion ...." << std::endl;
  testTaylorExp();
//  std::cout << "DONE" << std::endl;
//  std::cout << "All tests passed." << std::endl;
//  testDerivativeSymbolicMatrixMatrixFunc();
#if AMD_HAVE_ELEMENTAL
  elem::Finalize();
#endif

  return(0);
}
