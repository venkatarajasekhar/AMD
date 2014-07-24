#ifndef MatrixMatrixFunc_H
#define MatrixMatrixFunc_H

/**
 * @file MatrixMatrixFunc.hpp
 *
 * @author Peder Olsen, Anju Kambadur
 *
 * @brief This file defines a class to present Matrix-Matrix function. This 
 * class act as nodes in  the computational tree. The computation of matrix 
 * derivatives is by traversing the computation tree in the reverse mode.
 */

#include <iostream>
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

// forward declaration
template <class MT, class ST> class ScalarMatrixFunc;
/**
 * @brief MatrixMatrixFunc class. The computational tree consists of 
 * nodes of MatrixMatrixFunc type. In reverse mode differentiation, 
 * the computation of derivative is passing down to the leaf 
 * nodes of the tree and the derivate is the sum of matrices of all
 * leaf nodes.
 */
template <class MT, class ST>
class MatrixMatrixFunc {

public:
  typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
  typedef MatrixMatrixFunc<MT, ST> MatrixMatrixFunctionType;
  typedef MatrixMatrixFunctionType MMFT;
  typedef void (*CallBackFuncType)(boost::shared_ptr<MT>, 
				                           boost::shared_ptr<MT>,
				                           boost::shared_ptr<MT>, 
				                           boost::shared_ptr<MT>,
                                   boost::shared_ptr<MMFT>,   /**< current MMFT. */
                                   boost::shared_ptr<MMFT>,   /**< left MMFT. */
                                   boost::shared_ptr<MMFT>,   /**< right MMFT. */
                                   boost::shared_ptr<MMFT>,   /**< result MMFT. */
				                           const MatrixMatrixFunc<MT,ST>*,
				                           int&, bool&, bool&);

  boost::shared_ptr<MT> matrixPtr; /**< Once recorded the matrix should never 
                                        be changed - so a pointer is safe */
//  MT matrixVal;    /**< The matrix result for this node. It should not be changed. */
  CallBackFuncType callBackFunc; /**< Used to compute derivative after 
                                      evaluation tree is recorded */
  OpType opNum; /**< enum for the operator used at this node in the tree */
  bool isConst; /**< is this a variable or a constant function */
  int varNumRows; /**< number of rows in matrix variable */
  int varNumCols; /**< number of cols in matrix variable */
  MatrixMatrixFunc* leftChild; /**< optional left child */
  MatrixMatrixFunc* rightChild; /**< optional right child */
  ScalarMatrixFunc<MT, ST>* scalarChild; /**< scalar func * matrix func. */

  // TODO think about an extra scalar value here
  /**
   * @brief This is an empty constructor that initializes all values to
   * defaults.  Create a constant function.
   */ 
  MatrixMatrixFunc() : matrixPtr(), 
                       callBackFunc(NULL), 
                       opNum(NONE), 
			                 isConst(true), 
                       varNumRows(0), 
                       varNumCols(0), 
			                 leftChild(NULL), 
                       rightChild(NULL),
                       scalarChild(NULL)
  //                     matrixVal()
                       { }

  /**
   * @brief Makes an expensive copy of matrix -- avoid this constructor
   * if your matrices are large.
   */ 
  MatrixMatrixFunc(MT matrix, bool isConst=true) : matrixPtr(), 
                                                   callBackFunc(NULL), 
                                                   opNum(NONE),
                                                   isConst(isConst),
                                                   varNumRows(0), 
                                                   varNumCols(0), 
                                                   leftChild(NULL), 
                                                   rightChild(NULL),
                                                   scalarChild(NULL)
                        //                           matrixVal() 
                                                   {
    typedef MatrixAdaptor_t<MT> MatrixAdaptorType;
    // Makes an deep-copy of the matrix.
    boost::shared_ptr<MT> copy (new MT(matrix));
//    matrixVal = matrix;
    matrixPtr = copy;
    setVariableType (isConst);
  }

  /**
   * @brief Constructor with a MT type variable.
   */
    

  MatrixMatrixFunc(boost::shared_ptr<MT> matrixPtr, 
                   bool isConst=true) : matrixPtr(matrixPtr),
                                        callBackFunc(NULL), 
                                        opNum(NONE),
                                        isConst(isConst),
                                        varNumRows(0), 
                                        varNumCols(0), 
                                        leftChild(NULL), 
                                        rightChild(NULL),
                                        scalarChild(NULL)
          //                              matrixVal()
                                         {
//    matrixVal = *matrixPtr;
    setVariableType (isConst);
  }

