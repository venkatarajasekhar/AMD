#ifndef MatrixMatrixFuncHelper_HPP
#define MatrixMatrixFuncHelper_HPP

/**
 * @file MatrixMatrixFuncHelper.hpp
 *
 * @author Peder Olsen, Anju Kambadur, Suyang Zhu, Allan Sapucaia
 *
 * @brief This file defines different types call-back functions
 * (callBackFunc) and operator overloading for MatrixMatrixFunc class.
 */

#include <string>
#include <cstdio>
#include "boost/shared_ptr.hpp"
#include "utility.hpp"
#include "ScalarMatrixFunc.hpp"
#include "MatrixAdaptor.hpp"
#include "Exception.hpp"

namespace AMD {
  /**
   * @enum Enum type for operators.
   */
  enum OpType {
    NONE,
    CONST,
    VAR,
    PLUS,
    MINUS,
    NEGATION,
    TIMES,
    STIMESM, /**< Scalar times Matrix */
    MTIMESS, /**< Matrix times Scalar */
    ELEWISE,
    TRANSPOSE,
    INV,
    DIAG
  };

  /* enum type for possible status of a matrix. The first one is a progenerate
   * case, whereas the rest are degenerate ones*/
  enum MatrixType {
    kGeneral, //general; common case
    kNormal, //progenerate; common case
    kInvalid, //needs to be initialised
    kZero, //zero matrix
    kIdentity, //identity matrix
    kTranspose //transpose of a matrix
  };

  static std::string opName[] = { "none",
                                  "const",
                                  "var",
                                  "+",
                                  "-",
                                  "-",
                                  "*",
                                  "*",
                                  "*",
                                  "elementwise",
                                  "transpose",
                                  "inv",
                                  "diag"
                                };

  // forward declaration
  template <class MT, class ST> class MatrixMatrixFunc;
  template <class MT, class ST> class ScalarMatrixFunc;

