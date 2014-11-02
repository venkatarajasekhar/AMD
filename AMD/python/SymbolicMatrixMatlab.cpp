/**
 * @file SymbolicMatrixMatlab.cpp
 * @author adromanova
 * 
 * Python bindings for the SymbolicMatrixMatlab object and helper functions.
 */

#include <boost/python.hpp>
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD { namespace python {

/**
 * Automatically wrap overloaded member methods 
 * and member methods with default parameters.
 */
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(print_overloads, print, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(println_overloads, println, 0, 1)

/**
 * A function to export all the definitions for SymbolicMatrixMatlab object 
 * and helper functions.
 */
void export_symbolicmatrixmatlab () {

  using namespace boost::python;
  using namespace AMD;

  /**
   * docstring
   */
  const char* symMatrMat_docstring = 
    "This class represents a symbolic matrix using an internal string\n" \
    "representation. The string representation can be a variable name\n" \
    "such as X, HouseHolder or a derived value sich as transpose(X)*X,\n" \
    "where X is a symbolic matrix. The string representaion is intended to\n" \
    "yield executable MATLAB code, but the variables representing matrices\n" \
    "must already be defined.";

  const char* eye_docstring = 
    "Returns representaion of nxn identity matrix.\n" \
    "Args:\n" \
    "    n: size of the idetity matrix\n" \
    "Returns:\n" \
    "    SymbolicMatrixMatlab eye(n)";

  const char* zeros_docstring = 
    "Returns representation of mxn zero matrix.\n" \
    "Args:\n" \
    "    m: number of rows\n" \
    "    n: number of columns\n" \
    "Returns:\n" \
    "    SymbolicMatrixMatlab zeros(m, n)";

  const char* trace_docstring = 
    "Computes the scalar trace(a) from matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymbolicScalarMatlab representaion trace(a)";

  const char* logdet_docstring = 
    "Compute scalar logdet(a) from matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymbolicScalarMatlab representaion logdet(a)";

  const char* fnorm_docstring = 
    "Computes the scalar frobenius norm from the matrix a\n."
    "Args:\n" \
    "    a: symbolic matrix argument\n" \
    "Returns:\n" \
    "    SymbolicScalarMatlab representaion of ||a||_F";

  const char* inv_docstring = 
    "Computes the inverse matrix of the matrix a.\n" \
    "Agrs:\n" \
    "    SymbolicMatrixMatlab  representaion of inv(a)";

  const char* transpose_docstring = 
    "Computes the transposed matrix of the matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix argument.\n" \
    "Returns:\n" \
    "    SymbolicMatrixMatlab representation of a^T";

  const char* diag_docstring = 
    "Compute the diagonal matrix of matrix a.\n" \
    "Agrs:\n" \
    "    a: symbolic matrix argument\n" \
    "Returns:\n" \
    "    SymbolicMatrixMatlab representaion of diag(a)";

  const char* elementwiseProduct_docstring = 
    "Compute elementwise product between a and b.\n" \
    "Args:\n" \
    "    a: symbolic matrix argument\n" \
    "    b: another symbolic matrix argument\n" \
    "Returns:\n" \
    "    SymbolicMatrixMatlab representiaon of a.*b.";  

  // export SymbolicMatricMatlab
  class_<SymbolicMatrixMatlab>("symMatrMat", symMatrMat_docstring, init<>())
    .def(init<int, int>())
  	.def(init<std::string>())
    .def(init<std::string, int, int>())
    .def(init<const SymbolicMatrixMatlab &>())
    //TODO Drop Print method?
  	.def("Print", &SymbolicMatrixMatlab::print, print_overloads())
  	.def("println", &SymbolicMatrixMatlab::println, println_overloads())
    // static class functions
    .def("eye", &SymbolicMatrixMatlab::eye, eye_docstring)
    .staticmethod("eye")
    .def("zeros", &SymbolicMatrixMatlab::zeros, zeros_docstring)
    .staticmethod("zeros")
    // export member variables as properties
    .add_property("string",  
                  &SymbolicMatrixMatlab::getString, 
                  "internal string representaiton")
    .add_property("numRows", 
                  &SymbolicMatrixMatlab::getNumRows, 
                  "number of rows")
    .add_property("numCols", 
                  &SymbolicMatrixMatlab::getNumCols,
                  "number of columns")
    // export overloaded operators
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(SymbolicScalarMatlab() * self)
    .def(self * SymbolicScalarMatlab())
    .def(self / SymbolicScalarMatlab())
  ;

  // export helper functions
  def("trace", trace, trace_docstring);
  def("logdet", logdet, logdet_docstring);
  def("fnorm", fnorm, fnorm_docstring);
  def("inv", inv, inv_docstring);
  def("transpose", transpose, transpose_docstring);
  def("diag", diag, diag_docstring);
  def("elementwiseProduct", elementwiseProduct, elementwiseProduct_docstring);

}

} } /* namespace AMD::python */
