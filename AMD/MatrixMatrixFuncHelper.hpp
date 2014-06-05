#ifndef MatrixMatrixFunHelper_H
#define MatrixMatrixFuncHelper_H

#include <string>
#include <cstdio>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include "utility.hpp"
#include "ScalarMatrixFunc.hpp"
#include "MatrixAdaptor.hpp"

namespace AMD {

enum OpType { NONE, CONST, VAR, PLUS, MINUS, TIMES, TRANSPOSE, INV};
std::string opName[] = 
  { "none", "const", "var", "+", "-", "*", "transpose", "inv" };

// forward declaration
template <class MT, class ST> class MatrixMatrixFunc;

/// Callback function for differentiation involving constant matrices
// since the matrix is constant the derivative is zero, and we don't
// need to do anything.
template <class MT,class ST>
void constOp(boost::shared_ptr<MT> result, 
             boost::shared_ptr<MT> current, 
	           boost::shared_ptr<MT> left, 
             boost::shared_ptr<MT> right,
	           const MatrixMatrixFunc<MT,ST>* node, 
	           int& transposeFlag,
	           bool& identityCurrentFlag, 
	           bool& zeroResultFlag) {
    assert( NULL != node && // check node type
	          NULL == node->leftChild &&
	          NULL == node->rightChild &&
	          node->isConst &&
	          CONST == node->opNum && 
	          0 == node->varNumRows &&
	          0 == node->varNumCols);
}

/// Callback function for differentiation involving a variable matrix
/// on the leaf node.
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
	  NULL == node->leftChild &&
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
      MT tmp;
      MatrixAdaptorType::transpose(*current, tmp);
      MatrixAdaptorType::add(*result, tmp, *result);
    } else {
      MatrixAdaptorType::add((*result), (*current), (*result));
    }
  }
}

// Functions to deal with opNum==PLUS
/// Callback function for differentiation involving operator+
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
  (*left)  = (*current);
  (*right) = (*current);
  if (transposeFlag) {
    transposeFlag=3; // both left and right should inherit transpose
  }
}

template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator+ (const MatrixMatrixFunc<MT,ST> &lhs, 
				                           const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert(lhs.isConst || 
         rhs.isConst || 
	       (lhs.varNumRows==rhs.varNumRows && 
	       lhs.varNumCols==rhs.varNumCols));

  MatrixMatrixFunc<MT,ST> result;
  MT tmp;
  MatrixAdaptorType::add((*lhs.matrixPtr),(*rhs.matrixPtr), tmp);
  boost::shared_ptr<MT> sumPtr(new MT(tmp));
  result.binOpSet( sumPtr, PLUS, plusOp<MT,ST>, lhs, rhs );
  return(result);
}

// Functions to deal with opNum==MINUS
/// Callback function for differentiation involving operator-
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

template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator- (const MatrixMatrixFunc<MT,ST> &lhs, 
                        				   const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert(lhs.isConst || 
         rhs.isConst || 
	       (lhs.varNumRows==rhs.varNumRows && 
	       lhs.varNumCols==rhs.varNumCols));

  MatrixMatrixFunc<MT,ST> result;
  MT tmp;
  MatrixAdaptorType::minus(*(lhs.matrixPtr), *(rhs.matrixPtr), tmp);
  boost::shared_ptr<MT> diffPtr
   (new MT(tmp));
  result.binOpSet( diffPtr, MINUS, minusOp<MT,ST>, lhs, rhs );
  return(result);
}


// Functions to deal with opNum==TIMES
/// Callback function for differentiation involving operator*
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
      MT tmp0, tmp1, tmp2, tmp3;
      MatrixAdaptorType::multiply(*(node->rightChild->matrixPtr), (*current), tmp0);

      MatrixAdaptorType::copy((*left), tmp0);      

      // Why is this comment here? Peder?
      //(*right) = transpose(node->leftChild->val) * transpose(*current);
      MatrixAdaptorType::multiply((*current), *(node->leftChild->matrixPtr), tmp1);
      MatrixAdaptorType::copy((*right), tmp1);      
      transposeFlag = 3;
    } else {
      MT tmp0, tmp1, tmp2, tmp3;
      MatrixAdaptorType::transpose(*(node->rightChild->matrixPtr), tmp0);
      MatrixAdaptorType::multiply((*current), tmp0, tmp1);
      MatrixAdaptorType::copy ((*left), tmp1);

      MatrixAdaptorType::transpose(*(node->leftChild->matrixPtr), tmp2);
      MatrixAdaptorType::multiply(tmp2, *current, tmp3);
      MatrixAdaptorType::copy((*right), tmp3);

      transposeFlag = 0;
    }
  }
}

template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator* (const MatrixMatrixFunc<MT,ST> &lhs, 
			                        	   const MatrixMatrixFunc<MT,ST> &rhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst || 
          rhs.isConst || 
	        (lhs.varNumRows==rhs.varNumRows && 
	        lhs.varNumCols==rhs.varNumCols));

  MatrixMatrixFunc<MT,ST> result;
  MT tmp;
  MatrixAdaptorType::multiply(*(lhs.matrixPtr), *(rhs.matrixPtr), tmp);
  boost::shared_ptr<MT> timesPtr(new MT((tmp)));
  result.binOpSet(timesPtr, TIMES, timesOp<MT,ST>, lhs, rhs);
  return(result);
}


// Functions to deal with opNum==TRANSPOSE
/// Callback function for differentiation involving the matrix transpose
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
  MatrixAdaptorType::copy ((*left), (*current));
  if (!identityCurrentFlag) {
    if (transposeFlag) {
      transposeFlag = 0;
    } else {
      transposeFlag = 3;  // transpose all living children
    }
  }
}


