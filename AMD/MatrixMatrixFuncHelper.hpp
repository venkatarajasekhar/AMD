#ifndef MatrixMatrixFunHelper_H
#define MatrixMatrixFuncHelper_H

/**
 * @file MatrixMatrixFuncHelper.hpp
 *
 * @author Peder Olsen, Anju Kambadur, Suyang Zhu
 *
 * @brief This file defines different types call-back functions
 * (callBackFunc) and operator overloading for MatrixMatrixFunc class. 
 */

#include <string>
#include <cstdio>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include "utility.hpp"
#include "ScalarMatrixFunc.hpp"
#include "MatrixAdaptor.hpp"

namespace AMD {
/**
 * @enum Enum type for operators.
 */
enum OpType { NONE, 
              CONST, 
              VAR, 
              PLUS, 
              MINUS, 
              TIMES,
              STIMESM, /**< Scalar times Matrix */
              MTIMESS, /**< Matrix times Scalar */
              ELEWISE, 
              TRANSPOSE, 
              INV, 
              DIAG};
std::string opName[] = { "none", 
                         "const", 
                         "var", 
                         "+", 
                         "-", 
                         "*",
                         "*",
                         "*", 
                         "elementwise", 
                         "transpose", 
                         "inv", 
                         "diag" };

// forward declaration
template <class MT, class ST> class MatrixMatrixFunc;

/** 
 * @brief Callback function for differentiation involving constant matrices
 * since the matrix is constant the derivative is zero, and we don't
 * need to do anything.
 *
 * @tparam MT Matrix type
 * @tparam ST Scalar type
 *
 * @param[in] result                Matrix after calling call back function.
 * @param[in] current               Matrix before calling call back function.
 * @param[in] left                  The matrix associated with the left child
 *                                  node after calling call back function.
 * @param[in] right                 The matrix associated with the right 
 *                                  child node after calling call 
 *                                  back function.
 * @param[in] node                  Current node.
 * @param[in] transposeFlag         Indicate whether the matrix for this node 
 *                                  is transpose matrix.
 * @param[in] identityCurrentFlag   Indicate whether the matrix for this 
 *                                  node is identity matrix.
 * @param[in] zeroResultFlag        Indicate whether the result for this node
 *                                  is zero.
 *
 */
template <class MT,class ST>
void constOp(boost::shared_ptr<MT> result, 
             boost::shared_ptr<MT> current, 
	           boost::shared_ptr<MT> left, 
             boost::shared_ptr<MT> right,
	           const MatrixMatrixFunc<MT,ST>* node, 
	           int& transposeFlag,
	           bool& identityCurrentFlag, 
	           bool& zeroResultFlag) {
    assert( NULL != node && // check node type. 
	          NULL == node->leftChild &&  // node must be leaf node.
	          NULL == node->rightChild &&
	          node->isConst &&
	          CONST == node->opNum && 
	          0 == node->varNumRows &&
	          0 == node->varNumCols);
    // Nothing to do with constant matrices.
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
 * @param[in] left
 * @param[in] right
 * @param[in] node
 * @param[in] transposeFlag
 * @param[in] identityCurrentFlag
 * @param[in] zeroResultFlag
 */
template <class MT, class ST>
void varOp(boost::shared_ptr<MT> result, 
	   boost::shared_ptr<MT> current, 
	   boost::shared_ptr<MT> left, 
	   boost::shared_ptr<MT> right,
	   const MatrixMatrixFunc<MT,ST>* node, 
	   int& transposeFlag,
	   bool& identityCurrentFlag, 
	   bool& zeroResultFlag) {
    assert( NULL != node && // check node type
	  NULL == node->leftChild &&  // node must be leaf node.
	  NULL == node->rightChild &&
	  !node->isConst &&
	  VAR == node->opNum && 
	  0 <= node->varNumRows &&
	  0 <= node->varNumCols &&
	  result.use_count() >= 1 && // result and current must be valid
	  current.use_count() >= 1 );
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  if (zeroResultFlag) {
    zeroResultFlag = false;
    if (transposeFlag) {
      MatrixAdaptorType::transpose(*current, *result);  

    } else {
      (*result) = (*current);
    }
  } else {
    if (transposeFlag) {
      // FIXME Any way to avoid tmp?
      MT cTrans;
      MatrixAdaptorType::transpose(*current, cTrans);
      MatrixAdaptorType::add(*result, cTrans, *result);
    } else {
      MatrixAdaptorType::add((*result), (*current), (*result));
    }
  }
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
  * @param left
  * @param right
  * @param node
  * @param transposeFlag
  * @param identityCurrentFlag
  * @param zeroResultFlag
  */
template <class MT, class ST>
void plusOp( boost::shared_ptr<MT> result, 
	     boost::shared_ptr<MT> current, 
	     boost::shared_ptr<MT> left, 
	     boost::shared_ptr<MT> right,
	     const MatrixMatrixFunc<MT,ST>* node, 
	     int& transposeFlag,
	     bool& identityCurrentFlag, 
	     bool& zeroResultFlag) {
  assert( NULL != node && // check node type
	  NULL != node->leftChild &&
	  NULL != node->rightChild &&
	  PLUS == node->opNum &&
	  current.use_count()>=1 && // current, left and right must be present 
	  left.use_count()>=1 && 
	  right.use_count()>=1 );
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixAdaptorType::copy(*left, *current);
  MatrixAdaptorType::copy(*right, *current);
  if (transposeFlag) {
    transposeFlag=3; // both left and right should inherit transpose
  }
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
MatrixMatrixFunc<MT,ST> operator+ (const MatrixMatrixFunc<MT,ST> &lhs, 
				                           const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert(lhs.isConst || 
         rhs.isConst || 
	       (lhs.varNumRows==rhs.varNumRows && 
	       lhs.varNumCols==rhs.varNumCols));
  // The new node of MatrixMatrixFunction.
  MatrixMatrixFunc<MT,ST> result;
  MT lhsPlusRhs;
  // Add the matrices of left node and right node.
  MatrixAdaptorType::add((*lhs.matrixPtr),(*rhs.matrixPtr), lhsPlusRhs);
  boost::shared_ptr<MT> sumPtr(new MT(lhsPlusRhs));
  // Initialize the node in computational tree with the new matrix and PLUS 
  // operator.
  result.binOpSet( sumPtr, PLUS, plusOp<MT,ST>, lhs, rhs );
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
 * @param left
 * @param right
 * @param node
 * @param transposeFlag
 * @param identityCurrentFlag
 * @param zeroResultFlag
 */
template <class MT, class ST>
void minusOp( boost::shared_ptr<MT> result, 
	      boost::shared_ptr<MT> current, 
	      boost::shared_ptr<MT> left, 
	      boost::shared_ptr<MT> right,
	      const MatrixMatrixFunc<MT,ST>* node, 
	      int& transposeFlag,
	      bool& identityCurrentFlag, 
	      bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( NULL != node && // check node type
	        NULL != node->leftChild &&
	        NULL != node->rightChild &&
	        MINUS == node->opNum &&
	        current.use_count()>=1 && // current, left and right must be present 
	        left.use_count()>=1 && 
	        right.use_count()>=1 );
  MatrixAdaptorType::copy(*left, *current);
  MatrixAdaptorType::negation((*current), (*right));
  if (transposeFlag) {
    transposeFlag=3; // both left and right should inherit transpose
  }
}
/**
 * @brief Operator - overloading. Create a new node in computational tree 
 * which is the substraction of its left and right child.
 *
 * @tparam MT Matrix type.
 * @tparam ST Scalar type.
 *
 * @param lhs
 * @param rhs
 */
template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator- (const MatrixMatrixFunc<MT,ST> &lhs, 
                        				   const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert(lhs.isConst || 
         rhs.isConst || 
	       (lhs.varNumRows==rhs.varNumRows && 
	       lhs.varNumCols==rhs.varNumCols));
  // The new node.
  MatrixMatrixFunc<MT,ST> result;
  MT lhsMinusRhs;
  MatrixAdaptorType::minus(*(lhs.matrixPtr), *(rhs.matrixPtr), lhsMinusRhs);
  boost::shared_ptr<MT> diffPtr
   (new MT(lhsMinusRhs));
  // Initialize the new node with pointers and call back functions.
  result.binOpSet( diffPtr, MINUS, minusOp<MT,ST>, lhs, rhs );
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
 * @param left
 * @param right
 * @param node
 * @param transposeFlag
 * @param identityCurrentFlag
 * @param zeroResultFlag
*/
template <class MT, class ST>
void timesOp( boost::shared_ptr<MT> result, 
	      boost::shared_ptr<MT> current, 
	      boost::shared_ptr<MT> left, 
	      boost::shared_ptr<MT> right,
	      const MatrixMatrixFunc<MT,ST>* node, 
	      int& transposeFlag,
	      bool& identityCurrentFlag, 
	      bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( NULL != node && // check node type
	  NULL != node->leftChild &&
	  NULL != node->rightChild &&
	  TIMES == node->opNum &&
	  current.use_count()>=1 && // current, left and right must be present 
	  left.use_count()>=1 && 
	  right.use_count()>=1 );
  if (identityCurrentFlag) { // avoid superfluous multiplication
    transposeFlag = 0;
    if (TRANSPOSE == node->rightChild->opNum) {
      // if right is R^T then get R from it's left child
      (*left) = *(node->rightChild->leftChild->matrixPtr);
    } else {
      (*left) = *(node->rightChild->matrixPtr);
      transposeFlag |= 1; // set left transpose on
    }
    if (TRANSPOSE==node->leftChild->opNum) {
      // if right is R^T then get R from it's left child
      (*right) = *(node->leftChild->leftChild->matrixPtr);
    } else {
      (*right) = *(node->leftChild->matrixPtr);
      transposeFlag |= 2; // set right transpose on
    }
    identityCurrentFlag = false;
  } else {
    if (transposeFlag) {
      // use A^T*B^T = (B*A)^T to reduce the numbder of trans
      // Why is this comment here? Peder?
      //(*left) = transpose(*current) * transpose(node->rightChild->val);
      MatrixAdaptorType::multiply(*(node->rightChild->matrixPtr), 
                                  *current, 
                                  *left);

      // Why is this comment here? Peder?
      //(*right) = transpose(node->leftChild->val) * transpose(*current);
      MatrixAdaptorType::multiply(*current, 
                                  *(node->leftChild->matrixPtr), 
                                  *right);

      transposeFlag = 3;
    } else {
      MT rcTrans, lcTrans;
      // left = current * right->matrix^T
      MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), rcTrans);
      MatrixAdaptorType::multiply(*current, rcTrans, *left);

      // right = left->matrix^T * current
      MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), lcTrans);
      MatrixAdaptorType::multiply(lcTrans, *current, *right);

      transposeFlag = 0;
    }
  }
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
MatrixMatrixFunc<MT,ST> operator* (const MatrixMatrixFunc<MT,ST> &lhs, 
			                        	   const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst || 
          rhs.isConst || 
	        (lhs.varNumRows==rhs.varNumRows && 
	        lhs.varNumCols==rhs.varNumCols));
  // New node in computational tree.
  MatrixMatrixFunc<MT,ST> result;
  MT lhsTimesRhs;
  MatrixAdaptorType::multiply(*(lhs.matrixPtr), *(rhs.matrixPtr), lhsTimesRhs);
  boost::shared_ptr<MT> timesPtr(new MT((lhsTimesRhs)));
  // Initialize new node with time operator.
  result.binOpSet(timesPtr, TIMES, timesOp<MT,ST>, lhs, rhs);
  return(result);
}
//////////////////////////////////////////////////////////////////////
/**
 * @brief Functions to deal with opNum==MTIMESS
 * Callback function for differentiation involving operator*
 *
 * @tparam MT Matrix type.
 * @tparam ST Scalar type.
 *
 * @param result
 * @param current
 * @param left
 * @param right
 * @param node
 * @param transposeFlag
 * @param identityCurrentFlag
 * @param zeroResultFlag
*/
template <class MT, class ST>
void mtimessOp( boost::shared_ptr<MT> result, 
	      boost::shared_ptr<MT> current, 
	      boost::shared_ptr<MT> left, 
	      boost::shared_ptr<MT> right,
	      const MatrixMatrixFunc<MT,ST>* node, 
	      int& transposeFlag,
	      bool& identityCurrentFlag, 
	      bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( NULL != node && // check node type
	  NULL != node->leftChild &&
	  NULL != node->rightChild &&
	  TIMES == node->opNum &&
	  current.use_count()>=1 && // current, left and right must be present 
	  left.use_count()>=1 && 
	  right.use_count()>=1 );
  if (identityCurrentFlag) { // avoid superfluous multiplication
    transposeFlag = 0;
    if (TRANSPOSE == node->rightChild->opNum) {
      // if right is R^T then get R from it's left child
      (*left) = *(node->rightChild->leftChild->matrixPtr);
    } else {
      (*left) = *(node->rightChild->matrixPtr);
      transposeFlag |= 1; // set left transpose on
    }
    if (TRANSPOSE==node->leftChild->opNum) {
      // if right is R^T then get R from it's left child
      (*right) = *(node->leftChild->leftChild->matrixPtr);
    } else {
      (*right) = *(node->leftChild->matrixPtr);
      transposeFlag |= 2; // set right transpose on
    }
    identityCurrentFlag = false;
  } else {
    if (transposeFlag) {
      // use A^T*B^T = (B*A)^T to reduce the numbder of trans
      // Why is this comment here? Peder?
      //(*left) = transpose(*current) * transpose(node->rightChild->val);
      MatrixAdaptorType::multiply(*(node->rightChild->matrixPtr), 
                                  *current, 
                                  *left);

      // Why is this comment here? Peder?
      //(*right) = transpose(node->leftChild->val) * transpose(*current);
      MatrixAdaptorType::multiply(*current, 
                                  *(node->leftChild->matrixPtr), 
                                  *right);

      transposeFlag = 3;
    } else {
      MT rcTrans, lcTrans;
      // left = current * right->matrix^T
      MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), rcTrans);
      MatrixAdaptorType::multiply(*current, rcTrans, *left);

      // right = left->matrix^T * current
      MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), lcTrans);
      MatrixAdaptorType::multiply(lcTrans, *current, *right);

