#ifndef AMD_SymbolicMatrixMatlab_HPP
#define AMD_SymbolicMatrixMatlab_HPP

/**
 * @file SymbolicMatrixMatlab
 *
 * @author Peder Olsen and Anju Kambadur
 *
 * This file defines a class that act as a matrix and computes
 * symbolic matrix-matrix expressions and symbolic scalar-matrix
 * expressions.  The symbolic expressions are given using standard
 * MATLAB notation.
 */

#include <string>
#include <iostream>
#include <sstream>

#include "utility.hpp"

namespace AMD {

  /** Forward declaration so we can have function prototypes */
  struct SymbolicScalarMatlab;
  struct SymbolicMatrixMatlab;

  static SymbolicScalarMatlab trace(const SymbolicMatrixMatlab& a);
  static SymbolicScalarMatlab logdet(const SymbolicMatrixMatlab& a);
  static SymbolicScalarMatlab fnorm(const SymbolicMatrixMatlab& a);
  static SymbolicMatrixMatlab inv(const SymbolicMatrixMatlab& a);
  static SymbolicMatrixMatlab transpose(const SymbolicMatrixMatlab& a);
  static SymbolicMatrixMatlab diag(const SymbolicMatrixMatlab& a);
  static SymbolicMatrixMatlab elementwiseProduct(const SymbolicMatrixMatlab& a, 
                                                 const SymbolicMatrixMatlab& b);
  static SymbolicMatrixMatlab operator+(const SymbolicMatrixMatlab& a,
                                        const SymbolicMatrixMatlab& b);
  static SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a);
  static SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a,
                                        const SymbolicMatrixMatlab& b);
  static SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
                                        const SymbolicMatrixMatlab& b);
  static SymbolicMatrixMatlab operator*(const SymbolicScalarMatlab& a,
                                        const SymbolicMatrixMatlab& b);
  static SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
                                        const SymbolicScalarMatlab& b);
  static SymbolicMatrixMatlab operator/(const SymbolicMatrixMatlab& a,
                                        const SymbolicScalarMatlab& b);
  static bool operator==(const SymbolicMatrixMatlab& a,
                         const SymbolicMatrixMatlab& b);

  /**
   * @brief This class represents a symbolic matrix using an internal
   * string represenation.  The string representation can be a variable
   * name such as X, HouseHolder or a derived value such as
   * transpose(X)*X, where X is a symbolic matrix. The string
   * representation is intended to yield executable MATLAB code, but the
   * variables representing matrices must already be defined.
   */
  struct SymbolicMatrixMatlab {
    /** Default constructor giving X as the symbol name */
    SymbolicMatrixMatlab() : symbol("X"), nRows(1), nCols(1) {}

    /** Default constructor with sizes giving X as the symbol name */
    SymbolicMatrixMatlab(int m, int n) : symbol("X"), nRows(m), nCols(n) {}

    /**
     * Constructor from string value.
     * @param[in] symbol String representation of Symbolic Matrix.
     */
    SymbolicMatrixMatlab(std::string symbol)
      : symbol(symbol), nRows(1), nCols(1) {}

    /**
     * Constructor from string value.
     * @param[in] symbol String representation of Symbolic Matrix.
     * @param[in] nRows Number of rows in the symbolic matrix
     * @param[in] nCols Number of columns in the symbolic matrix
     */
    SymbolicMatrixMatlab(std::string symbol, int _nRows, int _nCols)
      : symbol(symbol), nRows(_nRows), nCols(_nCols) { }

    /**
     * Copy constructor.
     * @param[in] other SymbolicMatrixMatlab to copy.
     */
    SymbolicMatrixMatlab(const SymbolicMatrixMatlab& other) {
      symbol = other.getString();
      nRows = other.getNumRows();
      nCols = other.getNumCols();
    }

    /**
     * Return the string representation of the class.
     * @return The value of the internal string representation.
     */
    std::string getString() const { return(symbol); }

    /**
     * Print internal string representation to a file.
     * @param[out] os Outputstream to print to.
     */
    void print(std::ostream& os = std::cout) const {
      os << detail::removeParenthesis(symbol);
    }

    /**
     * Print internal string representation to a file.
     * @param[out] os Outputstream to print to.
     */
    void println(std::ostream& os = std::cout) const {
      os << detail::removeParenthesis(symbol) << std::endl;
    }

    // Below are functions needed to plug into the automatic
    // differentiation class.

    // number of rows and columns are considered irrelevant in this
    // symbolic differentiation class.

    /**
     * number of Rows in matrix.  For symbolic matrices this is always 1.
     * @return number of SymbolicMatrix rows.
     */
    int getNumRows() const { return(nRows); }

    /**
     * number of Columns in matrix.  For symbolic matrices this is always 1.
     * @return number of SymbolicMatrix columns.
     */
    int getNumCols() const { return(nCols); }

    /**
     * Copy content of one SymbolicMatrixMatlab to another.
     * @param[in] rhs The matrix that we are copying .
     * @return SymbolicMatrixMatlab object which is the deep-copy from another.
     */
    SymbolicMatrixMatlab& operator=(const SymbolicMatrixMatlab& rhs) {
      symbol = rhs.getString();
      nRows = rhs.getNumRows();
      nCols = rhs.getNumCols();
      return(*this);
    }
    /**
     * Copy content of one SymbolicMatrixMatlab to another.
     * @param[in] rhs The matrix that we are copying.
     * @return SymbolicMatrixMatlab object which is the deep-copy from another.
     */
    SymbolicMatrixMatlab& copy(const SymbolicMatrixMatlab& rhs) {
      symbol = rhs.getString();
      nRows = rhs.getNumRows();
      nCols = rhs.getNumCols();
      return(*this);
    }

    /**
     * Return representation of nxn identity matrix.
     * @param[in] n The size of the identity matrix.
     * @return the SymbolicScalarMatlab eye(n)
     */
    static SymbolicMatrixMatlab eye(int n)  {
      std::ostringstream ss;
      ss << "eye(" << n << ")";
      return SymbolicMatrixMatlab(ss.str(), n, n);
    }

    /**
     * Return representation of mxn zero matrix.
     * @param[in] m Number of rows.
     * @param[in] n Number of cols.
     * @return the SymbolicScalarMatlab zeros(m,n)
     */
    static SymbolicMatrixMatlab zeros(int m, int n)  {
      std::ostringstream ss;
      ss << "zeros(" << m << "," << n << ")";
      return SymbolicMatrixMatlab(ss.str(), m, n);
    }

    friend SymbolicScalarMatlab trace(const SymbolicMatrixMatlab& a);
    friend SymbolicScalarMatlab logdet(const SymbolicMatrixMatlab& a);
    friend SymbolicScalarMatlab fnorm(const SymbolicMatrixMatlab& a);
    friend SymbolicMatrixMatlab inv(const SymbolicMatrixMatlab& a);
    friend SymbolicMatrixMatlab transpose(const SymbolicMatrixMatlab& a);
    friend SymbolicMatrixMatlab diag(const SymbolicMatrixMatlab& a);
    friend SymbolicMatrixMatlab elementwiseProduct
        (const SymbolicMatrixMatlab& a, const SymbolicMatrixMatlab& b);
    friend SymbolicMatrixMatlab operator+(const SymbolicMatrixMatlab& a,
                                          const SymbolicMatrixMatlab& b);
    friend SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a);
    friend SymbolicMatrixMatlab operator-(const SymbolicMatrixMatlab& a,
                                          const SymbolicMatrixMatlab& b);
    friend SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
                                          const SymbolicMatrixMatlab& b);
    friend SymbolicMatrixMatlab operator*(const SymbolicScalarMatlab& a,
                                          const SymbolicMatrixMatlab& b);
    friend SymbolicMatrixMatlab operator*(const SymbolicMatrixMatlab& a,
                                          const SymbolicScalarMatlab& b);
    friend SymbolicMatrixMatlab operator/(const SymbolicMatrixMatlab& a,
                                          const SymbolicScalarMatlab& b);
    friend bool operator==(const SymbolicMatrixMatlab& a,
                           const SymbolicMatrixMatlab& b);

    private:
    std::string symbol; /**< This is the internal string variable */
    int nRows;          /**< The number of SymbolicMatrixMatlab rows. */
    int nCols;          /**< The number of SymbolicMatrixMatlab columns. */
  };

} /** namespace AMD */

#endif /** AMD_SymbolicMatrixMatlab_HPP */
