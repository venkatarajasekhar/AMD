#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include <AMD/AMD.hpp>

/** Let us first typedef the Symbolic matrices and scalars we will be using */
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef symbolic_adaptor_type::value_type symbolic_value_type;
typedef AMD::MatrixMatrixFunc<symbolic_matrix_type,
                      				symbolic_value_type> SymbolicMMFunc;
typedef AMD::ScalarMatrixFunc<symbolic_matrix_type,
                       				symbolic_value_type> SymbolicSMFunc;
static int ROW = 4, COL = 4;


void testMultiplicationOptimizations() {
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);

  /*Case 1 M*0 */
  SymbolicMMFunc fX_times_zero = fX*symbolicZeroMMFunc;
  SymbolicMMFunc* fX_times_zero_optimized = AMD::multiplyOpt(fX_times_zero);
  assert(fX_times_zero_optimized == symbolicZeroMMFunc);
  printf("Passed M*0 test\n");
  
  /*Case 2 0*M */
  SymbolicMMFunc zero_times_fX = symbolicZeroMMFunc*fX;
  SymbolicMMFunc* zero_times_fX_optimized = AMD::multiplyOpt(zero_times_fX);
  assert(zero_times_fX_optimized == symbolicZeroMMFunc);
  printf("Passed 0*M test\n");
  
  /*Case 3 M*I */
  SymbolicMMFunc fX_times_identity = fX*symbolicIdentityMMFunc;
  SymbolicMMFunc* fX_times_identity_optimized = 
      AMD::multiplyOpt(fX_times_identity);
  assert(fX_times_identity_optimized == symbolicIdentityMMFunc); 
  printf("Passed M*I test\n");
  /*Case 4 I*M */
  SymbolicMMFunc identity_times_fX = symbolicIdentityMMFunc*fX;
  SymbolicMMFunc* identity_times_fX_optimized = 
      AMD::multiplyOpt(identity_times_fX);
  assert(identity_times_fX_optimized == symbolicIdentityMMFunc);
  printf("Passed I*M test\n");
  /*Case 5 M*INV(M) */
  SymbolicMMFunc invfX = inv(fX);
  SymbolicMMFunc M_times_InvM = fX*invfX;
  SymbolicMMFunc* M_times_InvM_optimized =
      AMD::multiplyOpt(M_times_InvM);
  assert(M_times_InvM_optimized == symbolicIdentityMMFunc);
  printf("Passed M*inv(M) test\n");
}

void testAdditionOptimizations() {
  symbolic_matrix_type X("A",ROW,COL);
  SymbolicMMFunc fX(X,true);

  SymbolicMMFunc fX_plus_fX = fX+fX;
  SymbolicMMFunc* fX_plus_fX_optimized = AMD::plusMinusOpt(fX_plus_fX);
}

void testSubtractionOptimizations() {
  symbolic_matrix_type X("A",ROW,COL);
  SymbolicMMFunc fX(X,true);

  SymbolicMMFunc fX_minus_fX = fX-fX;
  SymbolicMMFunc* fX_minus_fX_optimized = AMD::plusMinusOpt(fX_minus_fX);
}

void testTransposeOptimizations() {
  symbolic_matrix_type X("A",ROW,COL);
  SymbolicMMFunc fX(X,true);

  SymbolicMMFunc trans_of_trans_of_fX = transpose(transpose(fX));
  SymbolicMMFunc* trans_of_trans_of_fX_optimized = 
                             AMD::transOpt(trans_of_trans_of_fX);
}

void testInverseOptimizations() {
  symbolic_matrix_type X("A",ROW,COL);
  SymbolicMMFunc fX(X,true);

  SymbolicMMFunc inv_of_inv_of_fX = inv(inv(fX));
  SymbolicMMFunc* inv_of_inv_of_fX_optimized = AMD::inverseOpt(inv_of_inv_of_fX);
}


int main(int argc, char** argv) {

  std::cout << "Testing matrix-product optimizations ....\n";
  testMultiplicationOptimizations();
  std::cout << "DONE" << std::endl;
/*
  std::cout << "Testing matrix-add optimizations .... ";
  testAdditionOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-minus optimizations .... ";
  testSubtractionOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-transpose optimizations .... ";
  testTransposeOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-inverse optimizations .... ";
  testInverseOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "DONE" << std::endl;
  std::cout << "All tests passed." << std::endl;
*/
  return(0);
}
