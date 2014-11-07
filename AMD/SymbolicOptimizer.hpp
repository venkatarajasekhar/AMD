#ifndef AMD_SymbolicOptimizer_HPP
#define AMD_SymbolicOptimizer_HPP

/**
 * @file SymbolicOptimizer.hpp
 *
 * @author Wuwei Zhang
 *
 * @brief This file defines optiming functions to simplify the compuatatal tree
 * .
 * TODO: We are using the utliity.hpp as tempalte.
 *
 * NOTES FOR WUWEI:
 * (1) WE CAN REPRESENT IDENTITY and ZERO MATRIX as CONSTANT MATRICES. FIND 
 *     OUT HOW YOU CAN DECLARE CONSTANT MATRICES FOR EACH OF THE SUPPORTED 
 *     MATRIX TYPES (SYMBOLIC, ELEMENTAL, and EIGEN). I CAN HELP YOU.
 *
 * (2) YOU DON'T REALLY NEED TO STORE FLAGS FOR TRANSPOSE and INVERSE. THE 
 *     MATRIXMATRIXFUNC STORES THE OPERATION CODE, WHICH IS SUFFICIENT. FOR
 *     EXAMPLE, FOR transpose(transpose(X)), the root node will have OPERATION
 *     CODE of TRANPOSE, so will it's LEFT CHILD.
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
namespace AMD {
  /**
   * @This function optimizing the both plus and minus operation
   * I believe there is no need to write a function for both plus and minus
   * because in fact they are the same. For minus we can pass the parameter 
   * as lhs and -rhs
   * @param lhs: The left child
   * @param rhs: The right child
   */
  SymbolicMMFunc plusMinusOpt(const MatrixMatrixFunc<MT, ST> &node){
    /** "node" HAS LHS AND RHS --- YOU SHOULD BE ACCEPTING THE "node" */

    /* Assume lhs and rhs pass the error checking part(diminsion match) */
    /* Case 1: If lhs or rhs or both are 0 matrix, we don't have compute 
     * lhs + rhs. 
     */
    if (node.leftChild == symbolicZeroMatrix) return node.rightChild;
    if (node.rightChild == symbolicZeroMatrix) return node.leftChild;

    /* Case 2: If lhs == -rhs, don't compute, return a zero matrix */
    /*TODO: I suggest we add negationFlag or cofficient  
    * if (lhs.cofficient + rhs.cofficient = 0) return zeroMatrix;
    */
    /* Case 3: If lhs == rhs, return factor*lhs
     */
    /*
    if (node.leftChild == node.rightChild){
       Step 1: Create a new MatrixMatrixFunc 
      MatrixMatrixFunc result = (node.leftChild);
      return result;
    }
     */
    /* No Optimization is done */
    return NULL;
  }

  /**
   * @This function optimizing M-M mulplication operation
   * @param lhs: The left child
   * @param rhs: The right child
   */
  SymbolicMMFunc multiplyOpt(const MatrixMatrixFunc<MT, ST> &node){
    /** "node" HAS LHS AND RHS --- YOU SHOULD BE ACCEPTING THE "node" */

    /** THERE ARE 3 CASES HERE:
     *  (1) MATRIX TIMES MATRIX (LHS!=NULL and RHS!=NULL, OPTYPE=TIMES)
     *  (2) MATRIX TIMES SCALAR (LHS!=NULL and RHS==NULL, SCALARCHILD!=NULL)
     *  (3) SCALAR TIMES MATRIX (LHS==NULL and RHS!=NULL, SCALARCHILD!=NULL)
     *
     *  YOU CAN DIFFERENTIATE THEM VERY EASILY BY LOOKING AT THE OPERATION
     *  TYPE (TIMES, MTIMESS, STIMESM) AND BY LOOKING AT THE POINTERS. SO
     *  YOU DONT NEED THREE SEPARATE OPTIMIZERS HERE.
     */  

    /** CASE 1 */
    /* Assume lhs and rhs pass the error checking part(diminsion match) */
    /* Case 1: If lhs or rhs or both are 0 matrix, we don't have compute 
     * lhs * rhs. */
    if (node.leftChild == symbolicZeroMatrix) 
      return node.leftChild;
    if (node.rightChild == symbolicZeroMatrix)
      return node.rightChild;

    /* Case 2: If lhs or rhs or both are identity matrix, we don't have compute 
     * lhs * rhs. */
    if (node.leftChild == symbolicIdentityMatrix)
      return node.rightChild;
    if (node.rightChild == symbolicIdentityMatrix)
      return node.leftChild;


    /* Case 3: If lhs == inv(rhs), we don't compute lhs * rhs. */
    if (node.rightChild.opNum == INV){ 
      if (node.rightChild.leftChild == node.leftChild){
        return symbolicIdentityMatrix;
      }
    }
    if (node.leftChild.opNum == INV){
      if (node.leftChild.leftChild == node.rightChild){
        return symbolicIdentityMatrix;
      }
    }
  
    /* Spelical case, lhs is orthonomal */
    //if (rhs == tranpose(lhs) && lhs is orthonomal) return eye; 

    /* Case 4: If lhs == rhs, we don't compute it in the symbolic step */
    /*if (lhs == rhs) 
     *  return (rhs ^{lhs.exponent + rhs.exponent})  
     */

    /* No Optimization is done */
    return NULL; 
  }


  /* This function optimizing the transpose operation.      
  * Notice thatfor a transpose inner node in the tree, it has only 1 child
  * @param node: The left child     
  */     
  SymbolicMMFunc transOpt(const MatrixMatrixFunc<MT, ST> &node){   
    /* Case 1: If node is eye or zero matrix, then there is no need to           
     * actually transpose it*/                                                  
    if (node.leftChild == symbolicIdentityMatrix) 
      return node.leftChild;
    if (node.leftChild == symbolicZeroMatrix)
      return node.leftChild;
                                                                                
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We        
     * simply take both transpose operation */                                  
    if (node.leftChild.opNum == TRANSPOSE){   
      return node.leftChild.leftChild;   
    }          
    /* TODO: Case 3: We don't want to transpose a symmetric either */                 
    /*
    if (node is Symmetric) 
      do not transpose.  
      return;
    */
    return NULL;
  }
  /* This function optimizing the inverse operation.                            
   * Notice thatfor a inversion inner node in the tree, it has only 1 child
   * @param node: The left child      
   * */                                                                 

  SymbolicMMFunc<MT, ST> inverseOpt(const MatrixMatrixFunc<MT, ST> &node){    
    /* Assumption: node is invertible! */                                        
                                                                                
    /*Case 1: If node is eye or zero matrix, then there is no need to            
     * actually inverse it */                                                   
    if (node.leftChild == symbolicIdentityMatrix) 
      return node.leftChild;                                          
    if (node.leftChild == symbolicZeroMatrix) 
      return node.leftChild;                                                   
    /*TODO: Case 2: node is orthonomal, then (inv)node = tranpose(node).
     * Compute         tranpose is cheaper than taking the inverse*/                            
    /* if (node is orthonomal) {                                                    
      set transpose falg to on;                                                 
      return;                                                                   
    }
    */
    return NULL;
  }

/* This function optimizing how to calcualte the determinant */               

//  ScalarMatrixFunc<MT, ST> detOpt(const MatrixMatrixFunc<MT, ST> &node){        
    /* Case 1: If node is eye, return 1 */
  //  if (node.leftChild == symbolicIdentityMatrix ) {
    //  ScalarMatrixFunc<MT,ST> retValue()
    //  return 1;
   // }
    /* Case 2: If node is zeroMatrix, return 0 */
    /* TODO: Logdet cannot be 0 */
   // if (node.leftChild == symbolicZeroMatrix) return 0;
    
    /*TODO: if (node == inverse of an expression A)
      calculating the determinant d of A.
      return 1/d;
    */
//  }     

} /** namespace AMD */

#endif /** AMD_SymbolicOptimizer_HPP */
