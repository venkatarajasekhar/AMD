#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"

namespace AMD {

template <>
struct MatrixAdaptor_t<SymbolicMatrixMatlab> {

  /** 1. get the number of rows */
  static int getNumRows (const SymbolicMatrixMatlab& matrix) {
    return matrix.getNumRows();
  }

  /** 2. get the number of cols */
  static int getNumCols (const SymbolicMatrixMatlab& matrix) {
    return matrix.getNumCols();
  }

  /** 3. add two matrices */
  static SymbolicMatrixMatlab add (const SymbolicMatrixMatlab& first,
                                   const SymbolicMatrixMatlab& second) {
    return (first+second);
  }

  /** 4. subtract two matrices */
  static SymbolicMatrixMatlab minus (const SymbolicMatrixMatlab& first,
                                   const SymbolicMatrixMatlab& second) {
    return (first-second);
  }

  /** 5. multiply two matrices */
  static SymbolicMatrixMatlab multiply (const SymbolicMatrixMatlab& first,
                                        const SymbolicMatrixMatlab& second) {
    return (first*second);
  }

#if 0
  /** Need to fix this */
  /** 6. divide two matrices */
  static SymbolicMatrixMatlab divide (const SymbolicMatrixMatlab& first,
                                      const SymbolicMatrixMatlab& second) {
    return (first/second);
  }
#endif

  /** 7. print out the matrix */
  static void print (const SymbolicMatrixMatlab& matrix, FILE* fp) {
    matrix.print(fp);
  }
};

} 

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
