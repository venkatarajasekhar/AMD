/**
 * @file ScalarMatrixFunc
 * @author adromanova
 * 
 * Python bindings for ScalarMatrixFunc and helper functions.
 */

#include <boost/python.hpp>

#include "ScalarMatrixFunc.hpp"

namespace AMD { namespace python {

  using namespace boost::python;
  using namespace AMD;

/**
 * @brief A function to export ScalarMatrixFunc and helper functions. 
 * @param[in] name      class name 
 * @param[in] docstirng class description
 */
template<class MT, class ST>
void export_scalarmatrixfunc (std::string name, std::string docstring) {
 
  using namespace boost::python;
  using namespace AMD;

  /**
   * docstring
   */
  const char* default_constructor_docstring = 
    "Constructor for a ScalarMatrixFunc object. "\
    "Default setting is a variable.";
  
  const char* val_constructor_docstring = 
    "Constructor from function value and derivative value.\n"\
    "    Args:\n"\
    "        arg1: scalar type function variable\n"\
    "        arg2: matrix type function variable\n";

  const char* MMF_val_constructor_docstring = ""; //TODO 

  const char* const_constructor_docstring = 
    "Constructor for constant functions\n"\
    "    Args:\n"\
    "        arg1: scalar type function variable\n"\
    "        arg2: number of rows\n"\
    "        arg3: number of columns\n";
  
  const char* initWithVariable_docstring = 
    "Assign parameters to data members, set co.\n"\
    "    Args:\n"\
    "        arg1: scalar type function variable to be assigned to functionVal\n"\
    "        arg2: matrix type function variable to be assigned to derivativeVal\n";

  const char* initWithConst_docstring = 
    "Assign parameter to .\n"
    "    Args:\n"\
    "        arg1: scalar type funciton variable\n"\
    "        arg2: number of rows\n"\
    "        arg3: number of columns\n";
  
  // export ScalarMatrixFunc
  class_<ScalarMatrixFunc<MT, ST> >(name.c_str(), docstring.c_str())
    // constructors
    .def(init<>(), default_constructor_docstring)
    .def(init<ST, MT>(), val_constructor_docstring) 
    .def(init<ST, MT, MatrixMatrixFunc<MT, ST> >(), 
         MMF_val_constructor_docstring)
    .def(init<ST, int, int>(), const_constructor_docstring)
    // methods
    .def("initWithVariable", 
        &ScalarMatrixFunc<MT, ST>::initWithAVariable, 
        initWithVariable_docstring)
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
  // TODO call export_scalarmatrfunc to export 
}

} } /* namespace AMD::python */
