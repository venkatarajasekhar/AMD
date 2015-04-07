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

boost::shared_ptr<ExpressionTree> generateDerivativeExpression(
                           boost::shared_ptr<ExpressionTree> expr, 
                           boost::shared_ptr<ExpressionTree> acc,
                           const char targetMatrix)
{
    switch(expr.info()){
        //tr, apply identity matrix
        case "tr":
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "*", acc, identity));
            boost::shared_ptr<ExpressionTree> rightAcc = nil;
            break;
        //lgdt, apply inverse transpose of left child
        case "lgdt":
            boost::shared_ptr<ExpressionTree> tempLeftAcc(new ExpressionTree(
            "_", acc, nil));
            boost::shared_ptr<ExpressionTree> leftAcc(new ExpressionTree(
            "'", tempLeftAcc, nil));
            boost::shared_ptr<ExpressionTree> rightAcc = nil;
            break;
        //multiplication, apply acc*right' + left'*acc
        case "*":
            break;
        //element wise multiplication, copy over accumulator
        case "o":
            boost::shared_ptr<ExpressionTree> leftAcc = acc;
            boost::shared_ptr<ExpressionTree> rightAcc = acc;
            
            break;
        //inverse match, derivative unknown, for now apply identity
        case "_":
            break;
        //transpose match, apply transpose
        case "'":
            break;
        //negation match, copy over accumulator after negating
        case "-":
            break;
        //its a leaf node match with our target matrix, return accumulator
        case targetMatrix:
            break;
        //must be a constant expression, ie either a double or a constant
        //matrix which can be ignored, hence return null
        default:
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
