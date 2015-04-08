#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>
#include <AMD/expressiontree.hpp>

namespace AMD {

typedef detail::ExpressionTree ExpressionTree;

static Expression nil;
static Expression zero(new AMD::detail::ExpressionTree("0.0", nil, nil));
static Expression identity(new ExpressionTree("I", nil, nil));

Expression2 generateDerivativeExpression(
                           const Expression2& expr, 
                           const std::string targetMatrix)
{
    AMD::detail::Tree tree = *expr;
    //Trace: tr(G(X)) = F(G(X))
    //R = I
    if (tree.info() == "tr") {
        return generateDerivativeExpressionHelper(
            tree.left(), identity, targetMatrix); 
    }
    //Log Determinant: lgdt(G(X)) = F(G(X))
    //R = G(X)'_
    else if (tree.info() ==  "lgdt"){
        Expression2 Gtranspose(new ExpressionTree("_", tree.left(), nil));
        Expression2 R(new ExpressionTree("'", Gtranspose, nil));
        return generateDerivativeExpressionHelper(
            tree.left(), R, targetMatrix); 
    }
    else {
        //Should throw error as expression did not start with tr or lgdt
        //technically impossible if going through pipeline, but incase someone
        //directly calls the method on a self-created parsed tree
    }
}

Expression2 generateDerivativeExpressionHelper(
                           const Expression2& expr, 
                           const Expression2& Z,
                           const std::string targetMatrix)
{
        AMD::detail::Tree tree = *expr;
        //Trace: tr(G(X)) = F(G(X))
        //R = I
        if (tree.info() == "tr") {
            Expression2 R(new ExpressionTree("*", identity, Z));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Log Determinant: lgdt(G(X)) = F(G(X))
        //R = G(X)'_
        else if (tree.info() ==  "lgdt"){
            Expression2 Zinverse(new ExpressionTree("_", Z, nil));
            Expression2 R(new ExpressionTree("'", Zinverse, nil));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Matrix Multiplication: H(X)*G(X) = F(G(X))
        //RLeft = Z*G(X)' 
        //RRight = H(X)'*Z
        else if (tree.info() ==  "*"){
            Expression2 Htranspose(new ExpressionTree("'", tree.right(), nil));
            Expression2 Gtranspose(new ExpressionTree("'", tree.left(), nil));
            Expression2 RLeft(new ExpressionTree("*", Z, Htranspose));
            Expression2 RRight(new ExpressionTree("*", Gtranspose, Z)); 
            Expression2 leftAcc = generateDerivativeExpressionHelper(
                tree.left(), RLeft, targetMatrix); 
            Expression2 rightAcc = generateDerivativeExpressionHelper(
                tree.right(), RRight, targetMatrix); 
            return addExpr(leftAcc, rightAcc);
        }
        //Element wise multiplication: H(X)oG(X) = F(G(X))
        //RLeft = G(X)oZ
        //RRight = H(X)oZ
        else if (tree.info() == "o"){
            Expression2 RLeft(new ExpressionTree("o", tree.right(), Z));
            Expression2 RRight(new ExpressionTree("o", tree.left(), Z));
            Expression2 leftAcc = generateDerivativeExpressionHelper(
                tree.left(), RLeft, targetMatrix); 
            Expression2 rightAcc = generateDerivativeExpressionHelper(
                tree.right(), RRight, targetMatrix); 
            return addExpr(leftAcc, rightAcc);
        }
        //Inverse: G(X)_ = F(G(X)
        //R = -Z*F(G(X)'*Z
        else if (tree.info() ==  "_") {
            Expression2 FGXtranspose(new ExpressionTree("'", expr, nil));
            Expression2 B(new ExpressionTree("*", Z, FGXtranspose));
            Expression2 C(new ExpressionTree("*", B, Z));
            Expression2 R(new ExpressionTree("-", C, nil));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        //Transpose: G(X)' = F(G(X))
        //R = Z'
        else if (tree.info() == "'"){
            Expression2 R(new ExpressionTree("'", Z, nil));
            return generateDerivativeExpressionHelper(
                tree.left(), R, targetMatrix); 
        }
        else if (tree.info() == "-"){
            //Binary Negation: H(X) - G(X)
            //RLeft = Z
            //RRight = -Z
            if (tree.right()) {
                Expression2 RLeft = Z;
                Expression2 RRight(new ExpressionTree("-", Z, nil));
                Expression2 leftAcc = generateDerivativeExpressionHelper(
                    tree.left(), RLeft, targetMatrix); 
                Expression2 rightAcc = generateDerivativeExpressionHelper(
                    tree.right(), RRight, targetMatrix); 
                return addExpr(leftAcc, rightAcc); 
            }
            //Unary Negation: -G(X) = F(G(X))
            //R = -Z 
            else {
                Expression2 R(new ExpressionTree("-", Z, nil));
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
            return zero;
        }
}

static Expression2 addExpr(Expression2& left, Expression2& right)
{
    Expression2 parent(new ExpressionTree("+",left,right));
    return parent;
}

} //End namespace
