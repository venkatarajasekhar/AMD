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
  /** 6. matrix transpose */
  static SymbolicMatrixMatlab transpose(const SymbolicMatrixMatlab& a) {
    return (AMD::transpose(a));
  }

  /** 7. matrix negation */
  static SymbolicMatrixMatlab negation(const SymbolicMatrixMatlab& a) {
    return (-a);
  }

  /** 8. matrix inverse */
  static SymbolicMatrixMatlab inv(const SymbolicMatrixMatlab& a) {
    return (AMD::inv(a));
  }

  /** 9. matrix trace */
  static SymbolicScalarMatlab trace(const SymbolicMatrixMatlab& a) {
    return (AMD::trace(a));
  }

  /** 10. matrix identity */
  // a cannot be const. Why?
  static SymbolicMatrixMatlab eye(SymbolicMatrixMatlab& a,  int n) {
    return (a.eye(n));
  }

  /** 11. matrix zero */ 
  static SymbolicMatrixMatlab zeros(SymbolicMatrixMatlab& a, 
                                           int m, 
                                           int n) {
    return (a.zeros(m,n));
  }

  /** 12. matrix logdet */
  static SymbolicScalarMatlab logdet(const SymbolicMatrixMatlab& a) {
    return (AMD::logdet(a));
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
