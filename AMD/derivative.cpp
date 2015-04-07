#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>


namespace AMD {

static boost::shared_ptr<ExpressionTree> nil;
static boost::shared_ptr<ExpressionTree> identity(new ExpressionTree(
                       "I", nil, nil));

static boost::shared_ptr<ExpressionTree> generateDerivativeExpression(
                           boost::shared_ptr<ExpressionTree> expr, 
                           boost::shared_ptr<ExpressionTree> acc,
                           const char targetMatrix)
{
    switch(expr.info()){
        //tr, apply identity matrix
        case "tr":
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "*", acc, identity));
            boost::shared_ptr<ExpressionTree> left = 
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //lgdt, apply inverse transpose of left child
        case "lgdt":
            boost::shared_ptr<ExpressionTree> tempLeftAcc(new ExpressionTree(
            "_", acc, nil));
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "'", tempLeftAcc, nil));
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //multiplication, apply acc*right' + left'*acc
        case "*":
            boost::shared_ptr<ExpressionTree> tempLeftAcc(new ExpressionTree(
            "'", right, nil));
            boost::shared_ptr<ExpressionTree> tempRightAcc(new ExpressionTree(
            "'", left, nil));
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "*", acc, tempLeftAcc));
            boost::shared_ptr<ExpressionTree> rightAcc(new ExpressionTree(
            "*", tempRightAcc, acc)); 
            boost::shared_ptr<ExpressionTree> left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            boost::shared_ptr<ExpressionTree> right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            break;
        //element wise multiplication, copy over accumulator
        case "o":
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "o", right, acc));
            boost::shared_ptr<ExpressionTree> rightAcc(new ExpressionTree(
            "o", acc, left));
            boost::shared_ptr<ExpressionTree> left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            boost::shared_ptr<ExpressionTree> right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            break;
        //inverse match, derivative unknown, for now apply identity
        case "_":
            boost::shared_ptr<ExpressionTree> leftAcc = acc;
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //transpose match, apply transpose
        case "'":
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "'", acc, nil));
            return generateDerivativeExpression(left, leftAcc, targetMatrix); 
            break;
        //negation match, copy over accumulator after negating
        case "-":
            if (*expr.right()) {
                boost::shared_ptr<ExpressionTree> leftAcc = 
                ExpressionTree::deepCopy(acc);
                boost::shared_ptr<ExpressionTree> rightAcc(new ExpressionTree(
                "-", acc, nil));
            boost::shared_ptr<ExpressionTree> left =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            boost::shared_ptr<ExpressionTree> right =
             generateDerivativeExpression(left, leftAcc, targetMatrix); 
            return addExpr(left, right) 
            }
            else {
                boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
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

boost::shared_ptr<ExpressionTree> addExpr(
            boost::shared_ptr<ExpressionTree> left, 
            boost::shared_ptr<ExpressionTree> right)
{
    boost::shared_ptr<ExpressionTree> parent(
                       new ExpressionTree("+",left,right));
    return parent;
}
///< Joins the left and right expression accumulations with a plus operation
/// @param[in] left The ExpressionTree representing the left accumulator
/// @param[in] right The ExpressionTree representing the right accumulator
//  @return An expressionTree composed of the two inputs combined with a 
//  plus operation node

} //End namespace
