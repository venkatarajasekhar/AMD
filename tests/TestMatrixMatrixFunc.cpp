#include <iostream>
#include <string>
#include <cmath>
#include <elemental.hpp>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include <AMD/AMD.hpp>

//#define DEBUG

#define ROW 4
#define COL 4

// Typedef for SymbolicMatrixMatlab and SymbolicScalarMatlab.
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef typename symbolic_adaptor_type::value_type symbolic_value_type;

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

void assert_close (double a, double b) {
  double error =  a-b;
  error = (0.0 > error) ? -error : error;
  assert (error <= 1e-9);
}
#define checkElementalMatrixEquality(x, y) CHECKElementalMatrixEquality(x, y)
void CHECKElementalMatrixEquality (const elemental_matrix_type& A, 
                                   const elemental_matrix_type& B) {
  #ifdef DEBUG
  // Print matrix A and matrix B to screen
  std::cout << "Matrix A " << std::endl;
  for (int i = 0; i < elemental_adaptor_type::getNumRows(A); i++) {
    for (int j = 0; j < elemental_adaptor_type::getNumCols(A); j++) {
      std::cout << A.Get(i, j) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Matrix B " << std::endl;
  for (int i = 0; i < elemental_adaptor_type::getNumRows(B); i++) {
    for (int j = 0; j < elemental_adaptor_type::getNumCols(B); j++) {
      std::cout << B.Get(i, j) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  #endif
  assert((elemental_adaptor_type::getNumRows(A) ==
          elemental_adaptor_type::getNumRows(B)) &&
          (elemental_adaptor_type::getNumCols(A) == 
          elemental_adaptor_type::getNumCols(B)));
  
  for (int i = 0; i < elemental_adaptor_type::getNumRows(A); i++) {
    for (int j = 0; j < elemental_adaptor_type::getNumCols(A); j++) {
      assert_close(A.Get(i,j), B.Get(i,j));
    }
  }
}

/**
 * @brief Test numerical matrix derivatives.
 */
void testElementalMatrixMatrixFunc() {

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
  elemental_matrix_type EYE = elemental_adaptor_type::eye(ROW); 
  elemental_matrix_type RESULT(ROW, COL); /**< Hand calculated matrix result. */
  elemental_matrix_type X(ROW, COL);

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


  elem::MakeGaussian(A);
  elem::MakeGaussian(B);
  elem::MakeGaussian(X);

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
  
  ElementalMMFunc fA(A, true);
  ElementalMMFunc fB(B, true);
  ElementalMMFunc fX(X, false);
  ElementalSMFunc func;

  /** 1. d/dx(trace(AX)) = A'; */
  func = trace(fA * fX);
  elemental_adaptor_type::copy(RESULT, AT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);


  /** 2. d/dx(trace(AXBX)) = A'X'B' + B'X'A' */
  func = trace(fA * fX * fB * fX);
  elemental_adaptor_type::multiply (AT, XT, C);
  elemental_adaptor_type::multiply (C, BT, E);
  elemental_adaptor_type::multiply (BT, XT, D);
  elemental_adaptor_type::multiply (D, AT, F);
  elemental_adaptor_type::add (E, F, RESULT); 
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 3. d/dx(trace(AX'B)) = -(X^-1)'A'B'(X^-1)' */
  func = trace(fA * inv(fX) * fB);
  elemental_adaptor_type::multiply (XIT, AT, C);
  elemental_adaptor_type::multiply (C, BT, D);
  elemental_adaptor_type::multiply (D, XIT, E);
  elemental_adaptor_type::negation (E, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 4. d/dx(trace(A.*X)) = A.*eye(N) */
  func = trace(elementwiseProduct (fA, fX));
  elemental_adaptor_type::elementwiseProduct (A, EYE, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 5. d/dx(trace (X.*X)) =  2 * A .* eye(N) */
  func = trace (elementwiseProduct (fX, fX));
  elemental_adaptor_type::elementwiseProduct(X, EYE, C);
  elemental_adaptor_type::add(C, C, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 6. d/dx(trace(X^-1 .* X^-1)) = 
         -2 * X^-T * X^-1 .* EYE * X ^ -T    */
  func = trace (elementwiseProduct(inv(fX), inv(fX)));
  elemental_adaptor_type::elementwiseProduct (XI, EYE, C);
  elemental_adaptor_type::multiply (XIT, C, D);
  elemental_adaptor_type::multiply (D, XIT, E);
  elemental_adaptor_type::add (E, E, F);
  elemental_adaptor_type::negation (F, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 7. d/dx(trace(X .* X^-1)) = 
         X^-1 .* EYE - X^-T * X .* EYE * X^-T */
  func = trace (elementwiseProduct (fX, inv(fX)));
  elemental_adaptor_type::elementwiseProduct (XI, EYE, C);
  elemental_adaptor_type::elementwiseProduct (X, EYE, D);
  elemental_adaptor_type::multiply (XIT, D, E);
  elemental_adaptor_type::multiply (E, XIT, F);
  elemental_adaptor_type::minus (C, F, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  


  // For logdet test case we pick specific matrices as test cases
  // because logdet variable must be positive.
  // Here we pick X={2, 1, 2, 3}, A = {2, -1, 0, 1}, B = {3, -1, 1, 2}
  //
  // Reset matrix Matrix should be symmetric and positive.
 
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
  checkElementalMatrixEquality(func.derivativeVal, XIT);

  /** 9. d/dx(logdet(AXB)) = A'((AXB)^-1)'B' */
  func = logdet(fA1 * fX1 * fB1);
  elemental_adaptor_type::multiply (A, X, C);
  elemental_adaptor_type::multiply (C, B, D);
  elemental_adaptor_type::inv (D, E);
  elemental_adaptor_type::transpose (E, F);
  elemental_adaptor_type::multiply (AT, F, G);
  elemental_adaptor_type::multiply (G, BT, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT); 

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
  checkElementalMatrixEquality (func.derivativeVal, RESULT); 

  /** 11. d/dx(logdet(A.*X)) = A.*(A.*X)^-T*/
  func = logdet(elementwiseProduct (fA1, fX1));
  elemental_adaptor_type::elementwiseProduct (A, X, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::transpose (D, E);
  elemental_adaptor_type::elementwiseProduct (A, E, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 12. d/dx(logdet(A.*X')) = A'.*(A.*X')^-1 */
  func = logdet(elementwiseProduct(fA1, transpose(fX1)));
  elemental_adaptor_type::elementwiseProduct (A, XT, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::elementwiseProduct (AT, D, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

  /** 13. d/dx(logdet(X.*X)) = 2 * X.*(X.*X)'^-1 */
  func = logdet(elementwiseProduct (fX1, fX1));
  elemental_adaptor_type::elementwiseProduct (X, X, C);
  elemental_adaptor_type::inv (C, D);
  elemental_adaptor_type::transpose (D, E);
  elemental_adaptor_type::elementwiseProduct (X, E, F);
  elemental_adaptor_type::add(F, F, RESULT);
  checkElementalMatrixEquality (func.derivativeVal, RESULT);

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
  checkElementalMatrixEquality (func.derivativeVal, RESULT);
}

#endif // AMD_HAVE_ELEMENTAL

/** Complete writing out the descriptions */
/** Also, change the name of the constant to be A, instead of Y */
void testBasicSymbolicMatrixMatrixFunc() {
  std::string ans; /**< SymbolicMatrixMatrixFunc result. */
  std::string row = std::to_string(ROW); /**< SymbolicMatrix row number. */ 

  /** Create a variable X and an identity function */
  symbolic_matrix_type X("X",ROW,COL);
  SymbolicMMFunc fX(X,false); 

  /** Create a constant A and an identity function */
  symbolic_matrix_type A("A",ROW,COL);
  SymbolicMMFunc fA(A,true); 

  /** Create a scalar-matrix function placeholder */ 
  SymbolicSMFunc func;

  /** 1. d/dx(trace(X*A)) = A' */
  ans = "A'";
  func = trace(fX*fA);
  assert(func.derivativeVal.getString()==ans);

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
  std::cout << func.derivativeVal.getString() << "  " <<  ans <<std::endl;
  assert(func.derivativeVal.getString()==ans);

  /** 9. d/dx(trace(A-X)) = I */
  ans = "(-eye(" + row + "))";
  func = trace(fA-fX);
  std::cout << func.derivativeVal.getString() << "  " <<  ans << std::endl;
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
}

/** The function names need to be more descriptive; also add comments */
void testAdvancedSymbolicMatrixMatrixFunc () {

  std::string ans;
  symbolic_matrix_type X("X", 2, 2);
  symbolic_matrix_type A("A", 2, 2);
  symbolic_matrix_type B("B", 2, 2);
  symbolic_matrix_type C("C", 2, 2);
  SymbolicMMFunc fX(X, false);
  SymbolicMMFunc fA(A, true);
  SymbolicMMFunc fB(B, true);
  SymbolicMMFunc fC(C, true);

  SymbolicMMFunc fBX = fB * fX;
  SymbolicMMFunc fAX1 = fA * fX;
  SymbolicMMFunc fAXB = fAX1 * fB;
  SymbolicMMFunc fCX = fC * fX;
  SymbolicMMFunc fDX = fBX + fCX;
  SymbolicMMFunc fAXDX = fA * fDX;

  SymbolicSMFunc func = trace(fBX);

  std::cout << "function" << std::endl;
  std::cout << func.functionVal.getString() << std::endl;
  std::cout << "derivative" << std::endl;
  std::cout << func.derivativeVal.getString() << std::endl;
  std::cout << "---- logdet tests ---- " << std::endl;
  SymbolicSMFunc func4; 
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
  
  func4 = trace (fA * fX * fX);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  // Element-wise Production test cases
  std::cout << "Now lets test some cases for element-wise product" << std::endl
   << std::endl;
  SymbolicMMFunc eTest0 = AMD::elementwiseProduct(fA, fX);
  func4 = trace (eTest0);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = trace(fA * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest1 = AMD::elementwiseProduct(fX, fX);
  func4 = trace (eTest1);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest2 = AMD::elementwiseProduct(AMD::elementwiseProduct(fX, fX), fX);
  func4 = trace (eTest2);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
//  func4 = trace(fX * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  SymbolicMMFunc eTest3 = AMD::elementwiseProduct(inv(fX), inv(fX));
  func4 = trace (eTest3);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
//  func4 = trace(inv(fX) * inv(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest4 = AMD::elementwiseProduct(fX, inv(fX));
  func4 = trace (eTest4);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
  
//  func4 = trace(fX * inv(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest5 = AMD::elementwiseProduct(fA, fX);
  func4 = logdet (eTest5);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fA * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest6 = AMD::elementwiseProduct(fA, transpose(fX));
  func4 = logdet (eTest6);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fA * transpose(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest7 = AMD::elementwiseProduct(fX, fX);
  func4 = logdet (eTest7);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
//  func4 = logdet(fX * fX);
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

  SymbolicMMFunc eTest8 = AMD::elementwiseProduct(fX, transpose(fX));
  func4 = logdet (eTest8);
  std::cout << func4.functionVal.getString() << std::endl;
  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;

//  func4 = logdet(fX * transpose(fX));
//  std::cout << func4.functionVal.getString() << std::endl;
//  std::cout << func4.derivativeVal.getString() << std::endl << std::endl;
}

int main(int argc, char** argv) {

  elem::Initialize(argc, argv); 
  std::cout << "Testing basic matrix-matrix functions .... ";
  testBasicSymbolicMatrixMatrixFunc();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing advanced matrix-matrix functions .... ";
  testAdvancedSymbolicMatrixMatrixFunc();
  std::cout << "DONE" << std::endl;

#if AMD_HAVE_ELEMENTAL
  std::cout << "Testing elemetal matrix-matrix functions .... ";
  testElementalMatrixMatrixFunc();
  std::cout << "DONE" << std::endl;
#endif

  std::cout << "All tests passed." << std::endl;
  elem::Finalize();

  return(0);
}
