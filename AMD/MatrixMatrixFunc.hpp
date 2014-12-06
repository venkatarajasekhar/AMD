#ifndef MatrixMatrixFunc_H
#define MatrixMatrixFunc_H
/**
 * @file MatrixMatrixFunc.hpp
 *
 * @author Peder Olsen, Anju Kambadur, Allan Sapucaia
 *
 * @brief This file defines a class to present Matrix-Matrix function. This
 * class acts as nodes in the computational tree. The computation of matrix
 * derivatives is performed by traversing the computation tree in the reverse
 * mode.
 */
#include <iostream>
#include <string>
#include <cstdio>
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
    typedef MatrixMatrixFunctionType MMF;
    typedef void(*CallBackFuncType)(boost::shared_ptr<MT>,
                                    boost::shared_ptr<MT>,
                                    boost::shared_ptr<MT>,
                                    boost::shared_ptr<MT>,
                                    boost::shared_ptr<MMF>,/**< current MMF.*/
                                    boost::shared_ptr<MMF>,/**< left MMF. */
                                    boost::shared_ptr<MMF>,/**< right MMF. */
                                    boost::shared_ptr<MMF>,/**< result MMF. */
                                    const MatrixMatrixFunc<MT, ST>*,
                                    int&, bool&, bool&);
 
   boost::shared_ptr<MT> matrixPtr; /**< Once recorded the matrix should never
                                          be changed - so a pointer is safe */
    CallBackFuncType callBackFunc; /**< Used to compute derivative after
                                        evaluation tree is recorded */
    OpType opNum; /**< enum for the operator used at this node in the tree */
    MatrixType mType;
    bool isConst; /**< is this a variable or a constant function */
    int numRows; /**< number of rows in matrix */
    int numCols; /**< number of cols in matrix */
    //int varNumRows; /**< number of rows in matrix variable */
    //int varNumCols; /**< number of cols in matrix variable */
#if 1
    MatrixMatrixFunc* leftChild; /**< optional left child */
    MatrixMatrixFunc* rightChild; /**< optional right child */
    ScalarMatrixFunc<MT, ST>* scalarChild; /**< scalar func * matrix func. */
#else
    boost::shared_ptr<MatrixMatrixFunc> leftChild; /**< optional left child */
    boost::shared_ptr<MatrixMatrixFunc> rightChild; /**< optional right child */
    boost::shared_ptr<ScalarMatrixFunc<MT, ST> > scalarChild; /**< scalar func * matrix func. */