      transposeFlag = 0;
    }
  }
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
MatrixMatrixFunc<MT,ST> operator* (const MatrixMatrixFunc<MT,ST> &lhs, 
			                        	   const ScalarMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  /* TODO: Rows == Cols or Rows == Rows?
  assert( lhs.isConst || 
          rhs.isConst || 
	        (lhs.varNumRows==rhs.varNumRows && 
	        lhs.varNumCols==rhs.varNumCols));
  */
  // New node in computational tree.
  MatrixMatrixFunc<MT,ST> result;
  MT lhsTimesRhs;
  MatrixAdaptorType::multiply(*(lhs.matrixPtr), *(rhs.functionVal), lhsTimesRhs);
  boost::shared_ptr<MT> timesPtr(new MT((lhsTimesRhs)));
  // Initialize new node with time operator.
  result.binOpSet(timesPtr, MTIMESS, mtimessOp<MT,ST>, lhs, rhs);
  return(result);
}
//////////////////////////////////////////////////////////////////////////////

// Functions to deal with opNum = ELEWISE
// Callback function for differentiation involving elementwise product
// The elementwiseOp deal with transpose in a opposite way of timesOp.
template<class MT, class ST> 
void elementwiseOp ( boost::shared_ptr<MT>   result,
                     boost::shared_ptr<MT>   current,
                     boost::shared_ptr<MT>   left,
                     boost::shared_ptr<MT>   right,
                     const MatrixMatrixFunc<MT,ST>* node,
                     int& transposeFlag,
                     bool& identityCurrentFlag,
                     bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( NULL != node &&
          NULL != node->leftChild &&
          NULL != node->rightChild &&
          ELEWISE == node->opNum &&
          current.use_count() >= 1 &&
          left.use_count() >= 1 &&
          right.use_count()>= 1);
  if (identityCurrentFlag) {
    // If the current matrix is identymatrix, the left and right
    // matrix will be the element-wise product of the current matix 
    // and the leftChild's and rightChild's matrix respectively.
    // This is different from timesOp with ignore the identity matrix.
    transposeFlag = 0;
    if (TRANSPOSE == node->rightChild->opNum) {
      MatrixAdaptorType::elementwiseProduct(*(node->rightChild-> \
                                          leftChild->matrixPtr),
                                         *(current),
                                         *left);
    } else {
      MatrixAdaptorType::elementwiseProduct(*(node->rightChild->matrixPtr),
                                         *current, 
                                         *left);
    }
    if (TRANSPOSE == node->leftChild->opNum) {
      MatrixAdaptorType::elementwiseProduct(*(node->leftChild-> \
                                         leftChild->matrixPtr),
                                         *current,
                                         *right);
    } else {
      MatrixAdaptorType::elementwiseProduct(*(node->leftChild->matrixPtr),
                                         *current,
                                         *right);
    }
    identityCurrentFlag = 0;
  } else {
  if (transposeFlag) {
    MT lcTrans, rcTrans;
    MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), lcTrans);
    MatrixAdaptorType::elementwiseProduct(lcTrans, *(current), *right);
    MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), rcTrans);
    MatrixAdaptorType::elementwiseProduct(rcTrans, *(current), *left);
    transposeFlag = 3;
  } else {
    MatrixAdaptorType::elementwiseProduct(*(node->leftChild->matrixPtr), 
                                       *(current), 
                                       *right);
    MatrixAdaptorType::elementwiseProduct(*(node->rightChild->matrixPtr), 
                                       *(current), 
                                       *left);
    transposeFlag = 0;
  }
  }
}

