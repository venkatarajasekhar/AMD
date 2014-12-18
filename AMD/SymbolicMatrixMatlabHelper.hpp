#ifndef AMD_SYMBOLIC_MATRIX_MATLAB_HELPER_HPP
#define AMD_SYMBOLIC_MATRIX_MATLAB_HELPER_HPP

/**
 * @file SymbolicMatrixMatlabHelper.hpp
 *
 * @author Peder Olsen, Anju Kambadur
 *
 * @brief This file defines methods for SymbolicMatrixMatlab class.
 */

#include <string>
#include <sstream>
#include "SymbolicMatrixMatlab.hpp"
#include "Exception.hpp"

namespace AMD {

  /**
   * Compute the scalar trace(a) from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation trace(a)
   */
  static SymbolicScalarMatlab trace(const SymbolicMatrixMatlab& a)  {
    SymbolicScalarMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != a.getNumCols())
      throw exception_generic_impl("AMD::trace", 
                                   "Matrix is not square", 
                                   AMD_NON_SQUARE);

    retVal = SymbolicScalarMatlab
                ("trace(" + detail::removeParenthesis(a.symbol) + ")");
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, trace)

    return retVal;
  }

  /**
   * Compute the scalar logdet(a) from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation logdet(a)
   */
  static SymbolicScalarMatlab logdet(const SymbolicMatrixMatlab& a)  {
    SymbolicScalarMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != a.getNumCols())
      throw exception_generic_impl("AMD::logdet", 
        "Matrix is not square", AMD_NON_SQUARE);

    retVal =  SymbolicScalarMatlab
                ("log(det(" + detail::removeParenthesis(a.symbol) + "))");
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, logdet)

    return retVal;
  }


  /**
   * Compute the scalar frobenius norm from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation of ||a||_F.
   */
  static SymbolicScalarMatlab fnorm(const SymbolicMatrixMatlab& a)  {
    return SymbolicScalarMatlab
              ("norm(" + detail::removeParenthesis(a.symbol) + ",'fro')");
  }

  /**
   * Compute the inverse matrix of the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of inv(a).
   */
  static SymbolicMatrixMatlab inv(const SymbolicMatrixMatlab& a) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != a.getNumCols())
      throw exception_generic_impl("AMD::inv", 
        "Matrix is not square", AMD_NON_SQUARE);

    retVal =  SymbolicMatrixMatlab("inv(" +
                                   detail::removeParenthesis(a.symbol) + ")",
                                   a.getNumRows(),
                                   a.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, inv)

    return retVal;
  }

  /**
   * Compute the transposed matrix of the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a^T.
   */
  static SymbolicMatrixMatlab transpose(const SymbolicMatrixMatlab& a) {
    return SymbolicMatrixMatlab(a.symbol + "'",
      a.getNumCols(),
      a.getNumRows());
  }

  /**
   * Compute the diagnol matrix of matrix a.
   *
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of diag(a).
   */

  static SymbolicMatrixMatlab diag(const SymbolicMatrixMatlab &a) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != a.getNumCols())
      throw exception_generic_impl("AMD::diag", 
        "Matrix is not square", AMD_NON_SQUARE);

    retVal =  SymbolicMatrixMatlab("diag(" +
                                   detail::removeParenthesis(a.symbol) + ")",
                                   a.getNumRows(),
                                   a.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, diag)

    return retVal;
  }

  // bivariate functions that return a matrix
  /**
   * Compute the elementwise product between a and b.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a .* b.
   */
  static SymbolicMatrixMatlab elementwiseProduct(const SymbolicMatrixMatlab& a,
                                          const SymbolicMatrixMatlab& b) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != b.getNumRows() || 
      a.getNumCols() != b.getNumCols())
      throw exception_generic_impl("AMD::elementwiseProduct", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);

    retVal =  SymbolicMatrixMatlab("(" + a.symbol + ".*" + b.symbol + ")",
                                   a.getNumRows(),
                                   a.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, elementwiseProduct)

    return retVal;
  }

  /**
   * Compute a+b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a + b.
   */
  static SymbolicMatrixMatlab operator+(const SymbolicMatrixMatlab& a,
    const SymbolicMatrixMatlab& b) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != b.getNumRows() || 
      a.getNumCols() != b.getNumCols())
      throw exception_generic_impl("AMD::operator+", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);

    retVal =  SymbolicMatrixMatlab("(" + a.symbol + "+" + b.symbol + ")",
                                   a.getNumRows(),
                                   a.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator+)

    return retVal;
  }

  /**
   * Compute a-b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a - b.
   */
  static SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a,
    const SymbolicMatrixMatlab& b) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumRows() != b.getNumRows() || 
      a.getNumCols() != b.getNumCols())
      throw exception_generic_impl("AMD::operator-", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);
    retVal = SymbolicMatrixMatlab("(" + a.symbol + "-" + b.symbol + ")",
                                  a.getNumRows(),
                                  a.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator-)

    return retVal;
  }

  /**
   * Unary minus: Compute the negation -a, where a is a matrix.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of -a.
   */
  static SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a) {
    return SymbolicMatrixMatlab("(-" + a.symbol + ")",
                                a.getNumRows(),
                                a.getNumCols());
  }


  /**
   * Compute a*b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a * b.
   */
  static SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
    const SymbolicMatrixMatlab& b) {
    SymbolicMatrixMatlab retVal;

    AMD_START_TRY_BLOCK()
    if (a.getNumCols() != b.getNumRows())
      throw exception_generic_impl("AMD::operator*", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);
    retVal = SymbolicMatrixMatlab("(" + a.symbol + "*" + b.symbol + ")",
                                  a.getNumRows(), 
                                  b.getNumCols());
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator*)

    return retVal;
  }

  // Element-wise operations: 
  //   bivariate functions where one of the arguments is a scalar
  //   We define s*M, M*s and M/s.  s+M and s-M are not defined.
  //   We put s*M and M*s both in the canonical form s*M.
  /**
   * Compute a.*b, where a is a scalar and b is a matrix.
   * @param[in] a A SymbolicScalarMatlab.
   * @param[in] b A SymbolicMatrixMatlab.
   * @return The SymbolicMatrixMatlab representation of a .* b.
   */
  static SymbolicMatrixMatlab operator*(const SymbolicScalarMatlab& a,
    const SymbolicMatrixMatlab& b) {
    return SymbolicMatrixMatlab("(" + a.symbol + ".*" + b.symbol + ")",
      b.getNumRows(), b.getNumCols());
  }

  /**
   * Compute a.*b, where a is a matrix and b is a scalar.
   * @param[in] a A SymbolicMatrixMatlab.
   * @param[in] b A SymbolicScalarMatlab.
   * @return The SymbolicMatrixMatlab representation of a .* b.
   */
  static SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
    const SymbolicScalarMatlab& b) {
    return SymbolicMatrixMatlab("(" + b.symbol + ".*" + a.symbol + ")",
      a.getNumRows(), a.getNumCols());
  }

  /**
   * Note: scalar/Matrix should not be defined.  User will have to write
   * scalar * inv(Matrix)
   */

  /**
   * Compute a./b, where a is a matrix and b is a scalar.
   * @param[in] a A SymbolicMatrixMatlab.
   * @param[in] b A SymbolicScalarMatlab.
   * @return The SymbolicMatrixMatlab representation of a ./ b.
   */
  static SymbolicMatrixMatlab operator/(const SymbolicMatrixMatlab& a,
    const SymbolicScalarMatlab& b) {
    return SymbolicMatrixMatlab("(" + a.symbol + "./" + b.symbol + ")",
      a.getNumRows(), a.getNumCols());
  }

} /** namespace AMD */

#endif /** AMD_SYMBOLIC_MATRIX_MATLAB_HELPER_HPP */
