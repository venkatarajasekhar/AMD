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

namespace AMD { 
  /**
   * @This function optimizing the both plus and minus operation
   * I believe there is no need to write a function for both plus and minus
   * because in fact they are the same. For minus we can pass the parameter 
   * as lhs and -rhs
   * @param lhs: The left child
   * @param rhs: The right child
   */
  MatrixMatrixFunc<MT, ST> plusMinusOpt(const MatrixMatrixFunc<MT, ST> &node){
    /** "node" HAS LHS AND RHS --- YOU SHOULD BE ACCEPTING THE "node" */

    /* Assume lhs and rhs pass the error checking part(diminsion match) */
    /* Case 1: If lhs or rhs or both are 0 matrix, we don't have compute 
     * lhs + rhs. 
     */
    if (lhs.zeroResultFlag == 1) return rhs;
    if (rhs.zeroResultFlag == 1) return lhs;

    /* Case 2: If lhs == -rhs, don't compute, return a zero matrix */
    /*TODO: I suggest we add negationFlag or cofficient  
    * if (lhs.cofficient + rhs.cofficient = 0) return zeroMatrix;
    */
    /* Case 3: If lhs == rhs, return factor*lhs
     */
    if (lhs == rhs){
      /* Step 1: Create a new MatrixMatrixFunc */
      MatrixMatrixFunc result = (lhs, false /*TODO: set cofficient to 2*/);
      return result;
    }

    /* No Optimization is done */
    return NULL;
  }

  /**
   * @This function optimizing M-M mulplication operation
   * @param lhs: The left child
   * @param rhs: The right child
   */
  MatrixMatrixFunc<MT, ST> multiplyOpt(const MatrixMatrixFunc<MT, ST> &node){
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
    if (lhs.zeroResultFlag ==1) return lhs;
    if (rhs.zeroResultFlag ==1) return rhs;

    /* Case 2: If lhs or rhs or both are identity matrix, we don't have compute 
     * lhs * rhs. */
    if (lhs.identityInitialFlag == 1) return rhs;
    if (rhs.identityInitialFlag == 1) return lhs;

    /* Case 3: If lhs == inv(rhs), we don't compute lhs * rhs. */
    if (lhs == rhs || (rhs.inverseFlag + lhs.inverseFlag ==1)){ 
      lhs.identityFlag = 1;
      return lhs;
    }
    /* Spelical case, lhs is orthonomal */
    //if (rhs == tranpose(lhs) && lhs is orthonomal) return eye; 

    /* Case 4: If lhs == rhs, we don't compute it in the symbolic step */
    /*if (lhs == rhs) 
     *  return (rhs ^{lhs.exponent + rhs.exponent})  
     */

    /* No Optimization is done */
    return NULL; 

    /** CASE 2 */
    /* Case 1: If rhs is 0 or lhs is a zero matrix.
     * then the result is simply a zero Martrix
     */
    if (rhs.FunctionVal == 0) {
      lhs.zeroResultFlag = 1;
      return lhs;
    }
    if (lhs.zeroResultFlag ==1) return lhs;

    /* Case 2: If rhs is a 1 then we can ignore it */

    /* TODO if (rhs is 1) return lhs; */

    /* Case 3: If rhs < 0, we negate the matrix */
    /*
    if (rhs is -1){
      set lhs's negate flag to 1
      return lhs;
    }
    */
    return NULL;

    /** CASE 3 */
    /**
     * Case 1: If either left or right is a zero matrix,
     * Return a zero matrix instead of acutally computing the result
     */
    if (lhs.zeroResultFlag == 1) return lhs;

    if (rhs.zeroResultFlag == 1) return rhs;

    /* Case 2: If either the left or right is identity 
     * Return the other side instead of computing*/
    if (lhs.identityFlag == 1 ) return rhs;

    if (rhs.identityFlag == 1) return lfs;
    
    return NULL;
  }


  /* This function optimizing the transpose operation.      
  * Notice thatfor a transpose inner node in the tree, it has only 1 child
  * @param node: The left child     
  */     
  MatrixMatrixFunc<MT, ST> transOpt(const MatrixMatrixFunc<MT, ST> &node){   
    /* Case 1: If node is eye or zero matrix, then there is no need to           
     * actually transpose it*/                                                  
    if (node.identityFlag == 1) return node;
    if (node.zeroResultFlag == 1) return node;
                                                                                
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We        
     * simply take both transpose operation */                                  
    if (node.transposeFlag == 1){  
      node.transposeFlag = 0; 
      return node;   
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

  MatrixMagtrixFunc<MT, ST> inverseOpt(const MatrixMatrixFunc<MT, ST> &node){    
    /* Assumption: node is invertible! */                                        
                                                                                
    /*Case 1: If node is eye or zero matrix, then there is no need to            
     * actually inverse it */                                                   
    if (node.identityFlag == 1) return node;                                          
    if (node.zeroResultFlag == 1) return node;                                                   
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
  ScalarMagtrixFunc<MT, ST> detOpt(const MatrixMatrixFunc<MT, ST> &node){        
    /* Case 1: If node is eye, return 1 */
    if (node.identityFlag == 1) return 1;

    /* Case 2: If node is zeroMatrix, return 0 */
    /* TODO: Logdet cannot be 0 */
    if (node.zeroResultFlag == 0) return 0;
    
    /*TODO: if (node == inverse of an expression A)
      calculating the determinant d of A.
      return 1/d;
    */
  }     

} /** namespace AMD */

#endif /** AMD_SymbolicOptimizer_HPP */