#endif

    // TODO think about an extra scalar value here
    /**
     * @brief This is an empty constructor that initializes all values to
     * defaults.  Create a invalid function.
     */
    MatrixMatrixFunc() : matrixPtr(),
      callBackFunc(NULL),
      opNum(NONE),
      isConst(true),
      mType(kInvalid),
      numRows(0),
      numCols(0),
      leftChild(NULL),
      rightChild(NULL),
      scalarChild(NULL) { }

    /**
     * @brief Makes an expensive copy of matrix -- avoid this constructor
     * if your matrices are large.
     */
    MatrixMatrixFunc(const MT& matrix, bool isConst = true, MatrixType mType = kGeneral) : matrixPtr(),
      callBackFunc(NULL),
      opNum(NONE),
      isConst(isConst),
      mType(mType),
      numRows(0),
      numCols(0),
      leftChild(NULL),
      rightChild(NULL),
      scalarChild(NULL) {
      typedef MatrixAdaptor_t<MT> MatrixAdaptorType;

      // Makes an deep-copy of the matrix.
      // TODO: new MT(matrix) should really be MatrixAdaptorType::copy()
      boost::shared_ptr<MT> copy(new MT(matrix));
      matrixPtr = copy;
      setVariableType(isConst);
    }

    /**
     * @brief Constructor with a MT type variable.
     */
    MatrixMatrixFunc(boost::shared_ptr<MT> matrixPtr,
      bool isConst = true, MatrixType mType = kGeneral) : matrixPtr(matrixPtr),
      callBackFunc(NULL),
      opNum(NONE),
      isConst(isConst),
      mType(mType),
      numRows(0),
      numCols(0),
      leftChild(NULL),
      rightChild(NULL),
      scalarChild(NULL) {
      setVariableType(isConst);
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
    void setVariableType(bool isVariable) {
      // constant and variable matrices must be leaf nodes
      AMD_START_TRY_BLOCK()
        if (NULL != leftChild || NULL != rightChild)
          throw exception_generic_impl("AMD::isVariable", 
          "Node is not a leaf", AMD_INTERNAL_NODE);

        // If is constant, call the callbackfunction for constant.
        if (isConst) {
          callBackFunc = constOp < MT, ST > ;
          opNum = CONST;
          numRows = 0;
          numCols = 0;
        }
        else {
          callBackFunc = varOp < MT, ST > ;
          opNum = VAR;
          numRows = MatrixAdaptorType::getNumRows(*(matrixPtr));
          numCols = MatrixAdaptorType::getNumCols(*(matrixPtr));
        
        }
      AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, isVariable)
    }

    /**
     * @brief Reset the entire computational tree.
     */
    void reset() {
      callBackFunc = NULL;
      opNum = NONE;
      numRows = 0;
      numCols = 0;
      // Reset its left & right child recursively.
      if (NULL != leftChild) delete leftChild;
      if (NULL != rightChild) delete rightChild;
      if (NULL != scalarChild) delete scalarChild;
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
      matrixPtr = other.matrixPtr;
      opNum = other.opNum;
      isConst = other.isConst;
      callBackFunc = other.callBackFunc;
      numRows = other.getNumRows();
      numCols = other.getNumCols();
      leftChild = NULL;
      rightChild = NULL;
      scalarChild = NULL;
    }

    /**
     * @function
     *     * Create a deep copy of the current MatrixMatrixFunc node. This function
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
        leftChild = new MatrixMatrixFunc < MT, ST > ;
        leftChild->deepCopy(*(other.leftChild));
      }

      if (NULL != other.rightChild) {
        rightChild = new MatrixMatrixFunc < MT, ST > ;
        rightChild->deepCopy(*(other.rightChild));
      }
      if (NULL != other.scalarChild) {
        scalarChild = new ScalarMatrixFunc < MT, ST > ;
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
#if 1
    void binOpSet(boost::shared_ptr<MT> resultPtr,
      OpType operatorNum,
      CallBackFuncType cbf,
      const MatrixMatrixFunc<MT, ST> &lhs,
      const MatrixMatrixFunc<MT, ST> &rhs) {

      matrixPtr = resultPtr;

      opNum = operatorNum;

      callBackFunc = cbf;

      isConst = lhs.isConst && rhs.isConst;

    
      if (false == isConst) {
        numRows = MatrixAdaptorType::getNumRows(*matrixPtr);
        numCols = MatrixAdaptorType::getNumCols(*matrixPtr);
      }
      else {
        numRows = 0;
        numCols = 0;
      }
      
      /** TODO: Peder, why is there a deepCopy() here? */
      /** Because of temporaries */
      leftChild = new MatrixMatrixFunc < MT, ST > ;
      leftChild->deepCopy(lhs);
      rightChild = new MatrixMatrixFunc < MT, ST > ;
      rightChild->deepCopy(rhs);
    }
#else
    void binOpSet(boost::shared_ptr<MT> resultPtr,
      OpType operatorNum,
      CallBackFuncType cbf,
      boost::shared_ptr<MatrixMatrixFunc<MT, ST> > lhsMMF,
      boost::shared_ptr<MatrixMatrixFunc<MT, ST> > rhsMMF) {

      matrixPtr = resultPtr;
      opNum = operatorNum;
      callBackFunc = cbf;
      isConst = lhs.isConst && rhs.isConst;

      
      if (false == isConst) {
        numRows = MatrixAdaptorType::getNumRows(*matrixPtr);
        numCols = MatrixAdaptorType::getNumCols(*matrixPtr);
      }
      else {
        numRows = 0;
        numCols = 0;
      }
      
      leftChild = lhsMMF;
      rightChild = rhsMMF;
    }

