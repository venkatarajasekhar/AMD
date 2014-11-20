/**
 * @file Exception.cpp
 * 
 * @author adromanova
 *
 * @brief This file contains a function to tranlate AMD exception
 */

#include "Python.h"
#include "Exception.hpp"
#include <boost/python.hpp>

namespace AMD { namespace python {
  
  using namespace AMD;
  using namespace boost::python;

  static PyObject* AMDExceptionType = NULL;

  void translate_exception_generic_impl(exception_generic_impl const& e) {
        PyErr_SetObject(AMDExceptionType, object(e).ptr());
  }

  void export_exception() {
    class_<exception_generic_impl> AMDExceptionClass("AMDException",
        init<const char*, const char*, const error_code_type&>());
    AMDExceptionClass.add_property("trace", &exception_generic_impl::trace)
      .add_property("what", &exception_generic_impl::what)
      .add_property("errorCode", &exception_generic_impl::code);

    AMDExceptionType = AMDExceptionClass.ptr();
    
    register_exception_translator<exception_generic_impl>(
        &translate_exception_generic_impl);     
  }

} } /* namespace AMD::python */

