/**
 * @file ScalarMatrixFunc
 * @author adromanova
 * 
 * Python bindings for ScalarMatrixFunc and helper functions.
 */

#include <boost/python.hpp>

#include "ScalarMatrixFunc.hpp"
#include "SymbolicScalarMatlab.hpp"
#include "SymbolicScalarMatlabHelper.hpp"
#include "SymbolicMatrixMatlab.hpp"

namespace AMD { namespace python {

  using namespace boost::python;
  using namespace AMD;

/**
 * @brief A function template to export ScalarMatrixFunc and helper functions. 
 * @param[in] name      exported class name 
 * @param[in] docstirng exported class description
 */
template<class MT, class ST>
void export_scalarmatrixfunc(std::string name, std::string docstring) {

  /**
   * docstring
   */
  const char* default_constructor_docstring = 
    "Constructor for a ScalarMatrixFunc object. "\
    "Default setting is a variable.";
  
  const char* val_constructor_docstring = 
    "Constructor from function value and derivative value.\n"\
    "    Args:\n"\
    "        fVal: scalar type function variable\n"\
    "        dVal: matrix type function variable\n";

  const char* const_constructor_docstring = 
    "Constructor for constant functions\n"\
    "    Args:\n"\
    "        fVal: scalar type function variable\n"\
    "        m:    number of rows\n"\
    "        n:    number of columns\n";
  
  const char* initWithVariable_docstring = 
    "Assign parameters to data members, set co.\n"\
    "    Args:\n"\
    "        fVal: scalar type function variable to be assigned to functionVal\n"\
    "        dVal: matrix type function variable to be assigned to derivativeVal\n";

  const char* initWithConst_docstring = 
    "Assign parameter to .\n"
    "    Args:\n"\
    "        fVal: scalar type funciton variable\n"\
    "        m:    number of rows\n"\
    "        n:    number of columns\n";
  
  // export ScalarMatrixFunc
  class_<ScalarMatrixFunc<MT, ST> >(name.c_str(), docstring.c_str())
    // constructors
    .def(init<>(default_constructor_docstring))
    .def(init<ST, MT>(args("fVal", "dVal"), val_constructor_docstring)) 
    .def(init<ST, int, int>(args("fVal", "m", "n"),
                            const_constructor_docstring))
    // methods
    .def("initWithVariable", 
        &ScalarMatrixFunc<MT, ST>::initWithVariable,
        args("fVal, dVal"),
        initWithVariable_docstring)
    .def("initWithConst",
        &ScalarMatrixFunc<MT, ST>::initWithConst,
        args("fVal", "m", "n"), 
        initWithConst_docstring)
    // data members to properties
    .add_property("functionVal", 
                  &ScalarMatrixFunc<MT, ST>::functionVal,
                  "function value")
    .add_property("derivativeVal", 
                  &ScalarMatrixFunc<MT, ST>::derivativeVal, 
                  "derivative value")
    .add_property("derivativeFuncVal", 
                  &ScalarMatrixFunc<MT, ST>::derivativeFuncVal,
                  "derivative function value")
    .add_property("isConst", 
                  &ScalarMatrixFunc<MT, ST>::isConst,
                  "set to true if current ScalarMatrixFunc is constant")
    //operators
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self / self) 
    ;
}

void export_scalarmatrixfunc() {
  // export symbolic
  export_scalarmatrixfunc<SymbolicMatrixMatlab, SymbolicScalarMatlab>(
      "symSMF", 
      "Symbolic Scalar-Matrix function. This object acts as the root node\n" \
      "in the computational tree. Currently th e propgram can compute\n" \
      "derivatives for Scalar-Matrix function \"trace\" and \"logdet\".");  
}

} } /* namespace AMD::python */
