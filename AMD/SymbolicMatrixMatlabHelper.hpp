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

namespace AMD {

  /** 
   * Compute the scalar trace(a) from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation trace(a)
   */
  SymbolicScalarMatlab trace(const SymbolicMatrixMatlab& a)  {
    assert(a.getNumRows() == a.getNumCols());
    return SymbolicScalarMatlab("trace("+
				detail::removeParenthesis(a.symbol)+")");
  }

  /** 
   * Compute the scalar logdet(a) from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation logdet(a)
   */
  SymbolicScalarMatlab logdet(const SymbolicMatrixMatlab& a)  {
    assert(a.getNumRows() == a.getNumCols());
    return SymbolicScalarMatlab("log(det("+
				detail::removeParenthesis(a.symbol)+"))");
  }


  /** 
   * Compute the scalar frobenius norm from the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicScalarMatlab representation of ||a||_F.
   */
  SymbolicScalarMatlab fnorm(const SymbolicMatrixMatlab& a)  {
    return 
      SymbolicScalarMatlab("norm("+
			   detail::removeParenthesis(a.symbol)+",'fro')");
  }

  /** 
   * Compute the inverse matrix of the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of inv(a).
   */
  SymbolicMatrixMatlab inv(const SymbolicMatrixMatlab& a) {
    assert(a.getNumRows() == a.getNumCols());
    return SymbolicMatrixMatlab("inv("+
				detail::removeParenthesis(a.symbol)+")",
				a.getNumRows(), 
				a.getNumCols());
  }

  /** 
   * Compute the transposed matrix of the matrix a.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a^T.
   */
  SymbolicMatrixMatlab transpose(const SymbolicMatrixMatlab& a) {
    return SymbolicMatrixMatlab (a.symbol+"'",
				                         a.getNumCols(),
				                         a.getNumRows());
  }

  /**
   * Compute the diagnol matrix of matrix a.
   *
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of diag(a).
   */

  SymbolicMatrixMatlab diag(const SymbolicMatrixMatlab &a) {
    assert(a.getNumRows() == a.getNumCols()); // check square matrix.
    return SymbolicMatrixMatlab("diag(" +
                                detail::removeParenthesis(a.symbol)+")",
                                a.getNumRows(),
                                a.getNumCols());
  }


  // bivariate functions that return a matrix
  /** 
   * Compute the elementwise product between a and b.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a .* b.
   */
  SymbolicMatrixMatlab elementwiseProduct(const SymbolicMatrixMatlab& a, 
				       const SymbolicMatrixMatlab& b) {
    assert(a.getNumRows() == b.getNumRows() && 
	   a.getNumCols() == b.getNumCols());
    return SymbolicMatrixMatlab("("+a.symbol+".*"+b.symbol+")",
				a.getNumRows(),
				a.getNumCols());
  }

  /** 
   * Compute a+b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a + b.
   */
  SymbolicMatrixMatlab operator+(const SymbolicMatrixMatlab& a, 
				 const SymbolicMatrixMatlab& b) {
    assert(a.getNumRows() == b.getNumRows() && 
	   a.getNumCols() == b.getNumCols());
    return SymbolicMatrixMatlab("("+a.symbol+"+"+b.symbol+")",
				a.getNumRows(),
				a.getNumCols());
  }

  /** 
   * Compute a-b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a - b.
   */
  SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a, 
				 const SymbolicMatrixMatlab& b) {
    assert(a.getNumRows() == b.getNumRows() && 
	   a.getNumCols() == b.getNumCols());
    return SymbolicMatrixMatlab("("+a.symbol+"-"+b.symbol+")",
				a.getNumRows(),
				a.getNumCols());
  }

  /** 
   * Unary minus: Compute the negation -a, where a is a matrix.
   * @param[in] a A symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of -a.
   */
  SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a) {
    return SymbolicMatrixMatlab("(-"+a.symbol+")",
				a.getNumRows(),
				a.getNumCols());
  }


  /** 
   * Compute a*b, where a and b are matrices.
   * @param[in] a A symbolic matrix argument.
   * @param[in] b Another symbolic matrix argument.
   * @return The SymbolicMatrixMatlab representation of a * b.
   */
  SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a, 
				                         const SymbolicMatrixMatlab& b) {
    assert(a.getNumCols() == b.getNumRows());
    return SymbolicMatrixMatlab("("+a.symbol+"*"+b.symbol+")",
				a.getNumRows(),b.getNumCols());
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
  SymbolicMatrixMatlab operator*(const SymbolicScalarMatlab& a, 
                        				 const SymbolicMatrixMatlab& b) {
    return SymbolicMatrixMatlab("("+a.symbol+".*"+b.symbol+")",
				b.getNumRows(),b.getNumCols());
  }

  /** 
   * Compute a.*b, where a is a matrix and b is a scalar.
   * @param[in] a A SymbolicMatrixMatlab.
   * @param[in] b A SymbolicScalarMatlab.
   * @return The SymbolicMatrixMatlab representation of a .* b.
   */
  SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a, 
				                         const SymbolicScalarMatlab& b) {
    return SymbolicMatrixMatlab ("("+b.symbol+".*"+a.symbol+")",
				 a.getNumRows(),a.getNumCols());
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
  SymbolicMatrixMatlab operator/(const SymbolicMatrixMatlab& a,
				                         const SymbolicScalarMatlab& b) {
    return SymbolicMatrixMatlab ("("+a.symbol+"./"+b.symbol+")",
				 a.getNumRows(),a.getNumCols());
  }

} /** namespace AMD */

#endif /** AMD_SYMBOLIC_MATRIX_MATLAB_HELPER_HPP */
