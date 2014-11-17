/**
 * @file module.cpp
 * @author pkambadu
 *
 * This file contains all the sources that are used for the python bindings.
 */

#include <boost/python.hpp>
#include "Exception.hpp"

namespace AMD { namespace python {

extern void export_symbolicscalarmatlab();

extern void export_symbolicmatrixmatlab();

extern void export_scalarmatrixfunc();

extern void export_matrixmatrixfunc();

//extern void translate_exception_generic_impl(exception_generic_impl const& e);

extern const char* module_docstring;

BOOST_PYTHON_MODULE(AMD)
{

  using namespace boost::python;

  scope().attr("__doc__") = module_docstring;
  scope().attr("__author__") = "Prabhanjan Kambadur <pkambadu@us.ibm.com>";
  scope().attr("__date__") = "xx.xx.xxxx";
  scope().attr("__version__") = "1";
  scope().attr("__copyright__") = "Copyright (C) 2014 Bloomberg";
  scope().attr("__license__") = "http://www.eclipse.org/legal/epl-v10.html";

  export_symbolicscalarmatlab();
  export_symbolicmatrixmatlab();
  export_scalarmatrixfunc();
  export_matrixmatrixfunc();
 /* register_exception_translator<AMD::exception_generic_impl>(
      &translate_exception_generic_impl);
*/
}

} } /* namespace AMD::python */
