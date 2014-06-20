#include <iostream>
#include <string>
#include <assert.h>
#include <elemental.hpp>
#include <boost/shared_ptr.hpp>

#include <AMD/AMD.hpp>

typedef elem::Matrix<double> matrix_type;
typedef AMD::MatrixAdaptor_t<matrix_type> adaptor_type;
typedef typename adaptor_type::value_type value_type;

/**
 * TODO: Add more extensive tests and ensure that the results are correct.
 */ 
void testElementalAdaptorBasic() {
  /** Define two matrices */
  matrix_type A(5,5);
  matrix_type B(5,5);
  matrix_type C(5,5);

  /** Fill A and B with random entries drawn from Uniform(0,1) */
  elem::MakeGaussian(A);
  elem::MakeGaussian(B);

  /** Negate and print results */
  adaptor_type::negation (A,C);
  adaptor_type::print(A);
  adaptor_type::print(C);

#if 0
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
#endif
}

void testElementalAdaptorAdvanced () {

}


int main(int argc, char** argv) {
  /** Initialize elemental */
  elem::Initialize(argc, argv);

  std::cout << "Testing basic adaptor functions .... ";
  testElementalAdaptorBasic();
  std::cout << "DONE" << std::endl;

  std::cout << "Testing advanced adaptor functions .... ";
  testElementalAdaptorAdvanced();
  std::cout << "DONE" << std::endl;

  std::cout << "All tests passed." << std::endl;

  /** Finalize elemental */
  elem::Finalize();

  return(0);
}
