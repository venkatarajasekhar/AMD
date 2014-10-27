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

  class_<SymbolicMatrixMatlab>("symMatrMat", init<>())
    .def(init<int, int>())
  	.def(init<std::string>())
    .def(init<std::string, int, int>())
    .def(init<const SymbolicMatrixMatlab &>())
  	/* Attention to the capital 'P' in Print, such as *
  	 * to avoid conflict with python built-in 'print' */
  	.def("Print", &SymbolicMatrixMatlab::print, print_overloads())
  	.def("println", &SymbolicMatrixMatlab::println, println_overloads())
    // static class functions
    .def("eye", &SymbolicMatrixMatlab::eye)
    .staticmethod("eye")
    .def("zeros", &SymbolicMatrixMatlab::zeros)
    .staticmethod("zeros")
    // member variables converted to properties
    .add_property("string", &SymbolicMatrixMatlab::getString)
    .add_property("numRows", &SymbolicMatrixMatlab::getNumRows)
    .add_property("numCols", &SymbolicMatrixMatlab::getNumCols)
    // overloaded operators
    .def(self + self) 
    .def(self - self) 
    .def(self * self)  
    .def(SymbolicScalarMatlab() * self)
    .def(self * SymbolicScalarMatlab()) 
  ;

  // helper functions
  def("trace", trace);
  def("logdet", logdet);
  def("fnorm", fnorm);
  def("inv", inv);
  def("transpose", transpose);
  def("diag", diag);
  def("elementWiseProduct", elementwiseProduct);

}

} } /* namespace AMD::python */
