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
    if (tree.info() == "tr") {
        return generateDerivativeExpressionHelper(
            tree.left(), identity, targetMatrix); 
    }
    //lgdt, apply inverse transpose of left child
    else if (tree.info() ==  "lgdt"){
        Expression2 tempLeftAcc(new ExpressionTree("_", tree.left(), nil));
        Expression2 leftAcc(new ExpressionTree("'", tempLeftAcc, nil));
        return generateDerivativeExpressionHelper(
            tree.left(), leftAcc, targetMatrix); 
    }
    else {
        //Should throw error as expression did not start with tr or lgdt
        //technically impossible if going through pipeline, but incase someone
        //directly calls the method on a self-created parsed tree
    }
}

Expression2 generateDerivativeExpressionHelper(
                           const Expression2& expr, 
                           const Expression2& acc,
                           const std::string targetMatrix)
{
        AMD::detail::Tree tree = *expr;
        //tr, apply identity matrix
        if (tree.info() == "tr") {
            Expression2 leftAcc(new ExpressionTree("*", acc, identity));
            return generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
       }
        //lgdt, apply inverse transpose of left child
       else if (tree.info() ==  "lgdt"){
            Expression2 tempLeftAcc(new ExpressionTree("_", acc, nil));
            Expression2 leftAcc(new ExpressionTree("'", tempLeftAcc, nil));
            return generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
        }
        //multiplication, apply acc*right' + left'*acc
       else if (tree.info() ==  "*"){
            Expression2 tempLeftAcc(new ExpressionTree("'", tree.right(), nil));
            Expression2 tempRightAcc(new ExpressionTree("'", tree.left(), nil));
            Expression2 leftAcc(new ExpressionTree("*", acc, tempLeftAcc));
            Expression2 rightAcc(new ExpressionTree("*", tempRightAcc, acc)); 
            Expression2 left = generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
            Expression2 right = generateDerivativeExpressionHelper(
                tree.right(), rightAcc, targetMatrix); 
            return addExpr(left, right);
        }
        //element wise multiply,element wise multiply opposite child by acc
        else if (tree.info() == "o"){
            Expression2 leftAcc(new ExpressionTree("o", tree.right(), acc));
            Expression2 rightAcc(new ExpressionTree("o", tree.left(), acc));
            Expression2 left = generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
            Expression2 right = generateDerivativeExpressionHelper(
                tree.right(), rightAcc, targetMatrix); 
            return addExpr(left, right); 
        }
        //inverse match, derivative unknown, for now apply identity
        else if (tree.info() ==  "_") {
            Expression2 leftAcc = acc;
            return generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
        }
        //transpose match, apply transpose
       else if (tree.info() == "'"){
            Expression2 leftAcc(new ExpressionTree("'", acc, nil));
            return generateDerivativeExpressionHelper(
                tree.left(), leftAcc, targetMatrix); 
       }
        //negation match, check unary/binary, negate/copy + negate respectively
       else if (tree.info() == "-"){
            if (tree.right()) {
                Expression2 leftAcc = acc;
                Expression2 rightAcc(new ExpressionTree("-", acc, nil));
                Expression2 left = generateDerivativeExpressionHelper(
                    tree.left(), leftAcc, targetMatrix); 
                Expression2 right = generateDerivativeExpressionHelper(
                    tree.right(), rightAcc, targetMatrix); 
                return addExpr(left, right); 
            }
            else {
                Expression2 leftAcc(new ExpressionTree("-", acc, nil));
                return generateDerivativeExpressionHelper(
                    tree.left(), leftAcc, targetMatrix); 
            }
        }
        //its a leaf node match with our target matrix, return accumulator
        else if (tree.info() == targetMatrix){
            return acc; 
        }
        //must be a constant expression, ie either a double or a constant
        //matrix which can be ignored, hence return zero
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
