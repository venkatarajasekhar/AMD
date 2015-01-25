#ifndef AMD_SymbolicOptimizer_HPP
#define AMD_SymbolicOptimizer_HPP

/**
 * @file SymbolicOptimizer.hpp
 *
 * @author Wuwei Zhang
 *
 * @brief This file defines optiming functions to simplify the compuatatal tree
 * .
 *
 */

#include <string>

#include "SymbolicMatrixMatlab.hpp"
#include "MatrixAdaptor.hpp"
#include "MatrixMatrixFunc.hpp"
#include "SymbolicMatrixMatlab.hpp"
#include "SymbolicMatrixMatlabHelper.hpp"
#include "MatrixMatrixFuncHelper.hpp"
typedef AMD::SymbolicMatrixMatlab symbolic_matrix_type;
typedef AMD::MatrixAdaptor_t<symbolic_matrix_type> symbolic_adaptor_type;
typedef symbolic_adaptor_type::value_type symbolic_value_type;
typedef AMD::MatrixMatrixFunc<symbolic_matrix_type,
symbolic_value_type> SymbolicMMFunc;
typedef AMD::ScalarMatrixFunc<symbolic_matrix_type,
symbolic_value_type> SymbolicSMFunc;
const int POSTORDER = 1;
const int INORDER = 2;
const int PREORDER = 3;
namespace AMD {
  /**
   * This function optimize the symbolic expression and automatically
   * choose which opt functions to use like minusOpt or optimizeOpt
   * @param node
   */
  
  /**
   * @This function optimizing the minus operation
   * @param node
   */
  
  SymbolicMMFunc* minusOpt(SymbolicMMFunc &node){
    if (node.leftChild->mType == kZero){
      /* Create a negation version of rightChild */
      node.opNum = NEGATION;
      node.leftChild->shallowCopy(*node.rightChild);
      return &node;
    }
    if (node.rightChild->mType == kZero)
      return node.leftChild;
    return &node;
  }
  
  /**
   * @This function optimizing the plus operation
   * @param node
   */
  
  SymbolicMMFunc* plusOpt(SymbolicMMFunc &node){
    
    if (node.leftChild ==NULL || node.rightChild == NULL)
      return &node;
    if (node.leftChild->mType == kZero)
      return node.rightChild;
    if (node.rightChild->mType == kZero) return node.leftChild;
    /* No optimize is done */
    return &node;
  }
  
  
  /**
   * @This function optimizing S-M and M-S mulplication operation
   * @param node
   */
  
  SymbolicMMFunc* SMmultiplyOpt(SymbolicMMFunc &node){
    /* Case B: Matrix * Scalar */
    if (node.opNum == MTIMESS){
      if(node.leftChild != NULL && node.scalarChild != NULL){
        if (node.leftChild->mType == kZero){
          return node.leftChild;
        }
        else if (node.scalarChild == 0){
          node.mType = kZero;
          return &node;
        }
      }
    }
    /* Case B: Matrix * Scalar */
    else if(node.opNum == STIMESM){
      if(node.rightChild != NULL && node.scalarChild != NULL){
        if (node.rightChild->mType == kZero){
          return node.rightChild;
        }
        else if (node.scalarChild == 0){
          node.mType = kZero;
          return &node;
        }
      }
    }
    
    return &node;
  }
  /**
   * @This function optimizing M-M mulplication operation
   * @param node;
   */
  SymbolicMMFunc* multiplyOpt(SymbolicMMFunc &node){
    /** "node" HAS LHS AND RHS --- YOU SHOULD BE ACCEPTING THE "node" */
    
    
    /* CASE A Matrix * Matrix */
    if (node.leftChild != NULL && node.rightChild != NULL){
      /* Case 1: If lhs or rhs or both are 0 matrix, we don't have compute
       * lhs * rhs. */
      if (node.leftChild->mType == kZero)
        return node.leftChild;
      if (node.rightChild->mType == kZero)
        return node.rightChild;
      
      /* Case 2: If lhs or rhs or both are identity matrix, we don't have compute
       * lhs * rhs. */
      if (node.leftChild->mType == kIdentity)
        return node.rightChild;
      if (node.rightChild->mType == kIdentity)
        return node.leftChild;
      
      
      /* Case 3: If lhs == inv(rhs), we don't compute lhs * rhs. */
      if (node.rightChild->opNum == INV){
        if (node.rightChild->leftChild == node.leftChild){
          node.mType = kIdentity;
          return &node;
        }
      }
      if (node.leftChild->opNum == INV){
        if (node.leftChild->leftChild == node.rightChild){
          node.mType = kIdentity;
          return &node;
        }
      }
    }
    return &node;
  }
  
  
  
  /* This function optimizing the transpose operation.
   * Notice thatfor a transpose inner node in the tree, it has only 1 child
   * @param node
   */
  SymbolicMMFunc* transOpt(SymbolicMMFunc &node){
    /* Case 1: If node is eye or zero matrix, then there is no need to
     * actually transpose it*/
    if (node.leftChild == NULL) return &node;
    if (node.leftChild->mType == kIdentity)
      return node.leftChild;
    if (node.leftChild->mType == kZero)
      return node.leftChild;
    
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We
     * simply take both transpose operation */
    if (node.leftChild->opNum == TRANSPOSE){
      if(node.leftChild->leftChild!=NULL)
        return node.leftChild->leftChild;
      else if(node.leftChild->rightChild!=NULL)
        return node.leftChild->rightChild;
    }
    /* TODO: Case 3: We don't want to transpose a symmetric either */
    /*
     if (node is Symmetric)
     do not transpose.
     return;
     */
    return &node;
  }
  
