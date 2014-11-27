/**
 * @file MatrixMatrixFunc.cpp
 * @author adromanova
 * 
 * Python bindings for MatrixMatrixFunc and helper methods.
 */

#include <boost/python.hpp>
#include "SymbolicScalarMatlab.hpp"
#include "SymbolicScalarMatlabHelper.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "ScalarMatrixFunc.hpp"
#include "MatrixMatrixFunc.hpp"
#include "MatrixMatrixFuncHelper.hpp"

namespace AMD { namespace python {

/**
 * @brief This function is a wrapper around MatrixMatrixFunc::print() method.
 * It prints computational tree to std::stringstream and returns the resulting 
 * std::string.
 * @param[in] MatrixMatrixFunc 
 * @return std::string containing computational tree 
 */
template <class MT, class ST> 
static std::string getMMFString(const MatrixMatrixFunc<MT, ST>& mmf) {
  std::stringstream sStream;
  mmf.print(sStream);
  return sStream.str();
}

/**
 * @brief A funciton template to export MatrixMatrixFunc an dhelper functions.
 * @param[in] name      exported class name
 * @param[in] docstring exported class docstring
 */
template <class MT, class ST>
void export_matrixmatrixfunc (std::string name, std::string docstring) {

  /**
   * docstring 
   */
  const char* default_constructor_docstring =
    "Initialize all values to defaults."; 
  
  const char* set_variable_type_docstring = 
    "Set the value of member variables of leaf nodes.\n" \
    "If the matrix of leaf node is constant matrix ...\n" \
    "Args:\n" \
    "    is_variable: Boolean value indicating whether the implicit matrix\n" \
    "                is a variable. If so, initialize the object as \n" \
    "                a variable. Otherwise initialize it as a constant.\n";

  const char* reset_docstring = "Reset the entire computational tree."; 

  const char* shallow_copy_docstring = 
    "Copy other to current MatrixMatrixfunc. This function creates\n" \
    "a shallow copy. It does not copy children nodes.\n"\
    "Agrs:\n" \
    "    other: the MMFunc to be copied from";

  const char* deep_copy_docstring = 
    "Copy other to MatrixMatrixFunc node. This function creates a deep copy.\n" \
    "It also recursively copies the current node's children nodes\n" \
    "Agrs:\n" \
    "    other: the MMFunc to be copied from.";

  const char* gradient_vec_docstring = 
    "Traverse the computational tree in post-order. Replase the matrix\n" \
    "as moving towards the leaf nodes(Reverse Mode) by applying\n" \
    "specific callback function to each node.\n" \
    "Args:\n" \
    "    initial: initial matrix\n" \
    "    result:  result matrix\n"; //TODO

  const char* inv_docstring = 
    "inv operator. Create a new node inverse in computational tree\n" \
    "Args:\n" \
    "    lhs: MatrixMatrixFunc";

  const char* trace_docstring = 
    "Create the root node for a Scalar-MAtrix function trace.\n" \
    "Once the node is created, the calculation of derivatives through\n" \
    "the computational tree (gradientVec) will  be triggered.\n" \
    "Args:\n" \
    "    lhs: MatrixMatrixFunc";
  
  const char* logdet_docstring = 
    "Create the root node for Scalar-Matrix function logdet.\n" \
    "Once this node is created, the calculation of derivative along\n" \
    "the computational tree is triggered.\n" \
    "Args:\n"\
    "    lhs: MatrixMatrixFunc";
  
  const char* elementwise_product_docstring = ""; //TODO

  const char* transpose_docstring = 
    "Create a new node with operator transpose in computational tree.";

  const char* diag_docstring = 
    "Create a new node with operator diag in the computational tree.";

  using namespace boost::python;
  using namespace AMD;

  class_<MatrixMatrixFunc<MT, ST> >(name.c_str(), docstring.c_str()) 
  //constructors
  .def(init<>(default_constructor_docstring))
  .def(init<const MT&, optional<bool> >())
  // data members to properties
  .add_property("is_const", 
                &MatrixMatrixFunc<MT, ST>::isConst, 
                "true if this is a constant function")
  .add_property("var_num_rows",
                &MatrixMatrixFunc<MT, ST>::numRows,
                "number of rows in matrix variable")
  .add_property("var_num_cols",
                &MatrixMatrixFunc<MT, ST>::numCols,
                "number of columns in matrix variable")
  .add_property("value", 
                &MatrixMatrixFunc<MT, ST>::value, 
                "matrix associated with this node")
  .add_property("left_child", 
                &MatrixMatrixFunc<MT, ST>::leftChild,
                "optional left child")
  .add_property("right_child", 
                &MatrixMatrixFunc<MT, ST>::rightChild,
                "optional right child")
  .add_property("scalar_child",
                &MatrixMatrixFunc<MT, ST>::scalarChild,
                "scalar func * matrix func") //TODO
  // methods
  .def("set_variable_type", 
       &MatrixMatrixFunc<MT, ST>::setVariableType, 
       args("is_variable"), 
       set_variable_type_docstring)
  .def("reset", &MatrixMatrixFunc<MT, ST>::reset, reset_docstring)
  .def("shallow_copy", 
       &MatrixMatrixFunc<MT, ST>::shallowCopy, 
       args("other"), 
       shallow_copy_docstring)
  .def("deep_copy", 
       &MatrixMatrixFunc<MT, ST>::deepCopy,
       args("other"),
       deep_copy_docstring)
  .def("gradient_vec",
       &MatrixMatrixFunc<MT, ST>::gradientVec, 
       args("initial", "result", "current_MMF", "result_MMF", "transpose_flag",
         "identity_initial_falg", "zero_result_flag"),
       gradient_vec_docstring)
  //operators
  .def(self + self)
  .def(-self)
  .def(self * self)
  .def(self * ScalarMatrixFunc<MT, ST>())
  .def(ScalarMatrixFunc<MT, ST>() * self)
  .def("__str__", &getMMFString<MT, ST>)
  ;

  def("inv", inv<MT, ST>, args("lhs"), inv_docstring);
  def("elementwise_product", 
      elementwiseProduct<MT, ST>, 
      args("lhs"),
      elementwise_product_docstring);
  def("transpose", transpose<MT, ST>, args("lhs"), transpose_docstring);
  def("diag", diag<MT, ST>, args("lhs"), diag_docstring);
  def("trace", trace<MT, ST>, args("lhs"), trace_docstring);
  def("logdet", logdet<MT, ST>, args("lhs"), logdet_docstring);
  
}

void export_matrixmatrixfunc() {
  export_matrixmatrixfunc<SymbolicMatrixMatlab, SymbolicScalarMatlab>(
      "SymMMF",
      "A symbolic Matrix-Matrix function.\n" \
      "This class acts as a node in computational tree. The computation\n" \
      "of matrix derivatices is performed by traversing the computational\n" \
      "tree in the reverse mode."); 
}

} } /* namespace AMD::python */
