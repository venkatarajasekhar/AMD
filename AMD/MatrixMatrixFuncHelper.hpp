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
template <class MT,class ST>
void constOp(boost::shared_ptr<MT> result, boost::shared_ptr<MT> current, 
	     boost::shared_ptr<MT> left, boost::shared_ptr<MT> right,
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
	  0 == node->varNumCols );
  // since the matrix is constant the derivative is zero, and we don't
  // need to do anything.
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

      // TODO: replace with adaptor
      
//      (*result) = transpose(*current);
      (*result) = MatrixAdaptorType::transposeAdaptor(*current);  
    } else {
      (*result) = (*current);
    }
  } else {
    if (transposeFlag) {
      // TODO: replace with adaptor
//      (*result) = (*result)+transpose(*current);
//
      (*result) = MatrixAdaptorType::add((*result), 
      MatrixAdaptorType::transposeAdaptor(*current));

    } else {
      // TODO template adaptor

//      (*result) = (*result)+(*current);
      (*result) = MatrixAdaptorType::add((*result), (*current));
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
				    const MatrixMatrixFunc<MT,ST> &rhs)
{
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst || rhs.isConst || 
	  (lhs.varNumRows==rhs.varNumRows && 
	   lhs.varNumCols==rhs.varNumCols ) );

  MatrixMatrixFunc<MT,ST> result;
  // TODO: replace + with template adaptor
//  boost::shared_ptr<MT> sumPtr( new MT((*lhs.matrixPtr) + (*rhs.matrixPtr)));

  boost::shared_ptr<MT> sumPtr( new MT(MatrixAdaptorType::add((*lhs.matrixPtr),
   (*rhs.matrixPtr))));

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
  (*left)  = (*current);
  // TODO: template adaptor
  
//  (*right) = -(*current);
  (*right) = MatrixAdaptorType::negation((*current));
  if (transposeFlag) {
    transposeFlag=3; // both left and right should inherit transpose
  }
}


template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator- (const MatrixMatrixFunc<MT,ST> &lhs, 
				    const MatrixMatrixFunc<MT,ST> &rhs)
{
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst || rhs.isConst || 
	  (lhs.varNumRows==rhs.varNumRows && 
	   lhs.varNumCols==rhs.varNumCols ) );

  MatrixMatrixFunc<MT,ST> result;
  //TODO replace - with template adaptor
//  boost::shared_ptr<MT> diffPtr( new MT((*lhs.matrixPtr) - (*rhs.matrixPtr)));

  boost::shared_ptr<MT> diffPtr(new MT(MatrixAdaptorType::minus(*lhs.matrixPtr,
   *rhs.matrixPtr)));

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
      //(*left) = transpose(*current) * transpose(node->rightChild->val);
      // TODO template adaptor
//      (*left) = *(node->rightChild->matrixPtr) * (*current);
      (*left) = MatrixAdaptorType::multiply(*(node->rightChild->matrixPtr),
                                            *current);      
      //(*right) = transpose(node->leftChild->val) * transpose(*current);
      // TODO template adaptor
//      (*right) = (*current) * (*(node->leftChild->matrixPtr));
      (*right) = MatrixAdaptorType::multiply(*current, 
                                             *(node->leftChild->matrixPtr));      
      transposeFlag = 3;
    } else {
      // TODO template adaptor
//      (*left) = (*current) * transpose(*(node->rightChild->matrixPtr));
      (*left) = MatrixAdaptorType::multiply(*current,
        MatrixAdaptorType::transposeAdaptor(*(node->rightChild->matrixPtr)));     
      // TODO template adaptor
//      (*right) = transpose( *(node->leftChild->matrixPtr) ) * (*current);
      (*right) = MatrixAdaptorType::multiply(
      MatrixAdaptorType::transposeAdaptor(*(node->leftChild->matrixPtr)),
      *current);      
      transposeFlag = 0;
    }
  }
}

