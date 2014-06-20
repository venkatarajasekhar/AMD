#ifndef ScalarMatrixFunc_H
#define ScalarMatrixFunc_H

#include <string>
#include <cstdio>
#include <assert.h>

namespace AMD {
/**
 * @brief A Scalar-Matrix Function class. This class is a mapping 
 * from MatrixType class to a ScalarType class. 
 * 
 * @tparam MT Type of Matrix.
 * @tparam ST type of Scalar.
 *
 */ 
template <class MT, class ST> 
class ScalarMatrixFunc {
public:
  typedef MT MatrixType;
  typedef ST ScalarType;

  ST functionVal;
  MT derivativeVal;
  bool isConst;
/**
 * @brief Constructor for a ScalarMatrixFunc object. The default 
 * setting is a variable.
 */ 
  ScalarMatrixFunc() : functionVal(), derivativeVal(), isConst(false) { }

  ~ScalarMatrixFunc() { }


  /**
   * @brief Constructor from functionVal and derivativeVal.
   *
   * @param[in] fVal ScalarType function variable.
   * @param[in] dVal MatrixType function variable.
   */ 
  ScalarMatrixFunc(ST fVal, MT dVal ) 
    : functionVal(fVal), derivativeVal(dVal), isConst(false) { }

  /// Constructor for constant functions
  /// give m, n to indicate the size of the derivative matrix.
  /**
   * @brief Constructor for constant functions. 
   *
   * @param[in] fVal  ScalarType function variable.
   * @param[in] m     Number of rows.
   * @param[in] n     Number of columns.
   */ 
  ScalarMatrixFunc(ST fVal, int m, int n ) 
    : functionVal(fVal), derivativeVal(MT::zeros(m,n)), isConst(true) { }

  /**
   * @brief Operator overloading for "=". rhs and lhs are 
   * ScalarMatrixFunc objects. Assign the value of 
   * rhs to lhs.
   *
   * @param[in] x ScalarmatrixFunc rhs.
   */
  ScalarMatrixFunc& operator= ( const ScalarMatrixFunc &x) {
    functionVal = x.functionVal;
    derivativeVal = x.derivativeVal;
    isConst = x.isConst;
    return(*this);
  }

}; // end ScalarMatrixFunc class definition

/**
 * @brief Friend function for ScalarMatrixFunc. Operator overloading 
 * for "+".
 *
 * @tparam MT Matrix Type.
 * @tparam ST Scalar Type.
 *
 */ 
template <class MT, class ST> 

/**
 * @param[in] lhs 
 * @param[in] rhs
 * 
 * @return The new ScalarMatrixFunc object which is the some of rhs and lhs.
 */
ScalarMatrixFunc<MT,ST> operator+( const ScalarMatrixFunc<MT,ST> &lhs,
				   const ScalarMatrixFunc<MT,ST> &rhs ) {
  assert( lhs.derivativeVal.getNumRows() == rhs.derivativeVal.getNumRows() &&
	  lhs.derivativeVal.getNumCols() == rhs.derivativeVal.getNumCols() );
  if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( lhs.functionVal+rhs.functionVal,
				     rhs.derivativeVal ) );
  }
  if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( lhs.functionVal+rhs.functionVal,
				     lhs.derivativeVal ) );
  }
    
  return( ScalarMatrixFunc<MT,ST>( lhs.functionVal+rhs.functionVal,
				   lhs.derivativeVal+rhs.derivativeVal ) );
}

/**
 * @brief Friend function for ScalarMatrixFunc. Operator overloading for
 * "-".
 *
 * @tparam MT Matrix Type.
 * @tparam ST Scalar Type.
 */ 
template <class MT, class ST> 
/**
 * @param[in] lhs
 * @param[in] rhs
 *
 * @return The substraction of two ScalarMatrixFun objects.
 */
ScalarMatrixFunc<MT,ST> operator-( const ScalarMatrixFunc<MT,ST> &lhs,
				   const ScalarMatrixFunc<MT,ST> &rhs ) {
  assert( lhs.derivativeVal.getNumRows() == rhs.derivativeVal.getNumRows() &&
	  lhs.derivativeVal.getNumCols() == rhs.derivativeVal.getNumCols() );
  if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( lhs.functionVal-rhs.functionVal,
				     -rhs.derivativeVal ) );
  }
  if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( lhs.functionVal-rhs.functionVal,
				     lhs.derivativeVal ) );
  }
    
  return( ScalarMatrixFunc<MT,ST>( lhs.functionVal-rhs.functionVal,
				   lhs.derivativeVal-rhs.derivativeVal ) );
}

  // unary minus
/**
 * @brief Operator overloading for "-".
 *
 * @tparam MT Matrix Type.
 * @tparam ST Scalar Type.
 *
 * @param[in] lhs
 * 
 * @return -lhs
 */
template <class MT, class ST> 
ScalarMatrixFunc<MT,ST> operator-( const ScalarMatrixFunc<MT,ST> &lhs ) {
  /*
  if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( -lhs.functionVal,
				     lhs.derivativeVal ) );
  }*/
  return( ScalarMatrixFunc<MT,ST>( -lhs.functionVal,
				   -lhs.derivativeVal ) );
}

/**
 * @brief Operator overloading for "*".
 *
 * @tparam MT Matrix Type.
 * @tparam ST Scalar Type.
 *
 * @param[in] lhs
 * @param[in] rhs
 * 
 * @return lhs * rhs
 */
template <class MT, class ST> 
ScalarMatrixFunc<MT,ST> operator*( const ScalarMatrixFunc<MT,ST> &lhs,
				   const ScalarMatrixFunc<MT,ST> &rhs ) {
  assert( lhs.derivativeVal.getNumRows() == rhs.derivativeVal.getNumRows() &&
	  lhs.derivativeVal.getNumCols() == rhs.derivativeVal.getNumCols() );
  const ST& f = lhs.functionVal;
  const ST& g = rhs.functionVal;
  const MT& df = lhs.derivativeVal;
  const MT& dg = rhs.derivativeVal;
  if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return(ScalarMatrixFunc<MT,ST>(f+g, f*dg));
  } 
  if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
    return(ScalarMatrixFunc<MT,ST>(f+g, df*g));
  } 
  return(ScalarMatrixFunc<MT,ST>(f+g, f*dg+df*g));
}

/**
 * @brief Operator overloading for "/".
 *
 * @tparam MT Matrix Type.
 * @tparam ST Scalar Type.
 *
 * @param[in] lhs
 * @param[in] rhs
 * 
 * @return lhs / rhs
 */
template <class MT, class ST> 
ScalarMatrixFunc<MT,ST> operator/( const ScalarMatrixFunc<MT,ST> &lhs,
				   const ScalarMatrixFunc<MT,ST> &rhs ) {
  assert( lhs.derivativeVal.getNumRows() == rhs.derivativeVal.getNumRows() &&
	  lhs.derivativeVal.getNumCols() == rhs.derivativeVal.getNumCols() );
  const ST& f = lhs.functionVal;
  const ST& g = rhs.functionVal;
  const MT& df = lhs.derivativeVal;
  const MT& dg = rhs.derivativeVal;
  if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return(ScalarMatrixFunc<MT,ST>(f+g, (-f*dg)/(g*g)));
  }
  if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
    return(ScalarMatrixFunc<MT,ST>(f+g, (df*g)/(g*g)));
  }
  return(ScalarMatrixFunc<MT,ST>(f+g, (df*g-f*dg)/(g*g)));
}


}  /** namespace AMD */

#endif
