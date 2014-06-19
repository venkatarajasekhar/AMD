#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD {

template <>
struct MatrixAdaptor_t<SymbolicMatrixMatlab> {

  /** 1. get the number of rows */
  static int getNumRows (const SymbolicMatrixMatlab& A) {
    return A.getNumRows();
  }

  /** 2. get the number of cols */
  static int getNumCols (const SymbolicMatrixMatlab& A) {
    return A.getNumCols();
  }

  /** 3. add two matrices */
  static void add (const SymbolicMatrixMatlab& A,
                   const SymbolicMatrixMatlab& B,
                   SymbolicMatrixMatlab& C) {
    copy(C, A+B);
  }

  /** 4. subtract two matrices */
  static void minus (const SymbolicMatrixMatlab& A,
                     const SymbolicMatrixMatlab& B, 
                     SymbolicMatrixMatlab& C) {
    copy(C, A-B);
  }

  /** 5. multiply two matrices */
  static void multiply (const SymbolicMatrixMatlab& A,
                        const SymbolicMatrixMatlab& B,
                        SymbolicMatrixMatlab& C) {
    copy(C, A*B);
  }

  /** 6. matrix transpose */

  static void transpose (const SymbolicMatrixMatlab& A, 
                         SymbolicMatrixMatlab& B) {
    copy(B, AMD::transpose(A));
  }

  /** 7. matrix negation */
  static void negation(const SymbolicMatrixMatlab& A,
                       SymbolicMatrixMatlab& B) {
    copy(B, -A);
  }

  /** 8. matrix inverse */
  static void inv(const SymbolicMatrixMatlab& A,
                  SymbolicMatrixMatlab& B) {
    copy(B, AMD::inv(A));
  }

  /** 9. matrix trace */
  static void trace(const SymbolicMatrixMatlab& A,
                    SymbolicScalarMatlab& B) {
    B.copy(AMD::trace(A));
  }

  /** 10. matrix identity */
  // a cannot be const. Why?
  static void eye(SymbolicMatrixMatlab& A,  
                  int n,
                  SymbolicMatrixMatlab& B) {
    copy(B, A.eye(n));
  }

  /** 11. matrix zero */ 
  static void zeros(SymbolicMatrixMatlab& A, 
                    int m, 
                    int n,
                    SymbolicMatrixMatlab& B) {
    copy(B, A.zeros(m,n));
  }

  /** 12. matrix logdet */
  static void logdet(const SymbolicMatrixMatlab& A,
                     SymbolicScalarMatlab& B) {
    B.copy(AMD::logdet(A));
  }

  /** 13. matrix copy operator */
  static void copy (SymbolicMatrixMatlab &A,      /**< target obj */
                    const SymbolicMatrixMatlab &B /**< source obj */ ) {
    // TODO symbol nRows nCols are private variables
    A.copy(B);
  }

  /** 14. print out the matrix */
  static void print (const SymbolicMatrixMatlab& A, 
                     std::ostream& os=std::cout) {
    A.print(os);
  }

  /** 15. matrix diagonal */
  static void diag(const SymbolicMatrixMatlab& A,
                   SymbolicMatrixMatlab& B) {
    copy(B, AMD::diag(A));
  }

  /** 16. matrix element-wise product */
  static void elementwiseProd(const SymbolicMatrixMatlab& A,
                              const SymbolicMatrixMatlab& B,
                              SymbolicMatrixMatlab& C) {
    copy(C, AMD::elementwiseProd(A, B));
  }
};

} 

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
