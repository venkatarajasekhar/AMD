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

/**
 * A function to export SymbolicScalarMatrix and helper functions
 */
void export_symbolicscalarmatlab () {
 
  /**
   * docstring
   */
  const char* SymScal_docstring = 
    "This class symbolically represents a scalar using " \
    "an internal string representation."; 

  const char* str_constructor_docstring = 
    "Constructor from string value.\n" \
    "Args:\n" \
    "    symbol: string representation of symbolic scalar.";

  const char* double_constructor_docstring = 
    "Constructor from double value.\n" \
    "Agrs:\n" \
    "    val: initializer for a symbol (to be converted to string)";

  const char* sqrt_docstring = 
    "Compute (sqrt(a). " \
    "If a is trace(X) then the return value will hold sqrt(trace(X)).\n" \
    "Args:\n" \
    "    a: symbolic argument passed to sqrt.\n" \
    "Returns:\n" \
    "    SymScal representaion of sqrt(a)";

  using namespace boost::python;
  using namespace AMD;
  
  // export SymbolicScalarMatlab
  class_<SymbolicScalarMatlab>("SymScal", SymScal_docstring, init<>())
  	// constructors
    .def(init<>("Default constructor. Sets symbol name to 'x'"))
    .def(init<std::string>(args("symbol"), str_constructor_docstring))
    .def(init<double>(args("val"), double_constructor_docstring))
    // export member variables as properties
    .add_property("string", 
                  &SymbolicScalarMatlab::getString, 
                  "internal string representaion")
    // export overloaded operators
    .def(self + self)   
    .def(self - self) 
    .def(-self)
    .def(self * self)  
    .def(self / self)
    .def("__str__", &SymbolicScalarMatlab::getString)
  ;

  // export helper functions
  def("sqrt", sqrt, args("a"), sqrt_docstring);

}

} } /* namespace AMD::python */