  /* This function optimizing the inverse operation.
   * Notice that for a inversion inner node in the tree, it has only 1 child
   * @param node:
   * */
  
  SymbolicMMFunc* inverseOpt(SymbolicMMFunc &node){
    /* Assumption: if node exists then it is invertible! */
    if (node.leftChild == NULL) return &node;
    /*Case 1: If node is eye or zero matrix, then there is no need to
     * actually inverse it */
    if (node.leftChild->mType == kIdentity)
      return node.leftChild;
    if (node.leftChild->mType == kZero)
      return node.leftChild;
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We
     * simply take both transpose operation */
    if (node.leftChild->opNum == INV){
      if(node.leftChild->leftChild!=NULL)
        return node.leftChild->leftChild;
      else if(node.leftChild->rightChild!=NULL)
        return node.leftChild->rightChild;
    }
    /*TODO: Case 2: node is orthonomal, then (inv)node = tranpose(node).
     * Compute         tranpose is cheaper than taking the inverse*/
    /* if (node is orthonomal) {
     set transpose falg to on;
     return;
     }
     */
    return &node;
  }
  /* This function optimizing log-determinant */
  SymbolicMMFunc* logdetOpt(SymbolicMMFunc &node){
    /* Case 1: If node is eye, return 1 */
    if (node.leftChild != NULL){
      
      
      if (node.leftChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("1");
        SymbolicSMFunc retVal(one,
                              node.leftChild->varNumRows,
                              node.leftChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      /* Case 2: If node is zeroMatrix, return 0 */
      if (node.leftChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("0");
        SymbolicSMFunc retVal(one,
                              node.leftChild->varNumRows,
                              node.leftChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }

    }
    
    if (node.rightChild != NULL){
      
      
      if (node.rightChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("1");
        SymbolicSMFunc retVal(one,
                              node.rightChild->varNumRows,
                              node.rightChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      /* Case 2: If node is zeroMatrix, return 0 */
      if (node.rightChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("0");
        SymbolicSMFunc retVal(one,
                              node.rightChild->varNumRows,
                              node.rightChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      
    }
    return &node;
  }
  
  /* This function optimizing trace */
  SymbolicMMFunc* traceOpt(SymbolicMMFunc &node){

    if (node.leftChild != NULL){
      /* Case 1: If node is eye, return 1 */
      if (node.leftChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one(node.leftChild->varNumCols);
        SymbolicSMFunc retVal(one,
                              node.leftChild->varNumRows,
                              node.leftChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      /* Case 2: If node is zeroMatrix, return 0 */
      if (node.leftChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("0");
        SymbolicSMFunc retVal(one,
                              node.leftChild->varNumRows,
                              node.leftChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      
    }
    
    if (node.rightChild != NULL){
      
      if (node.rightChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one(node.rightChild->varNumRows);
        SymbolicSMFunc retVal(one,
                              node.rightChild->varNumRows,
                              node.rightChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      /* Case 2: If node is zeroMatrix, return 0 */
      if (node.rightChild->mType == kIdentity ) {
        AMD::SymbolicScalarMatlab one("0");
        SymbolicSMFunc retVal(one,
                              node.rightChild->varNumRows,
                              node.rightChild->varNumCols);
        node.scalarChild = &retVal;
        return &node;
      }
      
    }
    return &node;
  }

  /* This function optimize the current node and its children recursively
   * with a specified optimize flag, the default is 1 == POSTORDER */
  SymbolicMMFunc* optimizeCurrentNode(SymbolicMMFunc &node){
    if (node.opNum == PLUS)
      return plusOpt(node);
    else if (node.opNum ==MINUS)
      return minusOpt(node);
    else if (node.opNum == TIMES)
      return multiplyOpt(node);
    else if (node.opNum == STIMESM || node.opNum == MTIMESS)
      return SMmultiplyOpt(node);
    else if (node.opNum == INV)
      return inverseOpt(node);
    else if  (node.opNum ==TRANSPOSE)
      return transOpt(node);
    return &node;
  }
  SymbolicMMFunc* optimize(SymbolicMMFunc &node, int optOrder){
    
    if (optOrder == PREORDER) optimizeCurrentNode(node);
    
    if (node.leftChild != NULL){
      node.leftChild = optimize(*(node.leftChild),optOrder);
    }
    
    if (optOrder == INORDER) optimizeCurrentNode(node);
    
    if (node.rightChild != NULL){
      node.rightChild =optimize(*(node.rightChild),optOrder);
    }
    if (optOrder == POSTORDER) optimizeCurrentNode(node);
    return &node;
  }
  
  
  SymbolicMMFunc* optimize(SymbolicMMFunc &node){
    return optimize(node, POSTORDER);
  }
  
  
  
  
} /** namespace AMD */

#endif /** AMD_SymbolicOptimizer_HPP */