#endif

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
      const MatrixMatrixFunc<MT, ST> &lhs) {
      numRows = lhs.getNumRows();  // should be 0 or a size
      numCols = lhs.getNumCols();
      matrixPtr = resultPtr;
      opNum = _opNum;
      isConst = lhs.isConst;
      callBackFunc = cbf;
      leftChild = new MatrixMatrixFunc < MT, ST > ;
      leftChild->deepCopy(lhs);
    }

    /**
     * @brief Print out the computational tree to log file.
     * @param[out] os Output stream to print to.
     */
    void print(std::ostream& os = std::cout) const {
      MatrixAdaptorType::print(*matrixPtr, os);

      if (NULL == leftChild && NULL == rightChild) os << ":" << opName[opNum];
      else {
        os << "(" << opName[opNum];
        if (NULL != leftChild) leftChild->print(os);
        if (NULL != rightChild) { os << ","; rightChild->print(os); }
        os << ")";
      }
    }

    /**
     * @brief Print the computational tree to file.
     * @param[out] os Output stream to print to.
     */
    void println(std::ostream& os = std::cout) const {
      print(os);
      os << std::endl;
    }

    /**
     * @brief  Get the number of rows.
     *
     * @return Number of rows.
     */
    int getNumRows() const { return numRows; }

    /**
     * @brief Get the number of columns.
     *
     * @return Number of columns.
     */

    int getNumCols() const { return numCols; }

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
    void gradientVec(boost::shared_ptr<MT> current,
      boost::shared_ptr<MT> result,
      boost::shared_ptr<MMF> currentMMF,
      boost::shared_ptr<MMF> resultMMF,
      int transposeFlag,
      bool identityInitialFlag,
      bool& zeroResultFlag) const {

      AMD_START_TRY_BLOCK()
      bool b_valid_shared_ptr = 
          current.use_count() >= 1 && result.use_count() >= 1;
      bool b_constant_function = isConst;
      bool b_matched_dimension = ( true == (*currentMMF).isConst || false == (*resultMMF).isConst ) ||
        (MatrixAdaptorType::getNumRows(*(result)) == getNumRows() &&
         MatrixAdaptorType::getNumCols(*(result)) == getNumCols() );

      /* Throwing first exception found. Alternatively, we could throw the
       * most critical one instead. */
      if (false == b_matched_dimension) throw 
        exception_generic_impl("AMD::gradientVec",
                               "Dimensions of result and current don't match",
                               AMD_MISMATCHED_DIMENSIONS);
      if (false == b_valid_shared_ptr) throw
        exception_generic_impl("AMD::gradientVec",
                               "Shared pointer is not valid anymore",
                               AMD_INVALID_SHARED_PTR);
      
      /**
       * Only need to do something if the current function is not a
       * constant
       */
      if (!isConst) {
        /** This will be the result matrix for the left child */
        boost::shared_ptr<MT> currentLeft(new MT);

        /** This will be the result matrix for the right child */
        boost::shared_ptr<MT> currentRight(new MT);

        /** These two are used for derivative of derivatives */
        boost::shared_ptr<MMF> leftMMF(new MMF);
        boost::shared_ptr<MMF> rightMMF(new MMF);
        callBackFunc(result,
          current,
          currentLeft,
          currentRight,
          resultMMF,
          currentMMF,
          leftMMF,
          rightMMF,
          this,
          transposeFlag,
          identityInitialFlag,
          zeroResultFlag);

        /**
         * The resultPtr is shared --- both leftChild and rightChild add
         * to this resultPtr. Therefore we cannot parallelize the calls
         * to leftChild and rightChild without cloning the resultPtr.
         *
         * Parallelization opportunity here:
         * (1) leftResultPtr = shared_ptr<>(new MT(*resultPtr));
         *     rightResultPtr = shared_ptr<>(new MT(*resultPtr));
         *
         * (2) Then call leftChild and rightChild gradientVec() with
         * leftResultPtr and rightResultPtr instead of resultPtr in
         * parallel.
         *
         * (3) *resultPtr = *leftResultPtr + *rightResultPtr
         */
        if (NULL != leftChild) {
          int leftFlag = transposeFlag & 1;
          leftChild->gradientVec(currentLeft,
            result,
            leftMMF,
            resultMMF,
            leftFlag,
            identityInitialFlag,
            zeroResultFlag);
        }
        if (NULL != rightChild) {
          int rightFlag = transposeFlag & 2;
          rightChild->gradientVec(currentRight,
            result,
            rightMMF,
            resultMMF,
            rightFlag,
            identityInitialFlag,
            zeroResultFlag);
        }
      } // end if (!isConst) 
      AMD_END_TRY_BLOCK()
      AMD_CATCH_AND_RETHROW(AMD, gradientVec)
    }
  };
}  /** namespace AMD */

#endif
