#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>
#include <AMD/expressiontree.hpp>

namespace AMD {

typedef detail::ExpressionTree ExpressionTree;

static Expression NIL;
///<Represents a null tree pointer used to create leaf nodes and unary op nodes

static Expression ZERO(new AMD::detail::ExpressionTree("0.0", NIL, NIL));
///<Represents a ZERO constant value, used for non-variable leaf nodes

static Expression IDENTITY(new ExpressionTree("I", NIL, NIL));
///<Represents Identity matrix which is needed for trace derivatives

Expression generateDerivativeExpression(
                           const Expression& expr, 
                           const std::string targetMatrix)
{
    //

    const ExpressionTree& tree = *expr;
    //Trace: tr(G(X)) = F(G(X))
    //R = I
    if (tree.info() == "tr") {
        return generateDerivativeExpressionHelper(
            tree.left(), IDENTITY, targetMatrix); 
    }
    //Log Determinant: lgdt(G(X)) = F(G(X))
    //R = G(X)'_
    else if (tree.info() ==  "lgdt"){
        Expression Gtranspose(new ExpressionTree("_", tree.left(), NIL));
        Expression R(new ExpressionTree("\'", Gtranspose, NIL));
        return generateDerivativeExpressionHelper(
            tree.left(), R, targetMatrix); 
    }
    else {
        //Should throw error as expression did not start with tr or lgdt
        //technically impossible if going through pipeline, but incase someone
        //directly calls the method on a self-created parsed tree
    }
}

Expression generateDerivativeExpressionHelper(
                           const Expression& expr, 
                           const Expression& Z,
                           const std::string targetMatrix)
{
        const ExpressionTree& tree = *expr;
        //Trace: tr(G(X)) = F(G(X))
        //R = I
        if (tree.info() == "tr") {
            Expression R(new ExpressionTree("*", IDENTITY, Z));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Log Determinant: lgdt(G(X)) = F(G(X))
        //R = G(X)'_
        else if (tree.info() ==  "lgdt"){
            Expression Zinverse(new ExpressionTree("_", Z, NIL));
            Expression R(new ExpressionTree("\'", Zinverse, NIL));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Matrix Multiplication: H(X)*G(X) = F(G(X))
        //RLeft = Z*G(X)' 
        //RRight = H(X)'*Z
        else if (tree.info() ==  "*"){
            Expression Htranspose(new ExpressionTree("'", tree.right(), NIL));
            Expression Gtranspose(new ExpressionTree("'", tree.left(), NIL));
            Expression RLeft(new ExpressionTree("*", Z, Htranspose));
            Expression RRight(new ExpressionTree("*", Gtranspose, Z)); 
            Expression leftAcc = generateDerivativeExpressionHelper(
                tree.left(), RLeft, targetMatrix); 
            Expression rightAcc = generateDerivativeExpressionHelper(
                tree.right(), RRight, targetMatrix); 
            return addExpr(leftAcc, rightAcc);
        }
        //Element wise multiplication: H(X)oG(X) = F(G(X))
        //RLeft = G(X)oZ
        //RRight = H(X)oZ
        else if (tree.info() == "o"){
            Expression RLeft(new ExpressionTree("o", tree.right(), Z));
            Expression RRight(new ExpressionTree("o", tree.left(), Z));
            Expression leftAcc = generateDerivativeExpressionHelper(
                tree.left(), RLeft, targetMatrix); 
            Expression rightAcc = generateDerivativeExpressionHelper(
                tree.right(), RRight, targetMatrix); 
            return addExpr(leftAcc, rightAcc);
        }
        //Inverse: G(X)_ = F(G(X)
        //R = -Z*F(G(X)'*Z
        else if (tree.info() ==  "_") {
            Expression FGXtranspose(new ExpressionTree("\'", expr, NIL));
            Expression B(new ExpressionTree("*", Z, FGXtranspose));
            Expression C(new ExpressionTree("*", B, Z));
            Expression R(new ExpressionTree("-", C, NIL));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Transpose: G(X)' = F(G(X))
        //R = Z'
        else if (tree.info() == "'"){
            Expression R(new ExpressionTree("'", Z, NIL));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        else if (tree.info() == "-"){
            //Binary Negation: H(X) - G(X)
            //RLeft = Z
            //RRight = -Z
            if (tree.right()) {
                Expression RLeft = Z;
                Expression RRight(new ExpressionTree("-", Z, NIL));
                Expression leftAcc = generateDerivativeExpressionHelper(
                    tree.left(), RLeft, targetMatrix); 
                Expression rightAcc = generateDerivativeExpressionHelper(
                    tree.right(), RRight, targetMatrix); 
                return addExpr(leftAcc, rightAcc); 
            }
            //Unary Negation: -G(X) = F(G(X))
            //R = -Z 
            else {
                Expression R(new ExpressionTree("-", Z, NIL));
                return generateDerivativeExpressionHelper(
                    tree.left(), R, targetMatrix); 
            }
        }
        //Leaf Node (Target Matrix): X = F(G(X))
        //R = Z
        else if (tree.info() == targetMatrix){
            return Z; 
        }
        //Leaf Node (Constant double or matrix): c = F(G(X))
        //R = 0.0
        else {
            return ZERO;
        }
}

static Expression addExpr(Expression& left, Expression& right)
{
    Expression parent(new ExpressionTree("+",left,right));
    return parent;
}

} //End namespace