  /**
   * Do what's necessary to set up a variable or a constant
   */ 
  /**
   * @brief Set the value of member variables of leaf nodes. 
   * If the matrix of leaf node is constant matrix, the callBackFunc 
   * is constOp, the varNumRows and varNumCols are set to zero and the 
   * opNum is set to CONST. Else if the matrix of the leaf node is 
   * variable matrix, the varNumRows and varNumCols are set to the size
   * of the matrix associated to this node.
   *
   * @param[in] isVariable Boolean value indicates whether the implicit
   * matrix is a variable. If so, initialize the object as a variable.
   * Otherwise initialize as a constant.
   */
  void setVariableType( bool isVariable ) {
    // constant and variable matrices must be leaf nodes
    assert( NULL == leftChild && NULL == rightChild );
    // If is constant, call the callbackfunction for constant.
    if (isConst) {
      callBackFunc = constOp<MT,ST>;
      opNum = CONST;
      varNumRows = 0;
      varNumCols = 0;
    } else {
      varNumRows = MatrixAdaptorType::getNumRows(*(matrixPtr));
      varNumCols = MatrixAdaptorType::getNumCols(*(matrixPtr));
      callBackFunc = varOp<MT,ST>;
      opNum = VAR;
    }
  }

  /**
   * @brief Reset the entire computational tree.
   */
  void reset () {
    callBackFunc = NULL;
    opNum = NONE;
    varNumRows = 0;
    varNumCols = 0;
    // Reset its left & right child recursively.
    if (NULL!=leftChild) delete leftChild;
    if (NULL!=rightChild) delete rightChild;
    if (NULL != scalarChild) delete scalarChild;
//    if (NULL != derivativeMMFunc) delete derivativeMMFunc;
  }

  /**
   * @brief Destructor. Delete the left and the right children recursively
   */
  ~MatrixMatrixFunc() { reset(); }

  /**
   * @function
   * Create a shallow copy of the current MatrixMatrixFunc node. 
   * @param[in] other The MMFunc that we want to copy from.
   * @return Nothing
   */ 
  void shallowCopy(const MatrixMatrixFunc &other) { 
//    matrixVal = other.matrixVal;
    matrixPtr = other.matrixPtr;
    opNum = other.opNum;
    isConst = other.isConst;
    callBackFunc = other.callBackFunc;
    varNumRows = other.varNumRows;
    varNumCols = other.varNumCols;
    leftChild = NULL;
    rightChild = NULL;
    scalarChild = NULL;
  }

  /**
   * @function
   * Create a deep copy of the current MatrixMatrixFunc node. This function
   * will create a copy of this node as well as its children nodes.
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
    if (NULL != other.scalarChild) {
      scalarChild = new ScalarMatrixFunc<MT,ST>;
      *scalarChild = *other.scalarChild;
    }
  }
  /**
   * @brief Create a node with matrix binary operation.
   *
   * @param[in] resultPtr Pointer to the matrix associated to this node.
   * @param[in] _opNum Operator number.
   * @param[in] cbf The call back function for this node.
   * @param[in] lhs The left child node.
   * @param[in] rhs The right child node.
   */
  void binOpSet(boost::shared_ptr<MT> resultPtr,
		            OpType operatorNum,
		            CallBackFuncType cbf,
		            const MatrixMatrixFunc<MT,ST> &lhs, 
		            const MatrixMatrixFunc<MT,ST> &rhs) {

    matrixPtr = resultPtr;

    opNum = operatorNum;

    callBackFunc = cbf;

    isConst = lhs.isConst && rhs.isConst;

    if (false==isConst) {
      varNumRows = MatrixAdaptorType::getNumRows(*matrixPtr);
      varNumCols = MatrixAdaptorType::getNumCols(*matrixPtr);
    } else {
      varNumRows = 0;
      varNumCols = 0;
    }
    leftChild = new MatrixMatrixFunc<MT,ST>;
    leftChild->deepCopy(lhs);
    rightChild = new MatrixMatrixFunc<MT,ST>;
    rightChild->deepCopy(rhs);
  }

  /**
   * @brief Set the unary operator for this node.
   *
   * @param[in] resultPtr pointer to result[Matrix].
   * @param[in] _opNum    Operator number.
   * @param[in] cbf       Callback function type.
   * @param[in] lhs       The operand of the unary operation.
   */
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

  /**
   * @brief Print out the computational tree to log file.
   * @param[out] os Output stream to print to.
   */
  void print(std::ostream& os=std::cout) const {
    MatrixAdaptorType::print(*matrixPtr, os);

    if (NULL==leftChild && NULL==rightChild) os << ":" << opName[opNum];
    else {
      os << "(" << opName[opNum];
      if (NULL!=leftChild) leftChild->print(os);
      if (NULL!=rightChild) { os << ","; rightChild->print(os); }
      os << ")";
    }
  }