template <class MT, class ST>
MatrixMatrixFunc<MT,ST> transpose (const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (TRANSPOSE!=lhs.opNum) {
    MT tmp;
    MatrixAdaptorType::transpose(*(lhs.matrixPtr), tmp);
    boost::shared_ptr<MT> transposePtr(new MT(tmp)); 
    result.unaryOpSet(transposePtr, TRANSPOSE, transposeOp<MT,ST>, lhs);
  } else {
    assert(NULL!=lhs.leftChild);
    result.deepCopy(*lhs.leftChild);
  }
  return(result);
}



// Functions to deal with opNum==INV
/// Callback function for differentiation involving the matrix inverse
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
  boost::shared_ptr<MT> tmp = node->matrixPtr;
  if (!identityCurrentFlag) {
    if (transposeFlag) {
      MT tmp0, tmp1, tmp2;
      MatrixAdaptorType::multiply(*current, *tmp, tmp0);
      MatrixAdaptorType::multiply(*tmp, tmp0, tmp1);
      MatrixAdaptorType::negation(tmp1, tmp2);
      MatrixAdaptorType::copy((*left), tmp2);      

    } else {
      MT tmp0, tmp1, tmp2, tmp3;
      MatrixAdaptorType::transpose(*current, tmp0);
      MatrixAdaptorType::multiply(tmp0, *tmp, tmp1);
      MatrixAdaptorType::multiply(*tmp, tmp1, tmp2);
      MatrixAdaptorType::negation(tmp2, tmp3);
      MatrixAdaptorType::copy((*left), tmp3);
    }
  } else {
    MT tmp0, tmp1;
    MatrixAdaptorType::multiply(*tmp, *tmp, tmp0);
    MatrixAdaptorType::negation(tmp0, tmp1);
    MatrixAdaptorType::copy((*left), tmp1);    
  }
  transposeFlag = 3;
  identityCurrentFlag = false;
}

template <class MT, class ST>
MatrixMatrixFunc<MT,ST> inv(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (INV!=lhs.opNum) {
    MT tmp0;
    MatrixAdaptorType::inv(*lhs.matrixPtr, tmp0);
    boost::shared_ptr<MT> invPtr(new MT(tmp0));      
    result.unaryOpSet( invPtr, INV, invOp<MT,ST>, lhs );
  } else {
    assert(NULL!=lhs.leftChild);
    result.deepCopy(*lhs.leftChild);
  }
  return(result);
}

template <class MT, class ST>
ScalarMatrixFunc<MT,ST> trace(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  // matrix must be square in order to compute trace
  assert(MatrixAdaptorType::getNumRows(*lhs.matrixPtr) ==
         MatrixAdaptorType::getNumCols(*lhs.matrixPtr));  
  const int n = MatrixAdaptorType::getNumRows(*lhs.matrixPtr);  
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  MT tmp0, tmp1;
  MatrixAdaptorType::eye(*resPtr, n, tmp0);
  MatrixAdaptorType::copy((*initPtr), tmp0); 
  MatrixAdaptorType::zeros(*resPtr, lhs.varNumRows, lhs.varNumCols, tmp1); 
  MatrixAdaptorType::copy((*resPtr), tmp1);
  bool zeroFlag = true;

  // TODO any need to deal with gradientVec? Not at this moment
  lhs.gradientVec(initPtr, resPtr, false, true, zeroFlag);
  ST tmp2;
  if (zeroFlag) {
    MatrixAdaptorType::trace(*lhs.matrixPtr, tmp2);
    ScalarMatrixFunc<MT, ST> result( 
              tmp2,
              lhs.varNumRows,
              lhs.varNumCols);
    return(result);
  } else {
    MatrixAdaptorType::trace(*lhs.matrixPtr, tmp2);
    ScalarMatrixFunc<MT, ST> result (tmp2, *resPtr);				    
    return(result);
  }
}

template <class MT, class ST>
ScalarMatrixFunc<MT,ST> logdet(const MatrixMatrixFunc<MT,ST> &lhs) {
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

  // matrix must be square in order to compute trace
  assert(MatrixAdaptorType::getNumRows(*(lhs.matrixPtr)) ==
         MatrixAdaptorType::getNumCols(*(lhs.matrixPtr))); 

  const int n = MatrixAdaptorType::getNumRows(*(lhs.matrixPtr));
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  MT tmp0, tmp1;
  ST tmp2;
  MatrixAdaptorType::zeros(*(resPtr), lhs.varNumRows, lhs.varNumCols, tmp0);
  MatrixAdaptorType::copy ((*resPtr), tmp0); 
  bool transposeFlag = true;

  if (TRANSPOSE==lhs.opNum) { // logdet(X^T) == logdet(X)
    // logdet for MMF
    return(logdet((*lhs.leftChild)));
  }
  if (INV == lhs.opNum) { // logdet(X^{-1)) == - logdet(X)
    // logdet for MMF 
    return(-logdet((*lhs.leftChild)));
  }
  MatrixAdaptorType::inv(*lhs.matrixPtr, tmp1);
  MatrixAdaptorType::copy(*initPtr, tmp1);  
  bool zeroFlag = true;
  lhs.gradientVec(initPtr, resPtr, transposeFlag, false, zeroFlag);
  if (zeroFlag) { 
    MatrixAdaptorType::logdet(*lhs.matrixPtr, tmp2);
    ScalarMatrixFunc<MT,ST> result(
                tmp2,
                lhs.varNumRows, 
                lhs.varNumCols);
    // pass on knowledge that function is constant
    return(result);
  } else {
    // TODO template adaptor
    MatrixAdaptorType::logdet(*lhs.matrixPtr, tmp2);
    ScalarMatrixFunc<MT,ST> result( 
                tmp2,
				        *resPtr);
    return(result);
  }
}

}  /** namespace AMD */

#endif
