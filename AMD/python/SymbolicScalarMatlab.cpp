/**
 * @file SymbolicScalarMatlab.cpp
 * @author pkambadu
 * 
 * Python bindings for SymbolicScalarMatlab object and helper functions.
 */

#include <boost/python.hpp>

#include "SymbolicScalarMatlab.hpp"

namespace AMD { namespace python {

/* The two macro overloads following are necessary to expand the depicted *
 * methods with all the possibilities for their default arguments, once   *
 * python can't see that by default */
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(print_overloads, print, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(println_overloads, println, 0, 1)

/**
 * A function to export all the definitions for context object.
 */
void export_symbolicscalarmatlab () {

  using namespace boost::python;
  using namespace AMD;

  class_<SymbolicScalarMatlab>("symScalMat", init<>())
  	// constructors
    .def(init<std::string>())
    .def(init<double>())
  	/* Attention to the capital 'P' in Print, such as *
  	 * to avoid conflict with python built-in 'print' */
  	.def("Print", &SymbolicScalarMatlab::print, print_overloads())
  	.def("println", &SymbolicScalarMatlab::println, println_overloads())
//    .def("copy", &SymbolicScalarMatlab::copy)   //TODO
    // member variables converted to properties
    .add_property("string", &SymbolicScalarMatlab::getString)
    // overloaded operators
    .def(self + self)   
    .def(self - self)  
    .def(self * self)  
    .def(self / self) 
  ;

  //helper functions
  def("sqrt", sqrt);

}

} } /* namespace AMD::python */