  /**
   * @brief Print the computational tree to file.
   * @param[out] os Output stream to print to.
   */
  void println(std::ostream& os=std::cout) const {
    print(os);
    os << std::endl;
  }

  /**
   * @brief  Get the number of rows.
   *
   * @return Number of rows.
   */
  int numRows() const { return varNumRows; }

  /**
   * @brief Get the number of columns.
   *
   * @return Number of columns.
   */

  int numCols() const { return varNumCols; }

  /**
   * @brief  Get the matrix associated to this node.
   *
   * @return The matxi associated to this node.
   */

  virtual MT value() const { return(*matrixPtr); }

  // initial and result must point to existing MatrixTypes
  /**
   * @brief Traversse the computational tree in post-order. Replace the matrix
   * as moving towards the leaf nodes(Reverse Mode) by applying different 
   * specific callBackFunction to each node.
   *
   * @param[in] initial Initial matrix.
   * @param[in] result  Result matrix.
   * @param[in] transposeFlag Transpose Flag. 
   * @param[in] identityInitialFlag Boolean value for identity matrices.
   *                                If 1 the matrix of this node is 
   *                                identity matrix. 
   * @param[in] zeroResultFlag      Boolean value for identity matrices.
   *                                If 1 the matrix is a zero matrix.
   *                 
   *
   */
  void gradientVec(boost::shared_ptr<MT> initial, 
		               boost::shared_ptr<MT> result, 
                   boost::shared_ptr<MMFT> derivativeMMFT, /**< TODO this points to a newly create derivativeMMFT. */
                   boost::shared_ptr<MMFT> resultMMFT,
		               int transposeFlag, 
		               bool identityInitialFlag,
		               bool& zeroResultFlag ) const {

    /** Make these into individual asserts */
    assert(initial.use_count()>=1 && 
	         result.use_count()>=1  &&
	         isConst || 
           (MatrixAdaptorType::getNumRows(*(result)) == varNumRows &&
			      MatrixAdaptorType::getNumCols(*(result)) == varNumCols));

    // If the function is constant then the derivative is zero -- so
    // we don't need to do any computation
    if (!isConst) { 
      MT zero= MatrixAdaptorType::zeros(varNumRows, varNumCols);
      MatrixMatrixFunc<MT, ST> zeroMat(zero, false);
      boost::shared_ptr<MT> leftPtr(new MT);
//      if (NULL != leftChild) *leftPtr  = *(leftChild->matrixPtr);
      boost::shared_ptr<MT> rightPtr(new MT);
//      if (NULL != rightChild) *rightPtr  = *(rightChild->matrixPtr);
      boost::shared_ptr<MMFT> leftMMFT(new MMFT);
      boost::shared_ptr<MMFT> rightMMFT(new MMFT);
    // Replace matrices of its children according to different type
    // of callBackFunctions.
      callBackFunc(result, 
                   initial, 
                   leftPtr, 
                   rightPtr, 
                   derivativeMMFT,
                   leftMMFT,
                   rightMMFT,
                   resultMMFT,
                   this,
		               transposeFlag,
                   identityInitialFlag, 
                   zeroResultFlag);
      /*
      if (leftMMFT.use_count() >= 1 )
      if (rightMMFT.use_count() >= 1)
        */
      // Update the matrices recursively towards the leaf nodes.
      if (NULL!=leftChild) {
	      int leftFlag = transposeFlag & 1;
	      leftChild->gradientVec(leftPtr, 
				                       result, 
                               leftMMFT, /**< TODO: dummy ptr. replace with left derivativeMMFT. */
                               resultMMFT,  
				                       leftFlag, 
				                       identityInitialFlag,
				                       zeroResultFlag);
//        *(leftPtr) = leftChild->matrixVal; /**< restore previous matrix value 
//        for this node.*/
      }
      if (NULL!=rightChild) {
	      int rightFlag = transposeFlag & 2;
	      rightChild->gradientVec(rightPtr, 
				                        result,
                                rightMMFT,  /**< TODO: dummy ptr. replace with right derivativeMMFT. */
                                resultMMFT,
				                        rightFlag, 
				                        identityInitialFlag,
				                        zeroResultFlag);
//        *(rightPtr) = rightChild->matrixVal; /**< restore previous matrix value. */
      }
    } // end if (!isConst) 
  }
};
}  /** namespace AMD */

#endif
