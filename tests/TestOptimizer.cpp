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
symbolic_matrix_type zero("0");
SymbolicMMFunc symbolicZeroMMFunc(zero,false);
symbolic_matrix_type identityMatrix("I");
SymbolicMMFunc symbolicIdentityMMFunc(identityMatrix,false);
void testMultiplicationOptimizations() {
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);
  /*Case 1 M*0 */
  SymbolicMMFunc fX_times_zero = fX*symbolicZeroMMFunc;
  SymbolicMMFunc* fX_times_zero_optimized = AMD::multiplyOpt(fX_times_zero);
  assert(fX_times_zero_optimized.mTpye == kZero);
  printf("\t-Passed M*0 test\n");
  
  /*Case 2 0*M */
  SymbolicMMFunc zero_times_fX = symbolicZeroMMFunc*fX;
  SymbolicMMFunc* zero_times_fX_optimized = AMD::multiplyOpt(zero_times_fX);
  assert(zero_times_fX_optimized.mType == kZero);
  printf("\t-Passed 0*M test\n");
  
  /*Case 3 M*I */
  SymbolicMMFunc fX_times_identity = fX*symbolicIdentityMMFunc;
  SymbolicMMFunc* fX_times_identity_optimized = 
      AMD::multiplyOpt(fX_times_identity);
  assert(fX_times_identity_optimized.mType == kIdentity); 
  printf("\t-Passed M*I test\n");
  /*Case 4 I*M */
  SymbolicMMFunc identity_times_fX = symbolicIdentityMMFunc*fX;
  SymbolicMMFunc* identity_times_fX_optimized = 
      AMD::multiplyOpt(identity_times_fX);
  assert(identity_times_fX_optimized.mType == kIdentity);
  printf("\t-Passed I*M test\n");
  /*Case 5 M*INV(M) */
  SymbolicMMFunc invfX = inv(fX);
  SymbolicMMFunc M_times_InvM = fX*invfX;
  SymbolicMMFunc* M_times_InvM_optimized =
      AMD::multiplyOpt(M_times_InvM);
  assert(M_times_InvM_optimized.mTpye == kIdentity);
  printf("\t-Passed M*inv(M) test\n");
  /*Case 6 INV(M)*M */
  SymbolicMMFunc InvM_times_M = invfX*fX;
  SymbolicMMFunc* InvM_times_M_optimized =
      AMD::multiplyOpt(InvM_times_M);
  assert(InvM_times_M_optimized.mType == kIdentity);
  printf("\t-Passed inv(M)*M test\n");

}
void testAdditionOptimizations(){
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);
  /* Case 1: fX + 0 */
  SymbolicMMFunc fX_plus_0 = fX+symbolicZeroMMFunc;
  SymbolicMMFunc* fX_plus_0_optimized = AMD::plusOpt(fX_plus_0);
  assert(fX_plus_0_optimized == fX);
  printf("\t-Passed X+0 test\n");
  /* Case 2: 0 + fX */
  SymbolicMMFunc zero_plus_fX = symbolicZeroMMFunc + fX;
  SymbolicMMFunc* zero_plus_fX_optimized = AMD::plusOpt(zero_plus_fX);
  assert(zero_plus_fX_optimized == fX);
  printf("\t-Passed 0+X test\n");

  /* Case 3: 0 + 0 */
  SymbolicMMFunc zero_plus_zero = symbolicZeroMMFunc+symbolicZeroMMFunc;
  SymbolicMMFunc* zero_plus_zero_optimized 
    = AMD::plusOpt(zero_plus_zero);
  assert(zero_plus_zero_optimized.mType);
  printf("\t-Passed 0+0 test\n");

}

void testSubtractionOptimizations() {
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);
  /* Case 1: fX - fX */
  SymbolicMMFunc fX_minus_fX = fX-fX;
  SymbolicMMFunc* fX_minus_fX_optimized = AMD::minusOpt(fX_minus_fX);
  assert(fx_minus_fX_optimized.mType);
  printf("\t-Passed X-X test\n");
  /* Case 2: 0 - fX */
  SymbolicMMFunc zero_minus_fX = symbolicZeroMMFunc-fX;
  SymbolicMMFunc* zero_minus_fX_optimized = AMD::minusOpt(zero_minus_fX);
  assert(zero_minus_fX_optimized.opNum == NEGATION);
  assert(zero_minus_fX_optimized.leftChild == fX);
  printf("\t-Passed 0-X test\n");
  /* Case 3: fx - 0 */
  SymbolicMMFunc fX_minus_zero = fX - symbolicZeroMMFunc;
  SymbolicMMFunc* fX_minus_zero_optimized = AMD::minusOpt(fX_minus_zero);
  assert(zero_minus_fX_optimized ==fX);
  printf("\t-Passed X-0 test\n");
 

}

void testTransposeOptimizations() {
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);
  SymbolicMMFunc trans_of_trans_of_fX = transpose(transpose(fX));
  SymbolicMMFunc* trans_of_trans_of_fX_optimized = 
                             AMD::transOpt(trans_of_trans_of_fX);
  assert(trans_of_trans_of_fX == fX);
  printf("\t-Passed trans(trans(X)) test\n");
 
  SymbolicMMFunc trans_of_zero = transpose(symbolicZeroMMFunc); 
  assert(trans_of_zero == symbolicZeroMMFunc);
  printf("\t-Passed trans(0) test\n");
  
  SymbolicMMFunc trans_of_identity = transpose(symbolicIdentityMMFunc); 
  assert(trans_of_identity == symbolicIdentityMMFunc);
  printf("\t-Passed trans(I) test\n");
}

void testInverseOptimizations() {
  symbolic_matrix_type X("A");
  SymbolicMMFunc fX(X,true);
  SymbolicMMFunc inv_of_inv_of_fX = inv(inv(fX));
  SymbolicMMFunc* inv_of_inv_of_fX_optimized = 
                             AMD::transOpt(inv_of_inv_of_fX);
  assert(inv_of_inv_of_fX == fX);
  printf("\t-Passed inv(inv(X)) test\n");
 
  SymbolicMMFunc inv_of_zero = inv(symbolicZeroMMFunc); 
  assert(inv_of_zero == symbolicZeroMMFunc);
  printf("\t-Passed inv(0) test\n");
  
  SymbolicMMFunc inv_of_identity = inv(symbolicIdentityMMFunc); 
  assert(inv_of_identity == symbolicIdentityMMFunc);
  printf("\t-Passed inv(I) test\n");

}


int main(int argc, char** argv) {

  symbolicZeroMMFunc.mType = AMD::kZero;
  symbolicIdentityMMFunc.mType = AMD::kIdentity;
  std::cout << "Testing matrix-product optimizations ....\n";
  testMultiplicationOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-add optimizations ....\n";
  testAdditionOptimizations();
  std::cout << "DONE" << std::endl;
  std::cout << "Testing matrix-minus optimizations ....\n";
  testSubtractionOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-transpose optimizations ....\n";
  testTransposeOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing matrix-inverse optimizations ....\n";
  testInverseOptimizations();
  std::cout << "DONE" << std::endl;

  std::cout << "DONE" << std::endl;
  std::cout << "All tests passed." << std::endl;
  return(0);
}
