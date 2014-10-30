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
    if (lhs is zero Matrix) return rhs;
    if (rhs is zero Matrix) return lhs;

    /* Case 2: If lhs == -rhs, don't compute, return a zero matrix */
    if (lhs equalTo -rhs) return zeroMatrix;

    /* Case 3: If lhs == rhs, return factor*lhs
     */
    if (lhs equalTo rhs) 
      return ((lhs.factor + rhs.factor) *rhs);  
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
    if (lhs | rhs is zeroMatrix) return zeroMatrix;
    /* Case 2: If lhs or rhs or both are identity matrix, we don't have compute 
     * lhs * rhs. */
    if (lhs is eye) return rhs;
    if (rhs is eye) return lhs;

    /* Case 3: If lhs == inv(rhs), we don't compute lhs * rhs. */
    if (lhs == inv(rhs)) return eye;
    /* Spelical case, lhs is orthonomal */
    if (rhs == tranpose(lhs) && lhs is orthonomal) return eye; 

    /* Case 4: If lhs == rhs, we don't compute it in the symbolic step */
    if (lhs == rhs) 
      return (rhs ^{lhs.exponent + rhs.exponent})  
  }


  /* This function optimizing the transpose operation.                            
  * Notice thatfor a transpose inner node in the tree, it has only 1 child
  * @param lhs: The left child     
  */     
  MatrixMatrixFunc<MT, ST> transOpt(const MatrixMatrixFunc<MT, ST> &lhs){   
    /* Case 1: If lhs is eye or zero matrix, then there is no need to           
     * actually transpose it*/                                                  
    if (lhs is eye | zero) return lhs;                                          
                                                                                
    /* Case 2: If we are doing two transpose. Transpose(transpoe(A)). We        
     * simply take both transpose operation */                                  
    if (transPose flage is On) 
      set it to off and return lhs.symbol;   
      return;          
    /* Case 3: We don't want to transpose a symmetric either */                 
    if (lhs is Symmetric) 
      do not transpose.  
      return;                                    
  }
  /* This function optimizing the inverse operation.                            
   * Notice thatfor a inversion inner node in the tree, it has only 1 child
   * @param lhs: The left child      
   * */                                                                            
  MatrixMagtrixFunc<MT, ST> inverseOpt(const MatrixMatrixFunc<MT, ST> &lhs){    
    /* Assumption: lhs is invertible! */                                        
                                                                                
    /*Case 1: If lhs is eye or zero matrix, then there is no need to            
     * actually inverse it */                                                   
    if (lhs is eye | zero) return lhs;                                          
                                                                                
    /*Case 2: lhs is orthonomal, then (inv)lhs = tranpose(lhs). Compute         
     * tranpose is cheaper than taking the inverse*/                            
    if (lhs is orthonomal) {                                                    
      set transpose falg to on;                                                 
      return;                                                                   
    }                                                                           
  }
  /* This function optimizing how to calcualte the determinant */               
  ScalarMagtrixFunc<MT, ST> detOpt(const MatrixMatrixFunc<MT, ST> &lhs){        
    /* Case 1: If lhs is eye, return 1 */
    if (lhs is eye) return 1;

    /* Case 2: If lhs is zeroMatrix, return 0 */
    if (lhs is zeroMatrix) return 0;

    /* Case 3: If calculating det(A^-1) = (det(A))^-1  */
    if (lhs == inverse of an expression A)
      calculating the determinant d of A.
      return 1/d;

  }     
} } /** namespace AMD::detail */

#endif /** AMD_SymbolicOptimizer_HPP */
