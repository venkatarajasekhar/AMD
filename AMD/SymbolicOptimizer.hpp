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
 */

#include <string>

namespace AMD { namespace detail {
  /**
   * @This function optimizing the both plus and minus operation
   * I believe there is no need to write a function for both plus and minus
   * because in fact they are the same. For minus we can pass the parameter 
   * as lhs and -rhs
   * @param lhs: The left child
   * @param rhs: The right child
   */
  MatrixMatrixFunc<MT, ST> plusMinusOpt(const MatrixMatrixFunc<MT, ST> &lhs,
                                        const MatrixMatrixFunc<MT, ST> &rhs) {
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
  MatrixMatrixFunc<MT, ST> mulOpt(const MatrixMatrixFunc<MT, ST> &lhs,
                                  const MatrixMatrixFunc<MT, ST> &rhs) {   
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
  }

  /**
   * @This function optimizing M-S multiplication operation.
   * @param lhs: The left child
   * @param rhs: The right child
   * Assume right is always scalar, if not the caller should manually swithch
   */
  MatrixMatrixFunc<MT, ST> mulOpt(const MatrixMatrixFunc<MT, ST> &lhs,
                                  const ScalarMatrixFunc<MT,ST> &rhs){
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
  }

  MatrixMatrixFunc<MT, ST> eleMulOpt(const MatrixMatrixFunc<MT, ST> &lhs,
                                     const MatrixMatrixFunc<MT, ST> &rhs){
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
  * @param lhs: The left child     
  */     
  MatrixMatrixFunc<MT, ST> transOpt(const MatrixMatrixFunc<MT, ST> &lhs){   
    /* Case 1: If lhs is eye or zero matrix, then there is no need to           
     * actually transpose it*/                                                  
    if (lhs.identityFlag == 1) return lhs;
    if (lhs.zeroResultFlag == 1) return lhs;
                                                                                
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We        
     * simply take both transpose operation */                                  
    if (lhs.transposeFlag == 1){  
      lhs.transposeFlag = 0; 
      return lhs;   
    }          
    /* TODO: Case 3: We don't want to transpose a symmetric either */                 
    /*
    if (lhs is Symmetric) 
      do not transpose.  
      return;
    */
    return NULL;
  }
  /* This function optimizing the inverse operation.                            
   * Notice thatfor a inversion inner node in the tree, it has only 1 child
   * @param lhs: The left child      
   * */                                                                 

  MatrixMagtrixFunc<MT, ST> inverseOpt(const MatrixMatrixFunc<MT, ST> &lhs){    
    /* Assumption: lhs is invertible! */                                        
                                                                                
    /*Case 1: If lhs is eye or zero matrix, then there is no need to            
     * actually inverse it */                                                   
    if (lhs.identityFlag == 1) return lhs;                                          
    if (lhs.zeroResultFlag == 1) return lhs;                                                   
    /*TODO: Case 2: lhs is orthonomal, then (inv)lhs = tranpose(lhs). Compute         
     * tranpose is cheaper than taking the inverse*/                            
    /* if (lhs is orthonomal) {                                                    
      set transpose falg to on;                                                 
      return;                                                                   
    }
    */
    return NULL;
  }
  /* This function optimizing how to calcualte the determinant */               
  ScalarMagtrixFunc<MT, ST> detOpt(const MatrixMatrixFunc<MT, ST> &lhs){        
    /* Case 1: If lhs is eye, return 1 */
    if (lhs.identityFlag == 1) return 1;

    /* Case 2: If lhs is zeroMatrix, return 0 */
    /* TODO: Logdet cannot be 0 */
    if (lhs.zeroResultFlag == 0) return 0;
    
    /*TODO: if (lhs == inverse of an expression A)
      calculating the determinant d of A.
      return 1/d;
    */
  }     
} } /** namespace AMD::detail */

#endif /** AMD_SymbolicOptimizer_HPP */
