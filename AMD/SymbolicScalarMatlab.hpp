#ifndef AMD_SymbolicScalarMatlab_HPP
#define AMD_SymbolicScalarMatlab_HPP

/**
 * @file SymbolicScalarMatlab.hpp
 *
 * @author Peder Olsen, Anju Kambadur
 *
 * @brief This file defines a class to symbolically represent a scalar.  The
 * symbolic scalar is constructed from a symbolic matrix using an expression
 * such as trace(X), where X is a symbolic matrix.
 */

#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>

#include "utility.hpp"

namespace AMD {

struct SymbolicMatrixMatlab;

/**
 * @brief This class represents a symbolic scalar using an 
 * internal string represenation.  The string can be a variable
 * name such as x, myValue or a derived value such as logdet(X),
 * where X is a symbolic matrix.
 */ 
struct SymbolicScalarMatlab {
  /** Default constructor giving x as the symbol name */
  SymbolicScalarMatlab() : symbol("x") {}

  /** 
   * Constructor from string value.  
   * @param[in] symbol String representation of Symbolic Scalar.
   */
  SymbolicScalarMatlab(std::string symbol) : symbol(symbol) {}

  /** 
   * Constructor from a double value.
   * @param[in] val Initializer for a symbol (to be converted to string)
   */
  SymbolicScalarMatlab(double val) {
	  std::ostringstream ss;
	  ss << val;
	  symbol = ss.str();
  }

  /** 
   * Print internal string representation to a file.
   * @param[out] os Outputstream to print to.
   */
  void print(std::ostream& os=std::cout) const {
    os << detail::removeParenthesis(symbol);
  }

  /** 
   * Print internal string representation to a file.
   * @param[out] os Outputstream to print to.
   */
  void println(std::ostream& os=std::cout) const {
    os << detail::removeParenthesis(symbol) << std::endl;
  }

  /** 
   * Copy content of one SymbolicScalarMatlab to another.
   * @param[in] rhs The variable that we are making a copy of.
   */
  SymbolicScalarMatlab& operator=(const SymbolicScalarMatlab& rhs) {
    symbol = rhs.symbol;

    return(*this);
  }

  /**
   * Copy content of one SymbolicScalarMatlab to another.
   * @param[in] rhs The variable that we are making a copy of.
   */
  SymbolicScalarMatlab& copy(const SymbolicScalarMatlab& rhs) {
    symbol = rhs.symbol;
    return(*this);
  }


  /** 
   * Return the string representation of the class.
   * @return The value of the internal string representation.
   */
  std::string getString() const { return symbol; }
  void reset(std::string str) {
    symbol = str;
  }

  friend SymbolicScalarMatlab sqrt(const SymbolicScalarMatlab& a);
  friend SymbolicScalarMatlab operator+(const SymbolicScalarMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  friend SymbolicScalarMatlab operator-(const SymbolicScalarMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  friend SymbolicScalarMatlab operator-(const SymbolicScalarMatlab& a);
  friend SymbolicScalarMatlab operator*(const SymbolicScalarMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  friend SymbolicScalarMatlab operator/(const SymbolicScalarMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  friend SymbolicMatrixMatlab operator*(const SymbolicScalarMatlab& a, 
                                        const SymbolicMatrixMatlab& b);
  friend SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  friend SymbolicMatrixMatlab operator/(const SymbolicMatrixMatlab& a, 
                                        const SymbolicScalarMatlab& b);
  private:
  std::string symbol; /**< This is the internal string variable */
};

} /** namespace AMD */

#endif /** AMD_SymbolicScalarMatlab_HPP */
