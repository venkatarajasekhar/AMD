/**
 * @file SymbolicScalarMatlab.cpp
 * @author pkambadu, adromanova
 * 
 * Python bindings for SymbolicScalarMatlab and helper functions.
 */

#include <boost/python.hpp>

#include "SymbolicScalarMatlab.hpp"
#include "SymbolicScalarMatlabHelper.hpp"

namespace AMD { namespace python {

/* The two macro overloads following are necessary to expand the depicted *
 * methods with all the possibilities for their default arguments, once   *
 * python can't see that by default */
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(print_overloads, print, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(println_overloads, println, 0, 1)

/**
 * A function to export SymbolicScalarMatrix and helper functions
 */
void export_symbolicscalarmatlab () {
 
  /**
   * docstring
   */
  const char* symScalMat_docstring = 
    "This class symbolically represents a scalar using "\
    "an internal string representation."; 

  const char* sqrt_docstring = 
    "Compute (sqrt(a). " \
    "If a is trace(X) then the return value will hold sqrt(trace(X)).\n" \
    "Args:\n" \
    "    a: symbolic argument passed to sqrt.\n" \
    "Returns:\n" \
    "    SymbolicScalarMatlab representaion of sqrt(a)";

  using namespace boost::python;
  using namespace AMD;
  
  // export SymbolicScalarMatlab
  class_<SymbolicScalarMatlab>("symScalMat", symScalMat_docstring, init<>())
  	// constructors
    .def(init<std::string>())
    .def(init<double>())
  	/* Attention to the capital 'P' in Print, such as *
  	 * to avoid conflict with python built-in 'print' */
  	.def("Print", &SymbolicScalarMatlab::print, print_overloads())
  	.def("println", &SymbolicScalarMatlab::println, println_overloads())
    // export member variables as properties
    .add_property("string", 
                  &SymbolicScalarMatlab::getString, 
                  "internal string representaion")
    // export overloaded operators
    .def(self + self)   
    .def(self - self)  
    .def(self * self)  
    .def(self / self) 
  ;

  // export helper functions
  def("sqrt", sqrt, sqrt_docstring);

}

} } /* namespace AMD::python */
