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

Expression calcLogDetDerivative(Expression expr, const std::string variable)
{
    const ExpressionTree& tree = *expr;
    Expression Gtranspose(new ExpressionTree("_", tree.left(), NIL));
    Expression R(new ExpressionTree("\'", Gtranspose, NIL));
    return generateDerivativeExpressionHelper(tree.left(), R, variable);
}

Expression calcTraceDerivative(Expression expr, const std::string variable)
{
    const ExpressionTree& tree = *expr;
    return generateDerivativeExpressionHelper(tree.left(), IDENTITY, variable);
}

Expression calcTransposeDerivative(Expression expr, 
                                   Expression Z,
                                   const std::string variable)
{        
    const ExpressionTree& tree = *expr;
    Expression R(new ExpressionTree("'", Z, NIL));
    return generateDerivativeExpressionHelper(tree.left(), R, variable); 
}

Expression calcNegationDerivative(Expression expr, 
                                  Expression Z,
                                  const std::string variable)
{
    const ExpressionTree& tree = *expr;
    
    //Binary Negation: H(X) - G(X)
    //RLeft = Z
    //RRight = -Z
    if (tree.right()) {
        Expression RLeft = Z;
        Expression RRight(new ExpressionTree("-", Z, NIL));
        Expression leftAcc = generateDerivativeExpressionHelper(
            tree.left(), RLeft, variable); 
        Expression rightAcc = generateDerivativeExpressionHelper(
            tree.right(), RRight, variable); 
        return addExpr(leftAcc, rightAcc); 
    }
    //Unary Negation: -G(X) = F(G(X))
    //R = -Z 
    else {
        Expression R(new ExpressionTree("-", Z, NIL));
        return generateDerivativeExpressionHelper(
            tree.left(), R, variable); 
    }
}    
                           
Expression calcProductDerivative(Expression expr, 
                                 Expression Z,
                                 const std::string variable)
{
    const ExpressionTree& tree = *expr;
    Expression Htranspose(new ExpressionTree("'", tree.right(), NIL));
    Expression Gtranspose(new ExpressionTree("'", tree.left(), NIL));
    Expression RLeft(new ExpressionTree("*", Z, Htranspose));
    Expression RRight(new ExpressionTree("*", Gtranspose, Z)); 
    Expression leftAcc = generateDerivativeExpressionHelper(
        tree.left(), RLeft, variable); 
    Expression rightAcc = generateDerivativeExpressionHelper(
        tree.right(), RRight, variable); 
    return addExpr(leftAcc, rightAcc);
}
    
Expression calcElemProductDerivative(Expression expr,
                                     Expression Z,
                                     const std::string variable)
{        
    const ExpressionTree& tree = *expr;
    Expression RLeft(new ExpressionTree("o", tree.right(), Z));
    Expression RRight(new ExpressionTree("o", tree.left(), Z));
    Expression leftAcc = generateDerivativeExpressionHelper(
        tree.left(), RLeft, variable); 
    Expression rightAcc = generateDerivativeExpressionHelper(
        tree.right(), RRight, variable); 
    return addExpr(leftAcc, rightAcc);
}

Expression calcInverseDerivative(Expression expr, 
                                 Expression Z, 
                                 const std::string variable)
{       
    const ExpressionTree& tree = *expr;
    Expression FGXtranspose(new ExpressionTree("\'", expr, NIL));
    Expression B(new ExpressionTree("*", Z, FGXtranspose));
    Expression C(new ExpressionTree("*", B, Z));
    Expression R(new ExpressionTree("-", C, NIL));
    return generateDerivativeExpressionHelper(tree.left(), R, variable); 
}

Expression generateDerivativeExpression(const Expression& expr, 
                                        const std::string variable)
{
    const ExpressionTree& tree = *expr;
    //Trace: tr(G(X)) = F(G(X))
    //R = I
    if (tree.info() == "tr") {
        return calcTraceDerivative(expr, variable);
    }
    //Log Determinant: lgdt(G(X)) = F(G(X))
    //R = G(X)'_
    else if (tree.info() ==  "lgdt"){
        return calcLogDetDerivative(expr, variable);
    }
    else {
        //Should throw error as expression did not start with tr or lgdt
        //technically impossible if going through pipeline, but incase someone
        //directly calls the method on a self-created parsed tree
    }
}

Expression generateDerivativeExpressionHelper(const Expression& expr, 
                                              const Expression& Z,
                                              const std::string variable)
{
    const ExpressionTree& tree = *expr;
    //Trace: tr(G(X)) = F(G(X))
    //R = I
    if (tree.info() == "tr") {
        LOG_INFO << "Calculating trace derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcTraceDerivative(expr, variable);
    }
    //Log Determinant: lgdt(G(X)) = F(G(X))
    //R = G(X)'_
    else if (tree.info() ==  "lgdt"){
        LOG_INFO << "Calculating log det derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcLogDetDerivative(expr, variable);
    }
    //Matrix Multiplication: H(X)*G(X) = F(G(X))
    //RLeft = Z*G(X)' 
    //RRight = H(X)'*Z
    else if (tree.info() ==  "*"){
        LOG_INFO << "Calculating product derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcProductDerivative(expr, Z, variable);
    }
    //Element wise multiplication: H(X)oG(X) = F(G(X))
    //RLeft = G(X)oZ
    //RRight = H(X)oZ
    else if (tree.info() == "o"){
        LOG_INFO << "Calculating element-wise product derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcElemProductDerivative(expr, Z, variable);
    }
    //Inverse: G(X)_ = F(G(X)
    //R = -Z*F(G(X)'*Z
    else if (tree.info() ==  "_") {
        LOG_INFO << "Calculating inverse derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcInverseDerivative(expr, Z, variable);
    }
    //Transpose: G(X)' = F(G(X))
    //R = Z'
    else if (tree.info() == "'"){
        LOG_INFO << "Calculating transpose derivative (with Z = " << Z << ") on expresion: " << expr;
        return calcTransposeDerivative(expr, Z, variable);
    }
    else if (tree.info() == "-"){
        return calcNegationDerivative(expr, Z, variable);
    }
    //Leaf Node (Target Matrix): X = F(G(X))
    //R = Z
    else if (tree.info() == variable){
        LOG_TRACE << "Reached leaf node variable";
        LOG_INFO << "Returning Z as " << Z;
        return Z; 
    }
    //Leaf Node (Constant double or matrix): c = F(G(X))
    //R = 0.0
    else {
        LOG_TRACE << "Reached leaf node constant";
        return ZERO;
    }
}

static Expression addExpr(Expression& left, Expression& right)
{
    Expression parent(new ExpressionTree("+",left,right));
    return parent;
}

} //End namespace