// function for elementwise 
template <class MT, class ST> 
MatrixMatrixFunc<MT,ST> elementwiseProduct ( const MatrixMatrixFunc<MT,ST>& lhs,
                                          const MatrixMatrixFunc<MT,ST>& rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst ||
          rhs.isConst ||
          (lhs.varNumRows == rhs.varNumRows &&
           lhs.varNumCols == rhs.varNumCols));
  MatrixMatrixFunc<MT,ST> result;
  MT lcrcEwisePdt;
  MatrixAdaptorType::elementwiseProduct(*(lhs.matrixPtr), 
                                        *(rhs.matrixPtr), 
                                        lcrcEwisePdt);
  boost::shared_ptr<MT> elewisePtr(new MT(lcrcEwisePdt));
  result.binOpSet(elewisePtr, ELEWISE, elementwiseOp<MT,ST>, lhs, rhs);
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
 * @param left 
 * @param right
 */
template <class MT, class ST>
void transposeOp( boost::shared_ptr<MT> result, 
		  boost::shared_ptr<MT> current, 
		  boost::shared_ptr<MT> left, 
		  boost::shared_ptr<MT> right,
		  const MatrixMatrixFunc<MT,ST>* node, 
		  int& transposeFlag,
		  bool& identityCurrentFlag, 
		  bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

  assert( NULL != node && // check node type
	  NULL != node->leftChild &&
	  NULL == node->rightChild &&
	  TRANSPOSE== node->opNum &&
	  current.use_count()>=1 && // current, left and right must be present 
	  left.use_count()>=1 );
  // *left = *current;
  MatrixAdaptorType::copy ((*left), (*current));
  if (!identityCurrentFlag) {
    if (transposeFlag) {
      transposeFlag = 0;
    } else {
      transposeFlag = 3;  // transpose all living children
    }
  }
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
MatrixMatrixFunc<MT,ST> transpose (const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (TRANSPOSE!=lhs.opNum) {
    MT lhsTrans;
    MatrixAdaptorType::transpose(*(lhs.matrixPtr), lhsTrans);
    boost::shared_ptr<MT> transposePtr(new MT(lhsTrans)); 
    result.unaryOpSet(transposePtr, TRANSPOSE, transposeOp<MT,ST>, lhs);
  } else {
    assert(NULL!=lhs.leftChild);
    result.deepCopy(*lhs.leftChild);
  }
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
void diagOp ( boost::shared_ptr<MT>   result,
              boost::shared_ptr<MT>   current,
              boost::shared_ptr<MT>   left,
              boost::shared_ptr<MT>   right,
              const MatrixMatrixFunc<MT,ST>* node, // current node.
              int& transposeFlag,
              bool& identityCurrentFlag,
              bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( NULL != node && // check node type
          NULL != node->leftChild &&  // unary operator
          NULL == node->rightChild &&
          DIAG == node->opNum &&
          current.use_count() >= 1 &&
          left.use_count() >= 1 );
  MatrixAdaptorType::diag((*current), (*left));
}

/**
 * @brief Create a new node with operator Diag in computational tree.
 */

template <class MT, class ST> 
MatrixMatrixFunc<MT,ST> diag (const MatrixMatrixFunc<MT,ST>& lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (DIAG != lhs.opNum) {
    MT lhsDiag;
    MatrixAdaptorType::diag(*(lhs.matrixPtr), lhsDiag);
    boost::shared_ptr<MT> diagPtr(new MT(lhsDiag));
    result.unaryOpSet(diagPtr, DIAG, diagOp<MT,ST>, lhs);
  } else {
    assert(NULL != lhs.leftChild);
    result.deepCopy(*lhs.leftChild);
  }
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
 * @param left
 * @param right
 * @param node
 * @param transposeFlag
 * @param identityCurrentFlag
 * @param zeroResultFlag
 */
template <class MT, class ST>
void invOp( boost::shared_ptr<MT> result, 
	    boost::shared_ptr<MT> current, 
	    boost::shared_ptr<MT> left, 
	    boost::shared_ptr<MT> right,
	    const MatrixMatrixFunc<MT,ST>* node, 
	    int& transposeFlag,
	    bool& identityCurrentFlag, 
	    bool& zeroResultFlag) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

  assert( NULL != node && // check node type
	        NULL != node->leftChild &&
	        NULL == node->rightChild &&
	        INV == node->opNum &&
	        current.use_count()>=1 && // current, left and right must be present 
	        left.use_count()>=1 );
  boost::shared_ptr<MT> initMat= node->matrixPtr;
  if (!identityCurrentFlag) {
    if (transposeFlag) {
      MT lhsTimesRhs1, lhsTimesRhs2, cNeg;
      MatrixAdaptorType::multiply(*current, *initMat, lhsTimesRhs1);
      MatrixAdaptorType::multiply(*initMat, lhsTimesRhs1, lhsTimesRhs2);
      MatrixAdaptorType::negation(lhsTimesRhs2, cNeg);
      MatrixAdaptorType::copy((*left), cNeg);      

    } else {
      MT cTrans, lhsTimesRhs1, lhsTimesRhs2, cNeg;
      MatrixAdaptorType::transpose(*current, cTrans);
      MatrixAdaptorType::multiply(cTrans, *initMat, lhsTimesRhs1);
      MatrixAdaptorType::multiply(*initMat, lhsTimesRhs1, lhsTimesRhs2);
      MatrixAdaptorType::negation(lhsTimesRhs2, cNeg);
      MatrixAdaptorType::copy((*left), cNeg);
    }
  } else {
    MT lhsTimesRhs, lcNeg;
    MatrixAdaptorType::multiply(*initMat, *initMat, lhsTimesRhs);
    MatrixAdaptorType::negation(lhsTimesRhs, lcNeg);
    MatrixAdaptorType::copy((*left), lcNeg);    
  }
  transposeFlag = 3;
  identityCurrentFlag = false;
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
MatrixMatrixFunc<MT,ST> inv(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (INV!=lhs.opNum) {
    MT lhsInv;
    MatrixAdaptorType::inv(*lhs.matrixPtr, lhsInv);
    boost::shared_ptr<MT> invPtr(new MT(lhsInv));     
    // Initialize the node. 
    result.unaryOpSet( invPtr, INV, invOp<MT,ST>, lhs );
  } else {
    assert(NULL!=lhs.leftChild);
    result.deepCopy(*lhs.leftChild);
  }
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
ScalarMatrixFunc<MT,ST> trace(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  // matrix must be square in order to compute trace
  assert(MatrixAdaptorType::getNumRows(*lhs.matrixPtr) ==
         MatrixAdaptorType::getNumCols(*lhs.matrixPtr));  
  const int n = MatrixAdaptorType::getNumRows(*lhs.matrixPtr);  
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  *initPtr = MatrixAdaptorType::eye(n);
  *resPtr = MatrixAdaptorType::zeros(lhs.varNumRows, lhs.varNumCols); 
  bool zeroFlag = true;

  // TODO any need to deal with gradientVec? Not at this moment
  // Trigger gradientVec to calculate the derivative along the computational
  // tree reversely.
  lhs.gradientVec(initPtr, resPtr, false, true, zeroFlag);
  
  if (zeroFlag) {
    ScalarMatrixFunc<MT, ST> result( 
              MatrixAdaptorType::trace(*lhs.matrixPtr),
              lhs.varNumRows,
              lhs.varNumCols);
    return(result);
  } else {
    ScalarMatrixFunc<MT, ST> result(MatrixAdaptorType::trace(*lhs.matrixPtr), 
                                    *resPtr);				    
    return(result);
  }
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
ScalarMatrixFunc<MT,ST> logdet(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

  // matrix must be square in order to compute trace
  assert(MatrixAdaptorType::getNumRows(*(lhs.matrixPtr)) ==
         MatrixAdaptorType::getNumCols(*(lhs.matrixPtr))); 

  const int n = MatrixAdaptorType::getNumRows(*(lhs.matrixPtr));
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  *resPtr = MatrixAdaptorType::zeros(lhs.varNumRows, lhs.varNumCols);
  bool transposeFlag = true;

  if (TRANSPOSE==lhs.opNum) { // logdet(X^T) == logdet(X)
    // logdet for MMF
    return(logdet((*lhs.leftChild)));
  }
  if (INV == lhs.opNum) { // logdet(X^{-1)) == - logdet(X)
    // logdet for MMF 
    return(-logdet((*lhs.leftChild)));
  }
  // The starting point for logdet is a inverse matrix.
  MatrixAdaptorType::inv(*lhs.matrixPtr, *initPtr);
  bool zeroFlag = true;
  lhs.gradientVec(initPtr, resPtr, transposeFlag, false, zeroFlag);
  if (zeroFlag) { 
    return ScalarMatrixFunc<MT,ST> (MatrixAdaptorType::logdet(*lhs.matrixPtr),
                                    lhs.varNumRows, 
                                    lhs.varNumCols);
  } else {
    return ScalarMatrixFunc<MT,ST> (MatrixAdaptorType::logdet(*lhs.matrixPtr), 
				                           *resPtr);
  }
}

}  /** namespace AMD */

#endif