template <class MT, class ST>
MatrixMatrixFunc<MT,ST> operator* (const MatrixMatrixFunc<MT,ST> &lhs, 
				    const MatrixMatrixFunc<MT,ST> &rhs)
{
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  assert( lhs.isConst || rhs.isConst || 
	  (lhs.varNumRows==rhs.varNumRows && 
	   lhs.varNumCols==rhs.varNumCols ) );

  MatrixMatrixFunc<MT,ST> result;
  // TODO: replace * with Template Adaptor
//  boost::shared_ptr<MT> timesPtr( new MT((*lhs.matrixPtr) * (*rhs.matrixPtr));

  boost::shared_ptr<MT> timesPtr(new MT((
  MatrixAdaptorType::multiply(*lhs.matrixPtr, *rhs.matrixPtr))));

  result.binOpSet( timesPtr, TIMES, timesOp<MT,ST>, lhs, rhs );
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
  assert( NULL != node && // check node type
	  NULL != node->leftChild &&
	  NULL == node->rightChild &&
	  TRANSPOSE== node->opNum &&
	  current.use_count()>=1 && // current, left and right must be present 
	  left.use_count()>=1 );
  (*left) = (*current);
  if (!identityCurrentFlag) {
    if (transposeFlag) {
      transposeFlag = 0;
    } else {
      transposeFlag = 3;  // transpose all living children
    }
  }
}


template <class MT, class ST>
MatrixMatrixFunc<MT,ST> transpose (const MatrixMatrixFunc<MT,ST> &lhs)
{
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (TRANSPOSE!=lhs.opNum) {
    // TODO replace transpose with template adaptor Be careful there are
    // two transpose. One for MMF and one for Symbolic MM
//    boost::shared_ptr<MT> transposePtr( new MT(transpose(*lhs.matrixPtr)) );
    boost::shared_ptr<MT> transposePtr(
              new MT(MatrixAdaptorType::transposeAdaptor(*lhs.matrixPtr))
              ); 
    result.unaryOpSet( transposePtr, TRANSPOSE, transposeOp<MT,ST>, lhs );
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
      // TODO: replace * with template adaptor
//      (*left) = - (*tmp) * (*current) * (*tmp);
    (*left) = MatrixAdaptorType::negation(
                MatrixAdaptorType::multiply(*tmp, 
                  MatrixAdaptorType::multiply(*current, *tmp)));      

    } else {
      // TODO: replace * with template adaptor
//      (*left) = - (*tmp) * transpose(*current) * (*tmp);
      (*left) = MatrixAdaptorType::negation(
                  MatrixAdaptorType::multiply(*tmp,
                    MatrixAdaptorType::multiply(
                      MatrixAdaptorType::transposeAdaptor(*current), *tmp)));      
    }
  } else {
    // TODO: replace * with template adaptor
     
//    (*left) = - (*tmp) * (*tmp);
    (*left) = MatrixAdaptorType::negation(
                MatrixAdaptorType::multiply(*tmp, *tmp));    
  }
  transposeFlag = 3;
  identityCurrentFlag = false;
}


