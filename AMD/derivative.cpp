#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>


namespace AMD {

static Expression nil;
static Expression identity(new ExpressionTree(
                       "I", nil, nil));

static Expression generateDerivativeExpression(
                           Expression expr, 
                           Expression acc,
                           const char targetMatrix)
{
    switch(expr.info()){
        //tr, apply identity matrix
        case "tr":
            Expression leftAcc(new ExpressionTree(
            "*", acc, identity));
            Expression left = 
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //lgdt, apply inverse transpose of left child
        case "lgdt":
            Expression tempLeftAcc(new ExpressionTree(
            "_", acc, nil));
            Expression leftAcc(new ExpressionTree(
            "'", tempLeftAcc, nil));
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //multiplication, apply acc*right' + left'*acc
        case "*":
            Expression tempLeftAcc(new ExpressionTree(
            "'", right, nil));
            Expression tempRightAcc(new ExpressionTree(
            "'", left, nil));
            Expression leftAcc(new ExpressionTree(
            "*", acc, tempLeftAcc));
            Expression rightAcc(new ExpressionTree(
            "*", tempRightAcc, acc)); 
            Expression left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            Expression right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            break;
        //element wise multiplication, copy over accumulator
        case "o":
            Expression leftAcc(new ExpressionTree(
            "o", right, acc));
            Expression rightAcc(new ExpressionTree(
            "o", acc, left));
            Expression left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            Expression right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            break;
        //inverse match, derivative unknown, for now apply identity
        case "_":
            Expression leftAcc = acc;
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //transpose match, apply transpose
        case "'":
            Expression leftAcc(new ExpressionTree(
            "'", acc, nil));
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //negation match, copy over accumulator after negating
        case "-":
            if (*expr.right()) {
                Expression leftAcc = 
                ExpressionTree::deepCopy(acc);
                Expression rightAcc(new ExpressionTree(
                "-", acc, nil));
            Expression left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            Expression right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            }
            else {
                Expression leftAcc(new ExpressionTree(
                "-", acc, nil));
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            }
            break;
        //its a leaf node match with our target matrix, return accumulator
        case targetMatrix:
            return acc; 
            break;
        //must be a constant expression, ie either a double or a constant
        //matrix which can be ignored, hence return null
        default:
            return nil;
            break;
    }
}

///<Recursive function to generate the derivative expression tree for a given
// expression tree
// Essentially evaluates current node to determine which derivative rule to
// apply to children, calculates new accumulator value for left and right 
// children, calls recursive method on left and right children,
// then when recursive calls return, returns sum of left and right return
// value
// Base case for ending recursion is when it hits a leaf node consisting of
// either a constant matrix/double, or the targetMatrix
/// @param[in] expr  ExpressionTree to take the derivative of, noet
//  must have a trace or log det node at the top of the tree to be valid
/// @param[in] acc    ExpressionTree representing the accumulated derivative
//  initial value when starting derivative should be Identity matrix always
/// @param[in] targetMatrix Char value representing what we're 
//  taking the derivative with respect to, used to determine which matrices
//  are considered a constant matrix and which are considered variables

Expression addExpr(
            Expression left, 
            Expression right)
{
    Expression parent(
                       new ExpressionTree("+",left,right));
    return parent;
}
///< Joins the left and right expression accumulations with a plus operation
/// @param[in] left The ExpressionTree representing the left accumulator
/// @param[in] right The ExpressionTree representing the right accumulator
//  @return An expressionTree composed of the two inputs combined with a 
//  plus operation node

} //End namespace
