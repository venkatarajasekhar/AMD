#ifndef SYMBOLIC_MATRIX_ADAPATOR_HPP
#define SYMBOLIC_MATRIX_ADAPATOR_HPP

#include <ostream>
#include "MatrixAdaptor.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"

namespace AMD {

template <>
struct MatrixAdaptor_t<SymbolicMatrixMatlab> {

  /**< define the type of the scalar values */
  typedef SymbolicScalarMatlab value_type;

  /**< define the type of the matrix */
  typedef SymbolicMatrixMatlab matrix_type;

  /** 1. get the number of rows */
  static int getNumRows (const matrix_type& matrix) {
    return matrix.getNumRows();
  }

  /** 2. get the number of cols */
  static int getNumCols (const matrix_type& matrix) {
    return matrix.getNumCols();
  }

  /** 3. add two matrices */
  static void add (const matrix_type& first,
                   const matrix_type& second,
                   matrix_type& dest) {
    copy(dest, first+second);
  }

  /** 4. subtract two matrices */
  static void minus (const matrix_type& first,
                     const matrix_type& second, 
                     matrix_type& dest) {
    copy(dest, first-second);
  }

  /** 5. multiply two matrices */
  static void multiply (const matrix_type& first,
                        const matrix_type& second,
                        matrix_type& dest) {
    copy(dest, first*second);
  }

  /** 6. matrix transpose */

  static void transpose (const matrix_type& a, 
                         matrix_type& dest) {
    copy(dest, AMD::transpose(a));
  }

  /** 7. matrix negation */
  static void negation(const matrix_type& a,
                       matrix_type& dest) {
    copy(dest, -a);
  }

  /** 8. matrix inverse */
  static void inv(const matrix_type& a,
                  matrix_type& dest) {
    copy(dest, AMD::inv(a));
  }

  /** 9. matrix trace */
  static void trace(const matrix_type& a,
                    value_type& dest) {
    dest.copy(AMD::trace(a));
  }

  /** 10. matrix identity */
  // a cannot be const. Why?
  static matrix_type eye(int n) { 
    return matrix_type::eye(n);
  }

  /** 11. matrix zero */ 
  static matrix_type zeros(int m, int n) {
    return matrix_type::zeros(m, n);
  }

  /** 12. matrix logdet */
  static value_type logdet(const matrix_type& a) { 
    return AMD::logdet(a);
  }

  /** 13. matrix copy operator */
  static void copy (matrix_type &a,      /**< target obj */
                    const matrix_type &b /**< source obj */ ) {
    // TODO symbol nRows nCols are private variables
    a.copy(b);
  }

  /** 14. print out the matrix */
  static void print (const matrix_type& matrix, 
                     std::ostream& os=std::cout) {
    matrix.print(os);
  }

  /** 15. matrix diagonal */
  static void diag(const matrix_type& A,
                   matrix_type& B) {
    copy(B, AMD::diag(A));
  }

  /** 16. matrix element-wise product */
  static void elementwiseProd(const matrix_type& A,
                              const matrix_type& B,
                              matrix_type& C) {
    copy(C, AMD::elementwiseProd(A, B));
  }
};

} 

#endif /** SYMBOLIC_MATRIX_ADAPATOR_HPP */