template <class MT, class ST>
MatrixMatrixFunc<MT,ST> inv(const MatrixMatrixFunc<MT,ST> &lhs)
{
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  MatrixMatrixFunc<MT,ST> result;
  if (INV!=lhs.opNum) {
    // TODO: replace inv with template adaptor. Two different
    // inv here
//    boost::shared_ptr<MT> invPtr( new MT(inv(*lhs.matrixPtr)) );

    boost::shared_ptr<MT> invPtr(new MT(
      MatrixAdaptorType::invAdaptor(*lhs.matrixPtr)));      
      
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
  // TODO replace getNumCols and getNumRows with template adaptor
//  assert( lhs.matrixPtr->getNumRows() == lhs.matrixPtr->getNumCols() );
  
  assert(MatrixAdaptorType::getNumRows(*lhs.matrixPtr) 
        == MatrixAdaptorType::getNumCols(*lhs.matrixPtr));  

//  int n = lhs.matrixPtr->getNumRows();
  int n = MatrixAdaptorType::getNumRows(*lhs.matrixPtr);  
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  // TODO replace eye 
//  (*initPtr) = resPtr->eye(n);
  (*initPtr) = MatrixAdaptorType::eyeAdaptor(*resPtr, n);  
  // TODO replace zeros
//  (*resPtr) = resPtr->zeros(lhs.varNumRows,lhs.varNumCols);
  (*resPtr) = MatrixAdaptorType::zerosAdaptor(*resPtr, lhs.varNumRows, lhs.varNumCols);
  bool zeroFlag = true;

  // TODO any need to deal with gradientVec? Not at this moment
  lhs.gradientVec(initPtr,resPtr,false,true,zeroFlag);

  if (zeroFlag) {
    // TODO template adaptor
//    ScalarMatrixFunc<MT,ST> result( trace(*(lhs.matrixPtr)),
//				    lhs.varNumRows,
//				    lhs.varNumCols );
				    
    ScalarMatrixFunc<MT, ST> result( 
              MatrixAdaptorType::traceAdaptor(*(lhs.matrixPtr)),
              lhs.varNumRows,
              lhs.varNumCols);
                  				    
    // pass on knowledge that function is constant
    return(result);
  } else {
    // TODO template adaptor
     
//    ScalarMatrixFunc<MT,ST> result( trace(*(lhs.matrixPtr)),
//				    *resPtr );
    ScalarMatrixFunc<MT, ST> result (
              MatrixAdaptorType::traceAdaptor(*(lhs.matrixPtr)), *resPtr);				    
    return(result);
  }
}

template <class MT, class ST>
ScalarMatrixFunc<MT,ST> logdet(const MatrixMatrixFunc<MT,ST> &lhs) {

  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  // matrix must be square in order to compute trace
  // TODO 
//  assert( lhs.matrixPtr->getNumRows() == lhs.matrixPtr->getNumCols() );
  
  assert(MatrixAdaptorType::getNumRows(*(lhs.matrixPtr)) 
        == MatrixAdaptorType::getNumCols(*(lhs.matrixPtr))); 

//  int n = lhs.matrixPtr->getNumRows();
  int n = MatrixAdaptorType::getNumRows(*(lhs.matrixPtr));
  boost::shared_ptr<MT> initPtr(new MT);
  boost::shared_ptr<MT> resPtr(new MT);
  // TODO template adaptor
//  (*resPtr) = resPtr->zeros(lhs.varNumRows,lhs.varNumCols);
  (*resPtr) = MatrixAdaptorType::zerosAdaptor(*(resPtr), 
              lhs.varNumRows, lhs.varNumCols); 
  bool transposeFlag = true;

  if (TRANSPOSE==lhs.opNum) { // logdet(X^T) == logdet(X)
    // logdet for MMF
    return(logdet((*lhs.leftChild)));
  }
  if (INV == lhs.opNum) { // logdet(X^{-1)) == - logdet(X)
    // logdet for MMF 
    return(-logdet((*lhs.leftChild)));
  }
  // TODO template adaptor

//  *initPtr = inv(*(lhs.matrixPtr));
  *initPtr = MatrixAdaptorType::invAdaptor(*(lhs.matrixPtr));  
  bool zeroFlag = true;
  lhs.gradientVec(initPtr,resPtr,transposeFlag,false,zeroFlag);
  if (zeroFlag) { 
    // TODO template adaptor
//    ScalarMatrixFunc<MT,ST> result( logdet( *(lhs.matrixPtr) ),
//				    lhs.varNumRows,
//    			  lhs.varNumCols );
      ScalarMatrixFunc<MT,ST> result(
                MatrixAdaptorType::logdetAdaptor(*(lhs.matrixPtr)),
                lhs.varNumRows, 
                lhs.varNumCols);
    // pass on knowledge that function is constant
    return(result);
  } else {
    // TODO template adaptor
    ScalarMatrixFunc<MT,ST> result( 
                MatrixAdaptorType::logdetAdaptor( *(lhs.matrixPtr) ),
				        *resPtr );
    return(result);
  }
}

}  /** namespace AMD */

#endif