  template <class MT, class ST>
  static void unaryDiffOpStandardCheck(const MatrixMatrixFunc<MT, ST>* node,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    const OpType& opType) {
    /** 1. This is a unary operation --- left child valid */
    if (NULL == node) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Node pointer is NULL",
        AMD_NULL_PTR);
    }
    else if (opType != node->opNum) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Operation is inconsistent",
        AMD_INVALID_OPERATION);
    }
    else if (NULL == node->leftChild) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Node left pointer is NULL",
        AMD_NULL_PTR);
    }
    else if (NULL != node->rightChild && NULL != node->scalarChild) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Node right ptr or scalar ptr are not NULL",
        AMD_NULL_PTR);
    }

    /** 2. The shared pointers should contain valid pointers */
    if (1 > current.use_count()) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Current use count is not right",
        AMD_INVALID_SHARED_PTR);
    }
    else if (1 > currentLeft.use_count()) {
      throw exception_generic_impl("AMD::unaryDiffOpStandardCheck",
        "Current-Left use count is not right",
        AMD_INVALID_SHARED_PTR);
    }
  }

  template <class MT, class ST>
  static void binaryDiffOpStandardCheck(const MatrixMatrixFunc<MT, ST>* node,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    const OpType& opType) {
    /** 1. This is a binary operation --- left and right children valid */
    if (NULL == node) {
      throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
        "Node pointer is NULL",
        AMD_NULL_PTR);
    }
    else if (opType != node->opNum) {
      throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
        "Operation is inconsistent",
        AMD_INVALID_OPERATION);
    }
    else if (NULL == node->leftChild) {
      throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
        "Node left pointer is NULL",
        AMD_NULL_PTR);
    }
    else if (1 > current.use_count()) {
      throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
        "Current use count is not right",
        AMD_INVALID_SHARED_PTR);
    }
    else if (1 > currentLeft.use_count()) {
      throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
        "Current-Left use count is not right",
        AMD_INVALID_SHARED_PTR);
    }

    if (STIMESM == opType || MTIMESS == opType) {
      if (NULL == node->scalarChild) {
        throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
          "Node scalarChild pointer is NULL",
          AMD_NULL_PTR);
      }
      else if (NULL != node->rightChild) {
        throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
          "Node rightChild pointer is NOT NULL",
          AMD_NOT_NULL_PTR);
      }
    }
    else {
      if (NULL == node->rightChild) {
        throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
          "Node rightChild pointer is NULL",
          AMD_NULL_PTR);
      }
      else if (NULL != node->scalarChild) {
        throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
          "Node scalarChild pointer is NOT NULL",
          AMD_NOT_NULL_PTR);
      }
      else if (1 > currentRight.use_count()) {
        throw exception_generic_impl("AMD::binaryDiffOpStandardCheck",
          "Current-Right use count is not right",
          AMD_INVALID_SHARED_PTR);
      }
    }
  }

  template <class MT, class ST>
  static void unaryOpStandardCheck(const MatrixMatrixFunc<MT, ST> &lhs,
    const OpType& opType) {

    switch (opType) {
    case NEGATION:
    case TRANSPOSE:
    case DIAG:
    case INV:
    {
      if (NULL == lhs.leftChild) {
        throw exception_generic_impl(
          "AMD::unaryOpStandardCheck",
          "Negation of a NULL matrix",
          AMD_INVALID_ARGUMENTS);
      }
    }
      break;
    default:
      break;
    };
  }

  template <class MT, class ST>
  static void binaryOpStandardCheck(const MatrixMatrixFunc<MT, ST> &lhs,
    const MatrixMatrixFunc<MT, ST> &rhs,
    const OpType& opType,
    bool checkConst = true) {
    if (true == lhs.isConst && true == rhs.isConst) {
      throw exception_generic_impl("AMD::binaryOpStandardCheck",
        "Both LHS and RHS are constant",
        AMD_CONSTANT_FN);
    }

    switch (opType) {
    case PLUS:
    case MINUS:
    case ELEWISE:
    {
      if (lhs.getNumRows() != rhs.getNumRows() && lhs.mType != kZero && rhs.mType != kZero) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "+/- Mismatched number of rows",
          AMD_INVALID_ARGUMENTS);
      }
      else if (lhs.getNumCols() != rhs.getNumCols() && lhs.mType != kZero && rhs.mType != kZero) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "+/- Mismatched number of cols",
          AMD_INVALID_ARGUMENTS);
      }
    }
      break;

    case TIMES:
    {
      if (lhs.getNumCols() != rhs.getNumRows() && lhs.mType == kZero && rhs.isConst == kZero) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "* Mismatched number of rows/cols",
          AMD_INVALID_ARGUMENTS);
      }
    }
      break;

    case MTIMESS:
    {
      if (NULL == lhs.leftChild) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "* NULL left child pointer",
          AMD_INVALID_ARGUMENTS);
      }
    }
      break;

    case STIMESM:
    {
      if (NULL == rhs.leftChild) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "* NULL left child pointer",
          AMD_INVALID_ARGUMENTS);
      }
    }
      break;

    default:
      break;
    };
  }

  template <class MT, class ST>
  static void binaryOpStandardCheck(const MatrixMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs,
    const OpType& opType,
    bool checkConst = true) {
    if (false == lhs.isConst && false == rhs.isConst) {
      throw exception_generic_impl("AMD::binaryOpStandardCheck",
        "Both LHS and RHS are constant",
        AMD_CONSTANT_FN);
    }

    if (MTIMESS == opType || STIMESM == opType) {
      if (NULL == lhs.leftChild) {
        throw exception_generic_impl(
          "AMD::binaryOpStandardCheck",
          "* NULL left child pointer",
          AMD_INVALID_ARGUMENTS);
      }
    }
  }

  template<typename MT, typename ST>
  static void scalarOpDiffStandardCheck(const MatrixMatrixFunc<MT, ST>& lhs) {
    if (lhs.getNumRows() != lhs.getNumCols()) {
      throw exception_generic_impl(
        "AMD::scalarOpDiffStandardCheck",
        "scalar function (trace/logdet) called on non-square matrix",
        AMD_INVALID_ARGUMENTS);
    }
  }

  /**
   * @brief Callback function for differentiation involving constant matrices
   * since the matrix is constant the derivative is zero, and we don't
   * need to do anything.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param[in] result              Matrix after calling call back function.
   * @param[in] current             Matrix before calling call back function.
   * @param[in] currentLeft         The matrix associated with the currentLeft
   *                                child node after calling call back function.
   * @param[in] currentRight        The matrix associated with the currentRight
   *                                child node after calling call
   *                                back function.
   * @param[in] node                Current node.
   * @param[in] transposeFlag       Indicate whether the matrix for this node
   *                                is transpose matrix.
   * @param[in] identityCurrentFlag Indicate whether the matrix for this
   *                                node is identity matrix.
   * @param[in] zeroResultFlag      Indicate whether the result for this node
   *                                is zero.
   *
   */
  template <class MT, class ST>
  void constOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {

    AMD_START_TRY_BLOCK()
      if (NULL == node) throw exception_generic_impl("AMD::constOp",
        "Node pointer is NULL",
        AMD_NULL_PTR);
    if (NULL != node->leftChild || NULL != node->rightChild)
      throw exception_generic_impl("AMD::constOp",
      "Left or right children not NULL",
      AMD_INTERNAL_NODE);
    if (false == node->isConst ||
      CONST != node->opNum ||
      0 != node->getNumRows() ||
      0 != node->getNumCols())
      throw exception_generic_impl("AMD::constOp",
      "Node is not a constant",
      AMD_VARIABLE_FN);

    /** Nothing to do with constant matrices.*/
    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, constOp)
  }

  /**
   * @brief Callback function for differentiation involving a variable matrix
   * on the leaf node.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param[in] result
   * @param[in] current
   * @param[in] currentLeft
   * @param[in] currentRight
   * @param[in] node
   * @param[in] transposeFlag
   * @param[in] identityCurrentFlag
   * @param[in] zeroResultFlag
   */
  template <class MT, class ST>
  void varOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {

    AMD_START_TRY_BLOCK()

      if (NULL == node) throw exception_generic_impl("AMD::constOp",
        "Node pointer is NULL",
        AMD_NULL_PTR);
    if (NULL != node->leftChild || NULL != node->rightChild)
      throw exception_generic_impl("AMD::varOp",
      "Left or right children not NULL",
      AMD_INTERNAL_NODE);

    if (true == node->isConst ||
      VAR != node->opNum ||
      0 >= node->getNumRows() ||
      0 >= node->getNumCols())
      throw exception_generic_impl("AMD::varOp",
      "Node is a constant",
      AMD_CONSTANT_FN);

    if (1 > result.use_count() || 1 > current.use_count())
      throw exception_generic_impl("AMD::varOp",
      "Result or current is invalid",
      AMD_INTERNAL_NODE);

    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    if (zeroResultFlag) {
      zeroResultFlag = false;
      if (transposeFlag) {
        MatrixAdaptorType::transpose(*current, *result);
        (*resultMMF).deepCopy(transpose(*currentMMF));
      }
      else {
        (*result) = (*current);
        resultMMF->deepCopy(*currentMMF);
      }
    }
    else {
      if (transposeFlag) {
        // FIXME Any way to avoid tmp?
        MT cTrans;
        MatrixAdaptorType::transpose(*current, cTrans);
        MatrixAdaptorType::add(*result, cTrans, *result);
        (*resultMMF).deepCopy((*resultMMF) + transpose(*currentMMF));
      }
      else {
        MatrixAdaptorType::add((*result), (*current), (*result));
        (*resultMMF).deepCopy((*resultMMF) + (*currentMMF));
      }
    }
    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, varOp)
  }

  /**
    * @brief Functions to deal with opNum==PLUS
    * Callback function for differentiation involving plus operator
    * applying on this node.
    *
    * @tparam MT Matrix type
    * @tparam ST Scalar type
    *
    * @param result
    * @param current
    * @param currentLeft
    * @param currentRight
    * @param node
    * @param transposeFlag
    * @param identityCurrentFlag
    * @param zeroResultFlag
    */
  template <class MT, class ST>
  void plusOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, PLUS);

    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    if (currentMMF) {
      currentRightMMF->deepCopy(*currentMMF);
      currentLeftMMF->deepCopy(*currentMMF);
    }

    MatrixAdaptorType::copy(*currentLeft, *current);
    MatrixAdaptorType::copy(*currentRight, *current);

    /** currentLeft and currentRight should inherit transpose */
    if (transposeFlag) transposeFlag = 3;

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, plusOp)
  }

  /**
   * @brief MatrixMatrixFunc operator "+" overloading. Create a new node
   * which is the sum of its child node in the computational tree.
   *
   * @tparam MT Matrix type.
   * @tparam ST Scalar type.
   *
   * @param lhs
   * @param rhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator+ (const MatrixMatrixFunc<MT, ST> &lhs,
    const MatrixMatrixFunc<MT, ST> &rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      binaryOpStandardCheck(lhs, rhs, PLUS);

    MT lhsPlusRhs;
    // Add the matrices of currentLeft node and currentRight node.
    MatrixAdaptorType::add((*lhs.matrixPtr), (*rhs.matrixPtr), lhsPlusRhs);
    boost::shared_ptr<MT> sumPtr(new MT(lhsPlusRhs));
    // Initialize the node in computational tree with the new matrix and PLUS 
    // operator.
    result.binOpSet(sumPtr, PLUS, plusOp<MT, ST>, lhs, rhs);

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, operatorPlus)

      return(result);
  }

  /**
   * @brief Functions to deal with opNum==MINUS
   * Callback function for differentiation involving minus operation.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   * @param node
   * @param transposeFlag
   * @param identityCurrentFlag
   * @param zeroResultFlag
   */
  template <class MT, class ST>
  void minusOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, MINUS);

    MatrixAdaptorType::copy(*currentLeft, *current);
    currentLeftMMF->deepCopy(*currentMMF);
    MatrixAdaptorType::negation((*current), (*currentRight));
    currentRightMMF->deepCopy(-(*currentMMF));
    if (transposeFlag) {
      transposeFlag = 3; // both currentLeft and currentRight should inherit transpose
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, minusOp)
  }

  /**
   * @brief Operator - overloading. Create a new node in computational tree
   * which is the substraction of its currentLeft and currentRight child.
   *
   * @tparam MT Matrix type.
   * @tparam ST Scalar type.
   *
   * @param lhs
   * @param rhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator- (const MatrixMatrixFunc<MT, ST> &lhs,
    const MatrixMatrixFunc<MT, ST> &rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      binaryOpStandardCheck(lhs, rhs, MINUS);

    // The new node.
    MT lhsMinusRhs;
    MatrixAdaptorType::minus(*(lhs.matrixPtr), *(rhs.matrixPtr), lhsMinusRhs);
    boost::shared_ptr<MT> diffPtr
      (new MT(lhsMinusRhs));
    // Initialize the new node with pointers and call back functions.
    result.binOpSet(diffPtr, MINUS, minusOp<MT, ST>, lhs, rhs);

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, operatorMinus)

      return(result);
  }

  /**
   * @brief Functions to deal with opNum==NEGATION
   * Callback function for differentiation involving the matrix transpose
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   */
  template <class MT, class ST>
  void negationOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      unaryDiffOpStandardCheck(node, current, currentLeft, currentRight, NEGATION);

    MatrixAdaptorType::negation((*current), (*currentLeft));
    currentLeftMMF->deepCopy(-(*currentMMF));
    if (transposeFlag) transposeFlag = 3;

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, negationOp)
  }

  /**
   * @brief Create a new node with operator Transpose in computational
   * tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator-(const MatrixMatrixFunc<MT, ST> &lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      unaryOpStandardCheck(lhs, NEGATION);

    /** Cancel out double negation */
    if (NEGATION != lhs.opNum) {
      MT lhsNegation;
      MatrixAdaptorType::negation(*(lhs.matrixPtr), lhsNegation);
      boost::shared_ptr<MT> negationPtr(new MT(lhsNegation));
      result.unaryOpSet(negationPtr, NEGATION, negationOp<MT, ST>, lhs);
    }
    else {
      unaryOpStandardCheck(lhs, NEGATION);
      result.deepCopy(*lhs.leftChild);
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, operatorUnaryMinus)

      return(result);
  }

  /**
   * @brief Functions to deal with opNum==TIMES
   * Callback function for differentiation involving operator*
   *
   * @tparam MT Matrix type.
   * @tparam ST Scalar type.
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   * @param node
   * @param transposeFlag
   * @param identityCurrentFlag
   * @param zeroResultFlag
   */
  template <class MT, class ST>
  void timesOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, TIMES);

    if (identityCurrentFlag) { // avoid superfluous multiplication
      transposeFlag = 0;
      if (TRANSPOSE == node->rightChild->opNum) {
        // if currentRight is R^T then get R from it's currentLeft child
        (*currentLeft) = *(node->rightChild->leftChild->matrixPtr);
        currentLeftMMF->deepCopy(*(node->rightChild->leftChild));
      }
      else {
        (*currentLeft) = *(node->rightChild->matrixPtr);
        currentLeftMMF->deepCopy(*(node->rightChild));
        transposeFlag |= 1; // set currentLeft transpose on
      }

      if (TRANSPOSE == node->leftChild->opNum) {
        // if currentRight is R^T then get R from it's currentLeft child
        (*currentRight) = *(node->leftChild->leftChild->matrixPtr);
        currentRightMMF->deepCopy(*(node->leftChild->leftChild));
      }
      else {
        (*currentRight) = *(node->leftChild->matrixPtr);
        currentRightMMF->deepCopy(*(node->leftChild));
        transposeFlag |= 2; // set currentRight transpose on
      }
      identityCurrentFlag = false;
    }
    else {
      if (transposeFlag) {
        // use A^T*B^T = (B*A)^T to reduce the numbder of trans
        // Why is this comment here? Peder?
        //(*currentLeft) = transpose(*current) * transpose(node->rightChild->val);
        MatrixAdaptorType::multiply(*(node->rightChild->matrixPtr),
          *current,
          *currentLeft);
        currentLeftMMF->deepCopy((*(node->rightChild))* (*currentMMF));
        // Why is this comment here? Peder?
        //(*currentRight) = transpose(node->leftChild->val) * transpose(*current);
        MatrixAdaptorType::multiply(*current,
          *(node->leftChild->matrixPtr),
          *currentRight);
        currentRightMMF->deepCopy((*currentMMF) * (*(node->leftChild)));
        transposeFlag = 3;
      }
      else {
        MT rcTrans, lcTrans;
        // currentLeft = current * currentRight->matrix^T
        MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), rcTrans);
        MatrixAdaptorType::multiply(*current, rcTrans, *currentLeft);
        currentLeftMMF->deepCopy((*currentMMF) * transpose(*(node->rightChild)));
        // currentRight = currentLeft->matrix^T * current
        MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), lcTrans);
        MatrixAdaptorType::multiply(lcTrans, *current, *currentRight);
        currentRightMMF->deepCopy((*(node->leftChild)) * (*currentMMF));
        transposeFlag = 0;
      }
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, timesOp)
  }

  /**
   * @brief Operator * overloading. Create a new node with "*" operator
   * in computational tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   * @param rhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator* (const MatrixMatrixFunc<MT, ST> &lhs,
    const MatrixMatrixFunc<MT, ST> &rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      binaryOpStandardCheck(lhs, rhs, TIMES);

    MT lhsTimesRhs;
    MatrixAdaptorType::multiply(*(lhs.matrixPtr), *(rhs.matrixPtr), lhsTimesRhs);
    boost::shared_ptr<MT> timesPtr(new MT((lhsTimesRhs)));

    // Initialize new node with time operator.
    result.binOpSet(timesPtr, TIMES, timesOp<MT, ST>, lhs, rhs);

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, operatorTimes)

      return(result);
  }

  /**
   * @brief Functions to deal with opNum==MTIMESS
   * MatrixMatrixFunc - ScalarMatrixFunc product.
   * Callback function for differentiation involving operator*
   *
   * @tparam MT Matrix type.
   * @tparam ST Scalar type.
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   * @param node
   * @param transposeFlag
   * @param identityCurrentFlag
   * @param zeroResultFlag
   */
  template <class MT, class ST>
  void mtimesOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, MTIMESS);

    // derivative update: 
    MT lhsTimesRhs1, lhsTimesRhs2, rcTrans;
    MatrixAdaptorType::multiply(*(node->leftChild->matrixPtr),
      *current,
      lhsTimesRhs1);
    MatrixMatrixFunc<MT, ST> mmfunc(lhsTimesRhs1, false);
    ScalarMatrixFunc<MT, ST> scalarFunc = trace(mmfunc);

    if (zeroResultFlag) {
      zeroResultFlag = false;
      if (transposeFlag) {
        MatrixAdaptorType::transpose(node->scalarChild->derivativeVal, rcTrans);
        MatrixAdaptorType::multiply(rcTrans, scalarFunc.functionVal,
          lhsTimesRhs2);
        MatrixAdaptorType::transpose(lhsTimesRhs2, *result);
        resultMMF->deepCopy          \
          (transpose(transpose(*node->scalarChild->derivativeFuncVal)*scalarFunc));
      }
      else {
        MatrixAdaptorType::multiply(node->scalarChild->derivativeVal,
          scalarFunc.functionVal, lhsTimesRhs2);
        (*result) = (lhsTimesRhs2);
        resultMMF->deepCopy((*node->scalarChild->derivativeFuncVal)*scalarFunc);
      }
    }
    else {
      if (transposeFlag) {
        MatrixAdaptorType::transpose(node->scalarChild->derivativeVal, rcTrans);
        MatrixAdaptorType::multiply(rcTrans, scalarFunc.functionVal,
          lhsTimesRhs2);
        MatrixAdaptorType::add(*result, lhsTimesRhs2, *result);
        resultMMF->deepCopy((*resultMMF) +
          transpose(*node->scalarChild->derivativeFuncVal)*scalarFunc);

      }
      else {
        MatrixAdaptorType::multiply(node->scalarChild->derivativeVal,
          scalarFunc.functionVal, lhsTimesRhs2);
        MatrixAdaptorType::add(*result, lhsTimesRhs2, *result);
        resultMMF->deepCopy((*resultMMF) +
          (*node->scalarChild->derivativeFuncVal)*scalarFunc);
      }
    }

    if (transposeFlag) {
      MatrixAdaptorType::multiply(*current, node->scalarChild->functionVal,
        *currentLeft);
      currentLeftMMF->deepCopy((*currentMMF) * (*node->scalarChild));
      transposeFlag = 3;
    }
    else {
      MT cTrans;
      MatrixAdaptorType::transpose(*current, cTrans);
      MatrixAdaptorType::multiply(cTrans, node->scalarChild->functionVal,
        *currentLeft);
      currentLeftMMF->deepCopy(transpose(*currentMMF) * (*node->scalarChild));
      transposeFlag = 0;
    }
    identityCurrentFlag = false;

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, mtimesOp)
  }

  /**
   * @brief Operator * overloading. Create a new node with "*" operator
   * in computational tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   * @param rhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator* (const MatrixMatrixFunc<MT, ST> &lhs,
    const ScalarMatrixFunc<MT, ST> &rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()

      if (MTIMESS != lhs.opNum) {
      MT lhsTimesRhs;
      // matrix times scalar
      MatrixAdaptorType::multiply(*(lhs.matrixPtr), (rhs.functionVal),
        lhsTimesRhs);
      boost::shared_ptr<MT> mtimesPtr(new MT((lhsTimesRhs)));
      // Initialize new node with mtimes operator.
      // This is a unary op
      result.unaryOpSet(mtimesPtr, MTIMESS, mtimesOp<MT, ST>, lhs);

      // the pointer points to scalar function.
      result.scalarChild = new ScalarMatrixFunc < MT, ST > ;
      *result.scalarChild = rhs;
      }
      else {
        binaryOpStandardCheck(lhs, rhs, MTIMESS, false/*don't check const*/);
        result.deepCopy(*lhs.leftChild);
      }

      AMD_END_TRY_BLOCK()
        AMD_CATCH_AND_RETHROW(AMD, operatorMTimesS)

        return(result);
  }

  /**
   * @brief Functions to deal with opNum==STIMESM
   * MatrixMatrixFunc - ScalarMatrixFunc product.
   * Callback function for differentiation involving operator*
   *
   * @tparam MT Matrix type.
   * @tparam ST Scalar type.
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   * @param node
   * @param transposeFlag
   * @param identityCurrentFlag
   * @param zeroResultFlag
   */
  template <class MT, class ST>
  void stimesmOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, STIMESM);

    // derivative update: 
    //
    MT lhsTimesRhs1, lhsTimesRhs2, rcTrans;
    MatrixAdaptorType::multiply(*(node->leftChild->matrixPtr),
      *current,
      lhsTimesRhs1);
    MatrixMatrixFunc<MT, ST> mmfunc(lhsTimesRhs1, false);
    ScalarMatrixFunc<MT, ST> scalarFunc = trace(mmfunc);

    if (zeroResultFlag) {
      zeroResultFlag = false;
      if (transposeFlag) {
        MatrixAdaptorType::transpose(node->scalarChild->derivativeVal, rcTrans);
        MatrixAdaptorType::multiply(rcTrans, scalarFunc.functionVal,
          lhsTimesRhs2);
        MatrixAdaptorType::transpose(lhsTimesRhs2, *result);
        resultMMF->deepCopy
          (transpose(transpose(*node->scalarChild->derivativeFuncVal)*scalarFunc));
      }
      else {
        MatrixAdaptorType::multiply(node->scalarChild->derivativeVal,
          scalarFunc.functionVal, lhsTimesRhs2);
        (*result) = (lhsTimesRhs2);
        resultMMF->deepCopy((*node->scalarChild->derivativeFuncVal)*scalarFunc);
      }
    }
    else {
      if (transposeFlag) {
        MatrixAdaptorType::transpose(node->scalarChild->derivativeVal, rcTrans);
        MatrixAdaptorType::multiply(rcTrans, scalarFunc.functionVal,
          lhsTimesRhs2);
        MatrixAdaptorType::add(*result, lhsTimesRhs2, *result);
        resultMMF->deepCopy((*resultMMF) +
          transpose(*node->scalarChild->derivativeFuncVal)*scalarFunc);

      }
      else {
        MatrixAdaptorType::multiply(node->scalarChild->derivativeVal,
          scalarFunc.functionVal, lhsTimesRhs2);
        MatrixAdaptorType::add(*result, lhsTimesRhs2, *result);
        resultMMF->deepCopy((*resultMMF) +
          (*node->scalarChild->derivativeFuncVal)*scalarFunc);
      }
    }

    if (transposeFlag) {
      MatrixAdaptorType::multiply(*current, node->scalarChild->functionVal,
        *currentLeft);
      currentLeftMMF->deepCopy((*currentMMF) * (*node->scalarChild));
      transposeFlag = 3;
    }
    else {
      MT cTrans;
      MatrixAdaptorType::transpose(*current, cTrans);
      MatrixAdaptorType::multiply(cTrans, node->scalarChild->functionVal,
        *currentLeft);
      currentLeftMMF->deepCopy(transpose(*currentMMF) * (*node->scalarChild));
      transposeFlag = 0;
    }
    identityCurrentFlag = false;

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, stimesmOp)
  }

  /**
   * @brief Operator * overloading. Create a new node with "*" operator
   * in computational tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   * @param rhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> operator* (const ScalarMatrixFunc<MT, ST> &lhs,
    const MatrixMatrixFunc<MT, ST> &rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()

      if (STIMESM != rhs.opNum) {
      MT lhsTimesRhs;
      // matrix times scalar
      MatrixAdaptorType::multiply(*(rhs.matrixPtr),
        (lhs.functionVal),
        lhsTimesRhs);
      boost::shared_ptr<MT> stimesmPtr(new MT((lhsTimesRhs)));

      // Initialize new node with mtimes operator.  This is a unary op
      result.unaryOpSet(stimesmPtr, STIMESM, stimesmOp<MT, ST>, rhs);

      // the pointer points to scalar function.
      result.scalarChild = new ScalarMatrixFunc < MT, ST > ;
      *result.scalarChild = lhs;
      }
      else {
        binaryOpStandardCheck(rhs, lhs, STIMESM, false/*don't check const*/);
        result.deepCopy(*rhs.leftChild);
      }

      AMD_END_TRY_BLOCK()
        AMD_CATCH_AND_RETHROW(AMD, operatorSTimesM)

        return(result);
  }

  /* Functions to deal with opNum = ELEWISE
   * Callback function for differentiation involving elementwise product
   * The elementwiseOp deal with transpose in a opposite way of timesOp.
   * TODO add resultMMF to elementwiseOp
   */
  template<class MT, class ST>
  void elementwiseOp(boost::shared_ptr<MT>   result,
    boost::shared_ptr<MT>   current,
    boost::shared_ptr<MT>   currentLeft,
    boost::shared_ptr<MT>   currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      binaryDiffOpStandardCheck(node, current, currentLeft, currentRight, ELEWISE);

    if (identityCurrentFlag) {
      // If the current matrix is identymatrix, the currentLeft and
      // currentRight matrix will be the element-wise product of the current
      // matix and the leftChild's and rightChild's matrix respectively.  This
      // is different from timesOp with ignore the identity matrix.
      transposeFlag = 0;
      if (TRANSPOSE == node->rightChild->opNum) {
        MatrixAdaptorType::elementwiseProduct(*(node->rightChild-> \
          leftChild->matrixPtr),
          *(current),
          *currentLeft);
        currentLeftMMF->deepCopy((*(node->rightChild->leftChild)) * (*currentMMF));
      }
      else {
        MatrixAdaptorType::elementwiseProduct(*(node->rightChild->matrixPtr),
          *current,
          *currentLeft);
        currentLeftMMF->deepCopy((*(node->rightChild)) * (*currentMMF));
      }
      if (TRANSPOSE == node->leftChild->opNum) {
        MatrixAdaptorType::elementwiseProduct(*(node->leftChild-> \
          leftChild->matrixPtr),
          *current,
          *currentRight);
        currentRightMMF->deepCopy(elementwiseProduct((*(node->leftChild->leftChild)),
          (*currentMMF)));
      }
      else {
        MatrixAdaptorType::elementwiseProduct(*(node->leftChild->matrixPtr),
          *current,
          *currentRight);
        currentRightMMF->deepCopy(elementwiseProduct((*(node->leftChild)),
          (*currentMMF)));
      }
      identityCurrentFlag = 0;
    }
    else {
      if (transposeFlag) {
        MT lcTrans, rcTrans;
        MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), lcTrans);
        MatrixAdaptorType::elementwiseProduct(lcTrans, *(current), *currentRight);
        currentRightMMF->deepCopy(elementwiseProduct(transpose(*(node->leftChild)),
          *currentMMF));
        MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), rcTrans);
        MatrixAdaptorType::elementwiseProduct(rcTrans, *(current), *currentLeft);
        currentLeftMMF->deepCopy(elementwiseProduct(transpose(*(node->rightChild)),
          *currentMMF));
        transposeFlag = 3;
      }
      else {
        MatrixAdaptorType::elementwiseProduct(*(node->leftChild->matrixPtr),
          *(current),
          *currentRight);
        currentRightMMF->deepCopy(elementwiseProduct((*(node->leftChild)), *currentMMF));
        MatrixAdaptorType::elementwiseProduct(*(node->rightChild->matrixPtr),
          *(current),
          *currentLeft);
        currentLeftMMF->deepCopy(elementwiseProduct((*(node->rightChild)), *currentMMF));
        transposeFlag = 0;
      }
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, elementwiseOp)
  }

  // function for elementwise 
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> elementwiseProduct
    (const MatrixMatrixFunc<MT, ST>& lhs,
    const MatrixMatrixFunc<MT, ST>& rhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      binaryOpStandardCheck(lhs, rhs, ELEWISE);

    MT lcrcEwisePdt;
    MatrixAdaptorType::elementwiseProduct(*(lhs.matrixPtr),
      *(rhs.matrixPtr),
      lcrcEwisePdt);
    boost::shared_ptr<MT> elewisePtr(new MT(lcrcEwisePdt));
    result.binOpSet(elewisePtr, ELEWISE, elementwiseOp<MT, ST>, lhs, rhs);

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, elementwiseProduct)

      return (result);
  }

  /**
   * @brief Functions to deal with opNum==TRANSPOSE
   * Callback function for differentiation involving the matrix transpose
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   */
  template <class MT, class ST>
  void transposeOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      unaryDiffOpStandardCheck(node, current, currentLeft, currentRight, TRANSPOSE);

    MatrixAdaptorType::copy((*currentLeft), (*current));
    currentLeftMMF->deepCopy(*currentMMF);
    if (!identityCurrentFlag) {
      if (transposeFlag) transposeFlag = 0;
      else transposeFlag = 3;
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, transposeOp)
  }

  /**
   * @brief Create a new node with operator Transpose in computational
   * tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   */
  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> transpose(const MatrixMatrixFunc<MT, ST> &lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()

      if (TRANSPOSE != lhs.opNum) {
      MT lhsTrans;
      MatrixAdaptorType::transpose(*(lhs.matrixPtr), lhsTrans);
      boost::shared_ptr<MT> transposePtr(new MT(lhsTrans));
      result.unaryOpSet(transposePtr, TRANSPOSE, transposeOp<MT, ST>, lhs);
      }
      else {
        unaryOpStandardCheck(lhs, TRANSPOSE);
        result.deepCopy(*lhs.leftChild);
      }

      AMD_END_TRY_BLOCK()
        AMD_CATCH_AND_RETHROW(AMD, transpose)

        return(result);
  }

  /**
   * @brief Functions to deal with opNum = DIAG
   * Callback function for differentiation involving the matrix transpose.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   *
   */
  template <class MT, class ST>
  void diagOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT>   current,
    boost::shared_ptr<MT>   currentLeft,
    boost::shared_ptr<MT>   currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node, // current node.
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      unaryDiffOpStandardCheck(node, current, currentLeft, currentRight, DIAG);

    MatrixAdaptorType::diag((*current), (*currentLeft));
    currentLeftMMF->deepCopy(*currentMMF);

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, diagOp)
  }

  /**
   * @brief Create a new node with operator Diag in computational tree.
   */

  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> diag(const MatrixMatrixFunc<MT, ST>& lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()

      if (DIAG != lhs.opNum) {
      MT lhsDiag;
      MatrixAdaptorType::diag(*(lhs.matrixPtr), lhsDiag);
      boost::shared_ptr<MT> diagPtr(new MT(lhsDiag));
      result.unaryOpSet(diagPtr, DIAG, diagOp<MT, ST>, lhs);
      }
      else {
        unaryOpStandardCheck(lhs, DIAG);
        result.deepCopy(*lhs.leftChild);
      }

      AMD_END_TRY_BLOCK()
        AMD_CATCH_AND_RETHROW(AMD, diag)

        return result;
  }

  /**
   * @brief Functions to deal with opNum==INV
   * Callback function for differentiation involving the matrix inverse
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param result
   * @param current
   * @param currentLeft
   * @param currentRight
   * @param node
   * @param transposeFlag
   * @param identityCurrentFlag
   * @param zeroResultFlag
   */
  template <class MT, class ST>
  void invOp(boost::shared_ptr<MT> result,
    boost::shared_ptr<MT> current,
    boost::shared_ptr<MT> currentLeft,
    boost::shared_ptr<MT> currentRight,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  resultMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentLeftMMF,
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >  currentRightMMF,
    const MatrixMatrixFunc<MT, ST>* node,
    int& transposeFlag,
    bool& identityCurrentFlag,
    bool& zeroResultFlag) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

    AMD_START_TRY_BLOCK()
      unaryDiffOpStandardCheck(node, current, currentLeft, currentRight, INV);

    boost::shared_ptr<MT> initMat = node->matrixPtr;
    if (!identityCurrentFlag) {
      if (transposeFlag) {
        MT lhsTimesRhs1, lhsTimesRhs2, cNeg;
        MatrixAdaptorType::multiply(*current, *initMat, lhsTimesRhs1);
        MatrixAdaptorType::multiply(*initMat, lhsTimesRhs1, lhsTimesRhs2);
        MatrixAdaptorType::negation(lhsTimesRhs2, cNeg);
        MatrixAdaptorType::copy((*currentLeft), cNeg);
        currentLeftMMF->deepCopy(-((*node)*(*currentMMF) * (*node)));

      }
      else {
        MT cTrans, lhsTimesRhs1, lhsTimesRhs2, cNeg;
        MatrixAdaptorType::transpose(*current, cTrans);
        MatrixAdaptorType::multiply(cTrans, *initMat, lhsTimesRhs1);
        MatrixAdaptorType::multiply(*initMat, lhsTimesRhs1, lhsTimesRhs2);
        MatrixAdaptorType::negation(lhsTimesRhs2, cNeg);
        MatrixAdaptorType::copy((*currentLeft), cNeg);
        currentLeftMMF->deepCopy(-((*node)*transpose(*currentMMF) * (*node)));
      }
    }
    else {
      MT lhsTimesRhs, lcNeg;
      MatrixAdaptorType::multiply(*initMat, *initMat, lhsTimesRhs);
      MatrixAdaptorType::negation(lhsTimesRhs, lcNeg);
      MatrixAdaptorType::copy((*currentLeft), lcNeg);
      currentLeftMMF->deepCopy(-((*node)*(*node)));
    }
    transposeFlag = 3;
    identityCurrentFlag = false;

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, invOp)
  }

  /**
   * @brief Operator "inv" overloading. Create a new node inverse
   * in computational tree.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   */

  template <class MT, class ST>
  MatrixMatrixFunc<MT, ST> inv(const MatrixMatrixFunc<MT, ST> &lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    MatrixMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()

      if (INV != lhs.opNum) {
      MT lhsInv;
      MatrixAdaptorType::inv(*lhs.matrixPtr, lhsInv);
      boost::shared_ptr<MT> invPtr(new MT(lhsInv));
      // Initialize the node. 
      result.unaryOpSet(invPtr, INV, invOp<MT, ST>, lhs);
      }
      else {
        unaryOpStandardCheck(lhs, INV);
        result.deepCopy(*lhs.leftChild);
      }

      AMD_END_TRY_BLOCK()
        AMD_CATCH_AND_RETHROW(AMD, inv)

        return(result);
  }

  /**
   * @brief Create the root node for a Scalar-Matrix function trace.
   * Once creating this node, the calculation of derivatives through
   * the computational tree (gradienVec) will be trigged.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   */
  template <class MT, class ST>
  ScalarMatrixFunc<MT, ST> trace(const MatrixMatrixFunc<MT, ST> &lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    ScalarMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      scalarOpDiffStandardCheck(lhs);

    const int n = MatrixAdaptorType::getNumRows(*lhs.matrixPtr);
    boost::shared_ptr<MT> initPtr(new MT);
    boost::shared_ptr<MT> resPtr(new MT);
    *initPtr = MatrixAdaptorType::eye(n);
    *resPtr = MatrixAdaptorType::zeros(lhs.getNumRows(), lhs.getNumCols());

    /** This is needed to be able to compute the derivative of derivative */
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >
      initMMF(new MatrixMatrixFunc<MT, ST>(*initPtr, false));
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >
      resultMMF(new MatrixMatrixFunc<MT, ST>(*resPtr, false));
    result.derivativeFuncVal = resultMMF;

    bool zeroFlag = true;

    /**
     * Trigger gradientVec to calculate the derivative along the computational
     * tree reversely.
     */
    lhs.gradientVec(initPtr,
      resPtr,
      initMMF,
      resultMMF,
      false,
      true,
      zeroFlag);
    if (zeroFlag) {
      result.initWithConst(MatrixAdaptorType::trace(*lhs.matrixPtr),
        lhs.getNumCols(), lhs.getNumCols());
    }
    else {
      result.initWithVariable(MatrixAdaptorType::trace(*lhs.matrixPtr),
        *resPtr);
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, trace)

      return(result);
  }

  /**
   * @brief Create the root node for Scalar-Matrix function logdet. Once
   * creating this node, the calculation of derivative along the computational
   * tree is triggered.
   *
   * @tparam MT Matrix type
   * @tparam ST Scalar type
   *
   * @param lhs
   */
  template <class MT, class ST>
  ScalarMatrixFunc<MT, ST> logdet(const MatrixMatrixFunc<MT, ST> &lhs) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    ScalarMatrixFunc<MT, ST> result;

    AMD_START_TRY_BLOCK()
      scalarOpDiffStandardCheck(lhs);

    //const int n = MatrixAdaptorType::getNumRows(*(lhs.matrixPtr)); /*unused*/
    boost::shared_ptr<MT> initPtr(new MT);
    boost::shared_ptr<MT> resPtr(new MT);
    *resPtr = MatrixAdaptorType::zeros(lhs.getNumRows(), lhs.getNumCols());
    bool transposeFlag = true;
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >
      initMMF(new MatrixMatrixFunc<MT, ST>);
    boost::shared_ptr<MatrixMatrixFunc<MT, ST> >
      resultMMF(new MatrixMatrixFunc<MT, ST>);

    if (TRANSPOSE == lhs.opNum) { // logdet(X^T) == logdet(X)
      // logdet for MMF
      return(logdet((*lhs.leftChild)));
    }
    if (INV == lhs.opNum) { // logdet(X^{-1)) == - logdet(X)
      // logdet for MMF 
      return(-logdet((*lhs.leftChild)));
    }

    // The starting point for logdet is a inverse matrix.
    MatrixAdaptorType::inv(*lhs.matrixPtr, *initPtr);
    MatrixMatrixFunc<MT, ST> initMat = inv(lhs);
    MatrixMatrixFunc<MT, ST> resultMat(*resPtr, false);
    initMMF->deepCopy(initMat);
    resultMMF->deepCopy(resultMat);
    result.derivativeFuncVal = resultMMF;
    bool zeroFlag = true;
    lhs.gradientVec(initPtr, resPtr, initMMF, resultMMF,
      transposeFlag, false, zeroFlag);
    if (zeroFlag) {
      result.initWithConst(MatrixAdaptorType::logdet(*lhs.matrixPtr),
        lhs.getNumCols(), lhs.getNumCols());
    }
    else {
      result.initWithVariable(MatrixAdaptorType::logdet(*lhs.matrixPtr),
        *resPtr);
    }

    AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, logdet)

      return(result);
  }

} /** namespace AMD */

#endif
