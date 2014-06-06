#ifndef MATRIX_MATRIX_ADAPATOR_HPP
#define MATRIX_MATRIX_ADAPATOR_HPP

#include "MatrixAdaptor.hpp"
#include "elem.hpp"

namespace AMD {

template <typename T>
struct MatrixAdaptor_t<elem::Matrix<T> > {

  /** define the type of the values */
  typedef T value_type;

  /** typedef the matrix so that we can reuse it */
  typedef typename elem::Matrix<T> matrix_type;

  /** 1. get the number of rows */
  static int getNumRows (const matrix_type& A) {
    return A.Height();
  }

  /** 2. get the number of cols */
  static int getNumCols (const matrix_type& A) {
    return A.Width();
  }

  /** 3. add two matrices */
  static void add (const matrix_type& A,
                   const matrix_type& B,
                   matrix_type& C) {

    /** first copy the matrix over */
    elem::Copy (C, A);

    /** now, subtract the other matrix in */
    elem::Axpy (1.0, B, C);
  }

  /** 4. subtract two matrices */
  static void minus (const matrix_type& A,
                     const matrix_type& B, 
                     matrix_type& C) {

    /** first copy the matrix over */
    elem::Copy (C, A);

    /** now, subtract the other matrix in */
    elem::Axpy (-1.0, B, C);
  }

  /** 5. multiply two matrices */
  static void multiply (const matrix_type& A,
                        const matrix_type& B,
                        matrix_type& C) {

    /** We have to do a Gemm */
    elem::Gemm (elem::NORMAL, elem::NORMAL, 1.0, A, B, 0.0, C);
  }

  /** 6. matrix transpose */
  static void transpose (const matrix_type& A, 
                         matrix_type& B) {
    elem::Transpose (A, B);
  }

  /** 7. matrix negation */
  static void negation(const matrix_type& A,
                       matrix_type& B) {

    /** Copy over the matrix */
    elem::Copy (B, A);

    /** Multiply by -1.0 */
    elem::Scal(-1.0, B);
  }

  /** 8. matrix inverse */
  static void inv(const matrix_type& A,
                  matrix_type& B) {

    /** Assume that the matrix has an inverse */
    const int n = A.Height();

    /** First, copy over an identity as the right hand side */
    elem::Copy(B, eye(n));

    /** Solve the linear system using LU */
    elem::lu::SolveAfter(elem::NORMAL, A, B);
  }

  /** 9. matrix trace */
  /** TODO: This needs to be fixed somehow --- there is something wrong here */
  static value_type trace(const matrix_type& A) {
    value_type traceVal = 0.0;
    for (int i=0; i<a.Height(); ++i) traceVal += A.Get(i,i);

    return traceVal;
  }

  /** 10. matrix identity */
  static matrix_type eye(int n) {
    matrix_type I(n,n);
    for (i=0; i<n; ++i) I.Set(i,i,1.0);
    return I;
  }

  /** 11. matrix zero */ 
  static matrix_type zeros(int m, int n) {
    return matrix_type(m,n);
  }

  /** 12. matrix logdet */
  static value_type logdet(const matrix_type& A) {
    /** TODO */

    return value_type(0.0);
  }

  /** 13. matrix copy operator */
  static void copy (matrix_type &A,      /**< target obj */
                    const matrix_type &B /**< source obj */) {

    /** This one is pretty simple */
    elem::copy (A, B);
  }

  /** 14. print out the matrix */
  /** TODO: Elemental does not accept FILE*, it needs std::ostream */
  static void print (const matrix_type& A, FILE* fp) {
    A.print(fp);
  }
};

} 

#endif /** MATRIX_MATRIX_ADAPATOR_HPP */
