#ifndef MatrixMatrixFunc_H
#define MatrixMatrixFunc_H

#include <string>
#include <cstdio>
#include <assert.h>
#include "boost/shared_ptr.hpp"
#include "utility.hpp"
#include "ScalarMatrixFunc.hpp"
#include "MatrixAdaptor.hpp"

// definitions of enum, operators and callBackFunctions
#include "MatrixMatrixFuncHelper.hpp" 

namespace AMD {

template <class MT, class ST>
class MatrixMatrixFunc {

public:
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  typedef void (*CallBackFuncType)(boost::shared_ptr<MT>, 
				                           boost::shared_ptr<MT>,
				                           boost::shared_ptr<MT>, 
				                           boost::shared_ptr<MT>,
				                           const MatrixMatrixFunc<MT,ST>*,
				                           int&, bool&, bool&);

  boost::shared_ptr<MT> matrixPtr; /**< Once recorded the matrix should never 
                                        be changed - so a pointer is safe */
  CallBackFuncType callBackFunc; /**< Used to compute derivative after 
                                      evaluation tree is recorded */
  OpType opNum; /**< enum for the operator used at this node in the tree */
  bool isConst; /**< is this a variable or a constant function */
  int varNumRows; /**< number of rows in matrix variable */
  int varNumCols; /**< number of cols in matrix variable */
  MatrixMatrixFunc* leftChild; /**< optional left child */
  MatrixMatrixFunc* rightChild; /**< optional right child */

  /**
   * This is an empty constructor that initializes all values to defaults.
   */ 
  MatrixMatrixFunc() : matrixPtr(), 
                       callBackFunc(NULL), 
                       opNum(NONE), 
			                 isConst(true), 
                       varNumRows(0), 
                       varNumCols(0), 
			                 leftChild(NULL), 
                       rightChild(NULL) {}

  /**
   * Makes an expensive copy of matrix -- avoid this constructor
   * if your matrices are large.
   */ 
  MatrixMatrixFunc(MT matrix, bool isConst=true) : matrixPtr(), 
                                                   callBackFunc(NULL), 
                                                   opNum(NONE),
                                                   isConst(isConst),
                                                   varNumRows(0), 
                                                   varNumCols(0), 
                                                   leftChild(NULL), 
                                                   rightChild(NULL) {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    boost::shared_ptr<MT> copy (new MT(matrix));
    // TODO adaptor.copy
    matrixPtr = copy;
//    MatrixAdaptorType::copyAdaptor(*matrixPtr, *copy);
    setVariableType (isConst);
  }



  MatrixMatrixFunc(boost::shared_ptr<MT> matrixPtr, 
                   bool isConst=true) : matrixPtr(matrixPtr),
                                        callBackFunc(NULL), 
                                        opNum(NONE),
                                        isConst(isConst),
                                        varNumRows(0), 
                                        varNumCols(0), 
                                        leftChild(NULL), 
                                        rightChild(NULL) {
    setVariableType (isConst);
  }

  /**
   * Do what's necessary to set up a variable or a constant
   */ 
  void setVariableType( bool isVariable ) {
    // constant and variable matrices must be leaf nodes
    assert( NULL == leftChild && NULL == rightChild );

    if (isConst) {
      callBackFunc = constOp<MT,ST>;
      opNum = CONST;
      varNumRows = 0;
      varNumCols = 0;
    } else {
      /**
       * TODO: Wrap getNumCols() and getNumRows() in an adaptor class 
       */ 
      //varNumRows = MatrixAdaptorType::getNumRows(*(matrixPtr));
      varNumRows = MatrixAdaptorType::getNumRows(*(matrixPtr));
      varNumCols = MatrixAdaptorType::getNumCols(*(matrixPtr));
      callBackFunc = varOp<MT,ST>;
      opNum = VAR;
    }
  }

  /**
   * Reset the entire MMFunc.
   */
  void reset () {
    callBackFunc = NULL;
    opNum = NONE;
    varNumRows = 0;
    varNumCols = 0;
    if (NULL!=leftChild) delete leftChild;
    if (NULL!=rightChild) delete rightChild;
  }

  /**
   * Delete the left and the right children recursively
   */
  ~MatrixMatrixFunc() { reset(); }

  /**
   * @function
   * Create a shallow copy of the current MMFunc 
   * @param[in] other The MMFunc that we want to copy from.
   * @return Nothing
   */ 
  void shallowCopy(const MatrixMatrixFunc &other) { 
    matrixPtr = other.matrixPtr;
    opNum = other.opNum;
    isConst = other.isConst;
    callBackFunc = other.callBackFunc;
    varNumRows = other.varNumRows;
    varNumCols = other.varNumCols;
    leftChild = NULL;
    rightChild = NULL;
  }

