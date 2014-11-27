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

  static PyObject* AMDExceptionTypeObj = NULL; 

  void translate_exception_generic_impl(exception_generic_impl const& e) {
    PyErr_SetString(AMDExceptionTypeObj, e.what()); 
    PyErr_SetObject(AMDExceptionTypeObj, object(e).ptr());
  }

  void export_exception() {
    
    class_<exception_generic_impl>("AMDException",
      init<const char*, const char*, optional<const error_code_type&> >())
    .add_property("trace", &exception_generic_impl::trace)
    .add_property("what", &exception_generic_impl::what)
    .add_property("error_code", &exception_generic_impl::code)
    .def("__str__", &exception_generic_impl::what)
    ;
    
    AMDExceptionTypeObj = PyErr_NewException("AMD.AMDExceptionType", NULL, NULL);
    scope().attr("AMDExceptionType") = handle<>(borrowed(AMDExceptionTypeObj));
    register_exception_translator<exception_generic_impl>(
        &translate_exception_generic_impl);
  }

} } /* namespace AMD::python */

