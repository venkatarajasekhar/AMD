#ifndef MATRIX_ADAPTOR_HPP
#define MATRIX_ADAPTOR_HPP

//#include "../config.h"

namespace AMD {

template <typename MatrixType>
struct MatrixAdaptor_t {
  /** empty declaration --- fill in functions in the specialization */

  /** 1. get the number of rows */
  
  /** 2. get the number of cols */

  /** 3. add two matrices */

  /** 4. subtract two matrices */

  /** 5. multiply two matrices */

  /** 6. divide two matrices */

  /** 7. print out the matrix */

  /** There are more functions to add --- trace, inv, logdet, etc */
};

}

#include "SymbolicMatrixAdaptor.hpp"

#if AMD_HAVE_ELEMENTAL
// Include this when it actually works
//  #include "ElementalMatrixAdaptor.hpp"
#endif

#endif /** MATRIX_ADAPTOR_HPP */
