/**
 * @file SymbolicMatrixMatlab.cpp
 * @author adromanova
 * 
 * Python bindings for SymbolicMatrixMatlab and helper functions.
 */

#include <boost/python.hpp>
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD { namespace python {

/**
 * A function to export SymbolicMatrixMatlab and helper functions.
 */
void export_symbolicmatrixmatlab () {

  using namespace boost::python;
  using namespace AMD;

  /**
   * docstring
   */
  const char* SymMat_docstring = 
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
    "    SymMat representation of eye(n)";

  const char* zeros_docstring = 
    "Returns representation of mxn zero matrix.\n" \
    "Args:\n" \
    "    m: number of rows\n" \
    "    n: number of columns\n" \
    "Returns:\n" \
    "    SymMat representation of zeros(m, n)";

  const char* trace_docstring = 
    "Computes the scalar trace(a) from matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymMat representation of trace(a)";

  const char* logdet_docstring = 
    "Compute scalar logdet(a) from matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymMat representation of logdet(a)";

  const char* fnorm_docstring = 
    "Computes the scalar frobenius norm from the matrix a\n."
    "Args:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymMat representaion of ||a||_F";

  const char* inv_docstring = 
    "Computes the inverse matrix of the matrix a.\n" \
    "Agrs:\n" \
    "    a: symbolic matrix\n" \
    "Returns:\n" \
    "    SymMat representation of inv(a)";

  const char* transpose_docstring = 
    "Computes the transposed matrix of the matrix a.\n" \
    "Args:\n" \
    "    a: symbolic matrix argument.\n" \
    "Returns:\n" \
    "    SymMat representation of a^T";

  const char* diag_docstring = 
    "Compute the diagonal matrix of matrix a.\n" \
    "Agrs:\n" \
    "    a: symbolic matrix argument\n" \
    "Returns:\n" \
    "    SymMat representaion of diag(a)";

  const char* elementwise_product_docstring = 
    "Compute elementwise product between a and b.\n" \
    "Args:\n" \
    "    a: symbolic matrix argument\n" \
    "    b: another symbolic matrix argument\n" \
    "Returns:\n" \
    "    SymMat representiaon of a.*b.";  

  // export SymbolicMatricMatlab
  class_<SymbolicMatrixMatlab>("SymMat", SymMat_docstring, init<>())
    .def(init<int, int>())
  	.def(init<std::string>())
    .def(init<std::string, int, int>())
    .def(init<const SymbolicMatrixMatlab &>())
    // export static class functions
    .def("eye", &SymbolicMatrixMatlab::eye, args("n"), eye_docstring)
    .staticmethod("eye")
    .def("zeros", 
        &SymbolicMatrixMatlab::zeros, 
        args("m", "n"), 
        zeros_docstring)
    .staticmethod("zeros")
    // export data members as properties
    .add_property("string",  
                  &SymbolicMatrixMatlab::getString, 
                  "internal string representaiton")
    .add_property("num_rows", 
                  &SymbolicMatrixMatlab::getNumRows, 
                  "number of rows")
    .add_property("num_cols", 
                  &SymbolicMatrixMatlab::getNumCols,
                  "number of columns")
    // export overloaded operators
    .def(self + self)
    .def(self - self)
    .def(-self)
    .def(self * self)
    .def(SymbolicScalarMatlab() * self)
    .def(self * SymbolicScalarMatlab())
    .def(self / SymbolicScalarMatlab())
    .def("__str__", &SymbolicMatrixMatlab::getString)
  ;

  // export helper functions
  def("trace", trace, args("a"), trace_docstring);
  def("logdet", logdet, args("a"), logdet_docstring);
  def("fnorm", fnorm, ("a"), fnorm_docstring);
  def("inv", inv, args("a"), inv_docstring);
  def("transpose", transpose, args("a"), transpose_docstring);
  def("diag", diag, args("a"), diag_docstring);
  def("elementwise_product", 
      elementwiseProduct, 
      args("a", "b"),
      elementwise_product_docstring);

}

} } /* namespace AMD::python */
