/**
 * @file context.cpp
 * @author pkambadu
 * 
 * Python bindings for the context object.
 */

#include <boost/python.hpp>
#include <SymbolicScalarMatlab.hpp>

namespace AMD { namespace python {

/**
 * A function to export all the definitions for context object.
 */
void export_symbolicscalarmatlab () {

  using namespace boost::python;
  using namespace AMD;

  class_<SymbolicScalarMatlab>("symScalMat", init<double>())
    .def("print", &SymbolicScalarMatlab::print)
  ;

}

} } /* namespace AMD::python */
