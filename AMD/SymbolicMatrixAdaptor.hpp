#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

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
  static void add (const SymbolicMatrixMatlab& first,
                   const SymbolicMatrixMatlab& second,
                   SymbolicMatrixMatlab& dest
                   ) {
    copy(dest, first+second);
  }

  /** 4. subtract two matrices */
  static void minus (const SymbolicMatrixMatlab& first,
                     const SymbolicMatrixMatlab& second, 
                     SymbolicMatrixMatlab& dest
                     ) {
    copy(dest, first-second);
  }

  /** 5. multiply two matrices */
  static void multiply (const SymbolicMatrixMatlab& first,
                        const SymbolicMatrixMatlab& second,
                        SymbolicMatrixMatlab& dest
                        ) {
    copy(dest, first*second);
  }

  /** 6. matrix transpose */

  static void transpose (const SymbolicMatrixMatlab& a, 
                         SymbolicMatrixMatlab& dest
                         ) {
    copy(dest, a);
  }

  /** 7. matrix negation */
  static void negation(const SymbolicMatrixMatlab& a,
                       SymbolicMatrixMatlab& dest
                       ) {
    copy(dest, -a);
  }

  /** 8. matrix inverse */
  static void inv(const SymbolicMatrixMatlab& a,
                  SymbolicMatrixMatlab& dest
                  ) {
    copy(dest, AMD::inv(a));
  }

  /** 9. matrix trace */
  static void trace(const SymbolicMatrixMatlab& a,
                    SymbolicScalarMatlab& dest
                    ) {
    dest.copy(AMD::trace(a));
  }

  /** 10. matrix identity */
  // a cannot be const. Why?
  static void eye(SymbolicMatrixMatlab& a,  
                  int n,
                  SymbolicMatrixMatlab& dest
                  ) {
    copy(dest, a.eye(n));
  }

  /** 11. matrix zero */ 
  static void zeros(SymbolicMatrixMatlab& a, 
                    int m, 
                    int n,
                    SymbolicMatrixMatlab& dest
                    ) {
    copy(dest, a.zeros(m,n));
  }

  /** 12. matrix logdet */
  static void logdet(const SymbolicMatrixMatlab& a,
                     SymbolicScalarMatlab& dest
                     ) {
    dest.copy(AMD::logdet(a));
  }

  /** 13. matrix copy operator */
  static void copy (SymbolicMatrixMatlab &a,      /**< target obj */
                    const SymbolicMatrixMatlab &b /**< source obj */ ) {
    // TODO symbol nRows nCols are private variables
    a.copy(b);
  }

  /** 14. print out the matrix */
  static void print (const SymbolicMatrixMatlab& matrix, FILE* fp) {
    matrix.print(fp);
  }
};

} 

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
