#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>
#include <AMD/expressiontree.hpp>

namespace AMD {

typedef detail::ExpressionTree ExpressionTree;


static Expression identity(new ExpressionTree("I", nil, nil));

Expression2 generateDerivativeExpression(
                           const Expression2& expr, 
                           const Expression2& acc,
                           const std::string targetMatrix)
{
        AMD::detail::Tree tree = *expr;
        //tr, apply identity matrix
        if (tree.info() == "tr") {
            Expression2 leftAcc(new ExpressionTree("*", acc, identity));
            return generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
       }
        //lgdt, apply inverse transpose of left child
       else if (tree.info() ==  "lgdt"){
            Expression2 tempLeftAcc(new ExpressionTree("_", acc, nil));
            Expression2 leftAcc(new ExpressionTree("'", tempLeftAcc, nil));
            return generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
        }
        //multiplication, apply acc*right' + left'*acc
       else if (tree.info() ==  "*"){
            Expression2 tempLeftAcc(new ExpressionTree("'", tree.right(), nil));
            Expression2 tempRightAcc(new ExpressionTree("'", tree.left(), nil));
            Expression2 leftAcc(new ExpressionTree("*", acc, tempLeftAcc));
            Expression2 rightAcc(new ExpressionTree("*", tempRightAcc, acc)); 
            Expression2 left = generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
            Expression2 right = generateDerivativeExpression(
                tree.right(), rightAcc, targetMatrix); 
            return addExpr(left, right);
        }
        //element wise multiply,element wise multiply opposite child by acc
        else if (tree.info() == "o"){
            Expression2 leftAcc(new ExpressionTree("o", tree.right(), acc));
            Expression2 rightAcc(new ExpressionTree("o", tree.left(), acc));
            Expression2 left = generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
            Expression2 right = generateDerivativeExpression(
                tree.right(), rightAcc, targetMatrix); 
            return addExpr(left, right); 
        }
        //inverse match, derivative unknown, for now apply identity
        else if (tree.info() ==  "_") {
            Expression2 leftAcc = acc;
            return generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
        }
        //transpose match, apply transpose
       else if (tree.info() == "'"){
            Expression2 leftAcc(new ExpressionTree("'", acc, nil));
            return generateDerivativeExpression(
                tree.left(), leftAcc, targetMatrix); 
       }
        //negation match, check unary/binary, negate/copy + negate respectively
       else if (tree.info() == "-"){
            if (tree.right()) {
                Expression2 leftAcc = acc;
                Expression2 rightAcc(new ExpressionTree("-", acc, nil));
                Expression2 left = generateDerivativeExpression(
                    tree.left(), leftAcc, targetMatrix); 
                Expression2 right = generateDerivativeExpression(
                    tree.right(), rightAcc, targetMatrix); 
                return addExpr(left, right); 
            }
            else {
                Expression2 leftAcc(new ExpressionTree("-", acc, nil));
                return generateDerivativeExpression(
                    tree.left(), leftAcc, targetMatrix); 
            }
        }
        //its a leaf node match with our target matrix, return accumulator
        else if (tree.info() == targetMatrix){
            return acc; 
        }
        //must be a constant expression, ie either a double or a constant
        //matrix which can be ignored, hence return null
        else {
            return zero;
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

static Expression2 addExpr(
            Expression2& left, 
            Expression2& right)
{
    Expression2 parent(
                       new ExpressionTree("+",left,right));
    return parent;
}
///< Joins the left and right expression accumulations with a plus operation
/// @param[in] left The ExpressionTree representing the left accumulator
/// @param[in] right The ExpressionTree representing the right accumulator
//  @return An expressionTree composed of the two inputs combined with a 
//  plus operation node

} //End namespace
