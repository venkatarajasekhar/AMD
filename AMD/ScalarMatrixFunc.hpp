#ifndef ScalarMatrixFunc_H
#define ScalarMatrixFunc_H

/**
 * @file ScalarMatrixFunc.hpp
 *
 * @author Peder Olsen, Anju Kambadur
 *
 * @brief This file defines a Scalar-Matrix Function class and
 * the operator overloading for this class.This class act as the root node
 * in the computational tree. Currently the program can
 * compute derivates for Scalar-Matrix Function "trace" and "logdet".
 **/

#include <string>
#include <cstdio>
#include <iostream>
#include "MatrixAdaptor.hpp"
#include "MatrixMatrixFunc.hpp"
#include "Exception.hpp"

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
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    typedef MatrixMatrixFunc<MT, ST> MatrixMatrixFuncType;
    typedef MatrixMatrixFuncType MMF;

    ST functionVal;
    MT derivativeVal;
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> > derivativeFuncVal;
    bool isConst;
    /**
     * @brief Constructor for a ScalarMatrixFunc object. The default
     * setting is a variable.
     */
    ScalarMatrixFunc() : 
      functionVal(), 
      derivativeVal(), 
      derivativeFuncVal(), 
      isConst(false) { }

    ~ScalarMatrixFunc() { }


    /**
     * @brief Constructor from functionVal and derivativeVal.
     *
     * @param[in] fVal ScalarType function variable.
     * @param[in] dVal MatrixType function variable.
     */
    ScalarMatrixFunc(ST fVal, MT dVal) : 
      functionVal(fVal), 
      derivativeVal(dVal), 
      derivativeFuncVal(), 
      isConst(false) { }

    ScalarMatrixFunc(ST fVal, MT dVal, MMF dFuncVal)
      : functionVal(fVal), derivativeVal(dVal), isConst(false)
    {
      boost::shared_ptr<MatrixMatrixFunc<MT, ST> >
        copy(new MatrixMatrixFunc<MT, ST>);
      derivativeFuncVal = copy;
      derivativeFuncVal->deepCopy(dFuncVal);
    }

    /// Constructor for constant functions
    /// give m, n to indicate the size of the derivative matrix.
    /**
     * @brief Constructor for constant functions.
     *
     * @param[in] fVal  ScalarType function variable.
     * @param[in] m     Number of rows.
     * @param[in] n     Number of columns.
     */
    ScalarMatrixFunc(ST fVal, int m, int n) : 
      functionVal(fVal), 
      derivativeVal(MatrixAdaptorType::zeros(m, n)),
      derivativeFuncVal(),
      isConst(true){ }

    /**
     * @brief Operator overloading for "=". rhs and lhs are
     * ScalarMatrixFunc objects. Assign the value of
     * rhs to lhs.
     *
     * @param[in] x ScalarmatrixFunc rhs.
     */
    void initWithVariable(ST fVal, MT dVal) {
      functionVal = fVal;
      derivativeVal = dVal;
      isConst = false;
    }

    void initWithConst(ST fVal, int m, int n) {
      functionVal = fVal;
      derivativeVal = MatrixAdaptorType::zeros(m, n);
      isConst = true;
    }

    ScalarMatrixFunc& operator= (const ScalarMatrixFunc &x) {
      functionVal = x.functionVal;
      derivativeVal = x.derivativeVal;
      derivativeFuncVal = x.derivativeFuncVal;
      isConst = x.isConst;
      return(*this);
    }

    /**
     * @brief Print out function value and derivative value to output stream.
     * @param[in] os  output stream to print to, 
     *                standard output stream by default 
     */
   void print(std::ostream& os = std::cout) const { 
     os << "Function value: " << functionVal.getString() << std::endl;
     os << "Derivate value: " << derivativeVal.getString() << std::endl;
   }

  }; // end ScalarMatrixFunc class definition

  /**
   * @brief Friend function for ScalarMatrixFunc. Operator overloading
   * for "+".
   *
   * @tparam MT Matrix Type.
   * @tparam ST Scalar Type.
   * @return The new ScalarMatrixFunc object which is the some of rhs and lhs.
   */
  template <class MT, class ST>
  ScalarMatrixFunc<MT, ST> operator+(const ScalarMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs) {

    ScalarMatrixFunc<MT, ST> retVal;

    AMD_START_TRY_BLOCK()

    if (lhs.derivativeVal.getNumRows() != rhs.derivativeVal.getNumRows() ||
        lhs.derivativeVal.getNumCols() != rhs.derivativeVal.getNumCols())
      throw exception_generic_impl("AMD::operator+", 
                                   "Matrices don't match up", 
                                   AMD_MISMATCHED_DIMENSIONS);

    if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
      // FIXME: I do not know why if I put only *rhs.derivativeFuncVal as the
      // third value for SMF constructor(looks like *rhs.derivativeFuncVal
      // vanish somehow). I create a MMF which is a constant zero and put the
      // sum of zero and *rhs.derivativeFuncVal. This leads to extra space and
      // unnecessary computation.
      MT mat = MatrixAdaptor_t<MT>::zeros(rhs.derivativeVal.getNumRows(),
                                          rhs.derivativeVal.getNumCols());
      MatrixMatrixFunc<MT, ST> func(mat);
      retVal = ScalarMatrixFunc<MT, ST>(
                  lhs.functionVal + rhs.functionVal,
                  rhs.derivativeVal, 
                  func + *rhs.derivativeFuncVal);
    }

    if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
      MT mat = MatrixAdaptor_t<MT>::zeros(lhs.derivativeVal.getNumRows(),
        lhs.derivativeVal.getNumCols());
      MatrixMatrixFunc<MT, ST> func(mat);
      retVal = ScalarMatrixFunc<MT, ST>(
              lhs.functionVal + rhs.functionVal,
              lhs.derivativeVal, 
              *lhs.derivativeFuncVal + func);
    }

    retVal = ScalarMatrixFunc<MT, ST>(
                   lhs.functionVal + rhs.functionVal,
                   lhs.derivativeVal + rhs.derivativeVal,
                   *lhs.derivativeFuncVal+ *rhs.derivativeFuncVal);

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator+)

    return retVal;
  }

  /**
   * @brief Friend function for ScalarMatrixFunc. Operator overloading for
   * "-".
   * @param[in] lhs
   * @param[in] rhs
   *
   * @return The substraction of two ScalarMatrixFun objects.
   */
  template <class MT, class ST>
  ScalarMatrixFunc<MT, ST> operator-(const ScalarMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs) {

    ScalarMatrixFunc<MT, ST> retVal;
    AMD_START_TRY_BLOCK()

    if (lhs.derivativeVal.getNumRows() != rhs.derivativeVal.getNumRows() ||
      lhs.derivativeVal.getNumCols() != rhs.derivativeVal.getNumCols())
      throw exception_generic_impl("AMD::operator-", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);

    if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>(
                  lhs.functionVal - rhs.functionVal,
                  -rhs.derivativeVal, 
                  -(*rhs.derivativeFuncVal));
    }

    if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>(
              lhs.functionVal - rhs.functionVal,
              lhs.derivativeVal, 
              *lhs.derivativeFuncVal);
    }

    retVal =  ScalarMatrixFunc<MT, ST>(
                   lhs.functionVal - rhs.functionVal,
                   lhs.derivativeVal - rhs.derivativeVal,
                   *lhs.derivativeFuncVal - *rhs.derivativeFuncVal);

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator-)
    
    return retVal;
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
  ScalarMatrixFunc<MT, ST> operator-(const ScalarMatrixFunc<MT, ST> &lhs) {
    /*
    if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
    return( ScalarMatrixFunc<MT,ST>( -lhs.functionVal,
    lhs.derivativeVal ) );
    }*/
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MT lcTrans;
    MatrixAdaptorType::negation(lhs.derivativeVal, lcTrans);
    return(ScalarMatrixFunc<MT, ST>(-lhs.functionVal,
      lcTrans,
      -(*lhs.derivativeFuncVal)));
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
  // TODO  add derivativeFunc
  template <class MT, class ST>
  ScalarMatrixFunc<MT, ST> operator*(const ScalarMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs) {

    ScalarMatrixFunc<MT, ST> retVal;

    AMD_START_TRY_BLOCK()

    if (lhs.derivativeVal.getNumRows() != rhs.derivativeVal.getNumRows() ||
      lhs.derivativeVal.getNumCols() != rhs.derivativeVal.getNumCols())
      throw exception_generic_impl("AMD::operator*", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);

    const ST& f = lhs.functionVal;
    const ST& g = rhs.functionVal;
    const MT& df = lhs.derivativeVal;
    const MT& dg = rhs.derivativeVal;
    if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>
            (f*g, f*dg, lhs*(*rhs.derivativeFuncVal));
    }
    if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>
                      (f*g, df*g, (*lhs.derivativeFuncVal)*rhs);
    }

    retVal = ScalarMatrixFunc<MT, ST>(
               f*g, 
               f*dg + df*g,
               lhs * (*rhs.derivativeFuncVal) + (*lhs.derivativeFuncVal)*rhs);

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator*)

    return retVal;
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
  // TODO  add derivativeFunc
  /*
  template <class MT, class ST>
  ScalarMatrixFunc<MT,ST> operator*( const ST &lhs,
  const ScalarMatrixFunc<MT,ST> &rhs ) {
  ScalarMatrixFunc<MT,ST>
  f(lhs,rhs.derivativeVal.getNumRows(),rhs.derivativeVal.getNumRows());
  return(f*rhs);
  }

  */
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
  // TODO  add derivativeFunc
  // 
  /*
  template <class MT, class ST>
  ScalarMatrixFunc<MT,ST> operator*( const ScalarMatrixFunc<MT,ST> &lhs,
  const ST &rhs) {
      ScalarMatrixFunc<MT,ST>
      f(rhs,lhs.derivativeVal.getNumCols(),lhs.derivativeVal.getNumCols());
  return(lhs*f);
  }
  */

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
  ScalarMatrixFunc<MT, ST> operator/(const ScalarMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs) {

    ScalarMatrixFunc<MT, ST> retVal;

    AMD_START_TRY_BLOCK()

    if (lhs.derivativeVal.getNumRows() != rhs.derivativeVal.getNumRows() ||
      lhs.derivativeVal.getNumCols() != rhs.derivativeVal.getNumCols())
      throw exception_generic_impl("AMD::operator/", 
        "Matrices don't match up", AMD_MISMATCHED_DIMENSIONS);

    const ST& f = lhs.functionVal;
    const ST& g = rhs.functionVal;
    const MT& df = lhs.derivativeVal;
    const MT& dg = rhs.derivativeVal;
    if (lhs.isConst) {// i.e. lhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>(f + g, (-f*dg) / (g*g));
    }
    if (rhs.isConst) {// i.e. rhs.derivativeVal == zero
      retVal = ScalarMatrixFunc<MT, ST>(f + g, (df*g) / (g*g));
    }

    retVal = ScalarMatrixFunc<MT, ST>(f + g, (df*g - f*dg) / (g*g));

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_RETHROW(AMD, operator/)

    return retVal;
  }

  /**
   * @brief Stream insertion operator overloading.
   * @param MT matrix type
   * @param ST scalar type
   *
   * @param[in] os  output strean to write to
   * @param[in] smf ScalarMatrixFunc to be written to output stream
   */
  template <class MT, class ST>
  std::ostream& operator<<(std::ostream& os, 
                           const ScalarMatrixFunc<MT, ST> smf) {
    os << "Function value: " << smf.functionVal.getString() << std::endl;
    os << "Derivate value: " << smf.derivativeVal.getString() << std::endl;
    return os;
  }

}  /** namespace AMD */

#endif
