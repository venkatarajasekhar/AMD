#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>

namespace AMD {

boost::shared_ptr<ExpressionTree> generateDerivativeExpression(
                           boost::shared_ptr<ExpressionTree> expr, 
                           boost::shared_ptr<ExpressionTree> acc,
                           const char targetMatrix)
{

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

boost::shared_ptr<ExpressionTree> generateNewAccumulator(
                    boost::shared_ptr<ExpressionTree> prevAcc, 
                    boost::shared_ptr<ExpressionTree> left, 
                    boost::shared_ptr<ExpressionTree> right, 
                    boost::shared_ptr<ExpressionTree> current, 
                    int side)
{
    


}
///< Method that takes in current execution state and determines what the new
//   accumulator value should be by using a switch statement to evaluate which
     rule to apply
/// @param[in] prevAcc ExpressionTree for previous accumulator
/// @param[in] left ExpressionTree for left child
/// @param[in] right ExpressionTree for right child
/// @param[in] current ExpressionTree for current node, used to check which
//  rule should be applied
/// @param[in] side used to determine which side's accumulator value should
//   be calculated, the left child's or right child's
/// @return ExpressionTree representing new accumulator



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
