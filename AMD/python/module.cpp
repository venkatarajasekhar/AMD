/**
 * @file module.cpp
 * @author pkambadu
 *
 * This file contains all the sources that are used for the python bindings.
 */

#include <boost/python.hpp>

namespace AMD { namespace python {

extern void export_symbolicscalarmatlab();

extern void export_symbolicmatrixmatlab();

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

  AMD::python::export_symbolicscalarmatlab();
  AMD::python::export_symbolicmatrixmatlab();

}

} } /* namespace AMD::python */