  /**
   * @function
   * Create a deep copy of the current MMFunc 
   * @param[in] other The MMFunc that we want to copy from.
   * @return Nothing
   */ 
  void deepCopy(const MatrixMatrixFunc &other) { 
    /* reset the current object */
    reset();
    
    /* do a shallow copy first */
    shallowCopy(other);

    /* if there are left or right children, recursively copy them as well */
    if (NULL != other.leftChild) {
      leftChild = new MatrixMatrixFunc<MT,ST>;
      leftChild->deepCopy(*(other.leftChild));
    }

    if (NULL != other.rightChild) {
      rightChild = new MatrixMatrixFunc<MT,ST>;
      rightChild->deepCopy(*(other.rightChild));
    }
  }

  void binOpSet(boost::shared_ptr<MT> resultPtr,
		            OpType _opNum,
		            CallBackFuncType cbf,
		            const MatrixMatrixFunc<MT,ST> &lhs, 
		            const MatrixMatrixFunc<MT,ST> &rhs) {
    unaryOpSet(resultPtr,_opNum,cbf,lhs);
    rightChild = new MatrixMatrixFunc<MT,ST>;
    rightChild->deepCopy(rhs);

    isConst = lhs.isConst && rhs.isConst;
    varNumRows = lhs.varNumRows | rhs.varNumRows;  // should be 0 or a size
    varNumCols = lhs.varNumCols | rhs.varNumCols;
  }

  void unaryOpSet(boost::shared_ptr<MT> resultPtr,
		              OpType _opNum,
		              CallBackFuncType cbf,
		              const MatrixMatrixFunc<MT,ST> &lhs) {
    varNumRows = lhs.varNumRows;  // should be 0 or a size
    varNumCols = lhs.varNumCols;
    matrixPtr = resultPtr;
    opNum = _opNum;
    isConst = lhs.isConst;
    callBackFunc = cbf;

    leftChild = new MatrixMatrixFunc<MT,ST>;
    leftChild->deepCopy(lhs);
  }

  void print(FILE *fp) const {
    matrixPtr->print();
    if (NULL==leftChild && NULL==rightChild) {
      fprintf(fp,":%s",opName[opNum].c_str());
    } else {
      fprintf(fp,":%s(",opName[opNum].c_str());
      if (NULL!=leftChild) {
	      leftChild->print(fp);
      } 
      if (NULL!=rightChild) {
	      fprintf(fp,",");
	      rightChild->print(fp);
      } 
      fprintf(fp,")");
    }
  }

  void println(FILE *fp) const {
    print(fp);
    fprintf(fp,"\n");
  }

  void print() const {
    print(stdout);
  }

  void println() const {
    println(stdout);
  }

  int numRows() const { return varNumRows; }

  int numCols() const { return varNumCols; }

  virtual MT value() const { return(*matrixPtr); }

  // initial and result must point to existing MatrixTypes
  void gradientVec(boost::shared_ptr<MT> initial, 
		               boost::shared_ptr<MT> result, 
		               int transposeFlag, 
		               bool identityInitialFlag,
		               bool& zeroResultFlag ) const {

    /** Make these into individual asserts */
    // TODO: replate getNumRows with traits
    
    assert(initial.use_count()>=1 && 
	         result.use_count()>=1  &&
	         isConst || 
           (MatrixAdaptor::getNumRows(*(result)) == varNumRows &&
			      MatrixAdaptor::getNumCols(*(result)) == varNumCols));

    // If the function is constant then the derivative is zero -- so
    // we don't need to do any computation
    if (!isConst) { 
      boost::shared_ptr<MT> leftPtr(new MT);
      if (NULL != leftChild) *leftPtr  = *(leftChild->matrixPtr);
      boost::shared_ptr<MT> rightPtr(new MT);
      if (NULL != rightChild) *rightPtr  = *(rightChild->matrixPtr);

      callBackFunc(result, 
                   initial, 
                   leftPtr, 
                   rightPtr, 
                   this,
		               transposeFlag,
                   identityInitialFlag, 
                   zeroResultFlag);

      if (NULL!=leftChild) {
	      int leftFlag = transposeFlag & 1;
	      leftChild->gradientVec(leftPtr, 
				                       result, 
				                       leftFlag, 
				                       identityInitialFlag,
				                       zeroResultFlag);
      }

      if (NULL!=rightChild) {
	      int rightFlag = transposeFlag & 2;
	      rightChild->gradientVec(rightPtr, 
				                        result, 
				                        rightFlag, 
				                        identityInitialFlag,
				                        zeroResultFlag);
      }
    } // end if (!isConst) 
  }
};
}  /** namespace AMD */

#endif
