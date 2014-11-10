/**
 * @file Exception.cpp
 * 
 * @author adromanova
 *
 * @brief This file contains a function to tranlate AMD exception
 */

#include "Python.h"
#include "Exception.hpp"

namespace AMD { namespace python {
  
  using namespace AMD;

  void translate_exception_generic_impl(exception_generic_impl const& e) {
        // TODO more specific exception translation 
        PyErr_SetString(PyExc_RuntimeError, e.what());
  }

} } /* namespace AMD::python */

