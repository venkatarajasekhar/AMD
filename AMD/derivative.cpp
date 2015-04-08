#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>
#include <AMD/derivative.hpp>
#include <AMD/expressiontree.hpp>

namespace AMD {

typedef detail::ExpressionTree ExpressionTree;
//typedef boost::shared_ptr<AMD::detail::Tree> Expression2;




static Expression nil;
static Expression identity(new ExpressionTree(
                       "I", nil, nil));


static Expression generateDerivativeExpression(
                           Expression expr, 
                           Expression acc,
                           const std::string targetMatrix)
{
   // switch((*expr).info()){
        //tr, apply identity matrix
        ExpressionTree tree = *expr;
        if (tree.info() == "tr") {
            Expression leftAcc(new ExpressionTree(
            "*", acc, identity));
            return generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
       } //    break;
        //lgdt, apply inverse transpose of left child
       else if (tree.info() ==  "lgdt"){
            Expression tempLeftAcc(new ExpressionTree(
            "_", acc, nil));
            Expression leftAcc(new ExpressionTree(
            "'", tempLeftAcc, nil));
            return generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
        } //   break;
        //multiplication, apply acc*right' + left'*acc
       else if (tree.info() ==  "*"){
            Expression tempLeftAcc(new ExpressionTree(
            "'", tree.right(), nil));
            Expression tempRightAcc(new ExpressionTree(
            "'", tree.left(), nil));
            Expression leftAcc(new ExpressionTree(
            "*", acc, tempLeftAcc));
            Expression rightAcc(new ExpressionTree(
            "*", tempRightAcc, acc)); 
            Expression left =
             generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
            Expression right =
             generateDerivativeExpression(tree.right(), leftAcc, targetMatrix); 
            return addExpr(left, right);
        }//    break;
        //element wise multiplication, copy over accumulator
        else if (tree.info() == "o"){
            Expression leftAcc(new ExpressionTree(
            "o", tree.right(), acc));
            Expression rightAcc(new ExpressionTree(
            "o", acc, tree.left()));
            Expression left =
             generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
            Expression right =
             generateDerivativeExpression(tree.right(), leftAcc, targetMatrix); 
            return addExpr(left, right); 
        } //    break;
        //inverse match, derivative unknown, for now apply identity
        else if (tree.info() ==  "_") {
            Expression leftAcc = acc;
            return generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
        } //   break;
        //transpose match, apply transpose
       else if (tree.info() == "'"){
            Expression leftAcc(new ExpressionTree(
            "'", acc, nil));
            return generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
       }  //   break;
        //negation match, copy over accumulator after negating
       else if (tree.info() == "-"){
            if (tree.right()) {
                Expression leftAcc = acc;
                Expression rightAcc(new ExpressionTree(
                "-", acc, nil));
                Expression left =
                generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
                Expression right =
                generateDerivativeExpression(tree.right(), leftAcc, targetMatrix); 
                return addExpr(left, right); 
            }
            else {
                Expression leftAcc(new ExpressionTree(
                "-", acc, nil));
                return generateDerivativeExpression(tree.left(), leftAcc, targetMatrix); 
            }
        }
         //   break;
        //its a leaf node match with our target matrix, return accumulator
    else if (tree.info() == targetMatrix){
            return acc; 
    }
          //  break;
        //must be a constant expression, ie either a double or a constant
        //matrix which can be ignored, hence return null
      //  default:
     else {
                return nil;
       } //    break;
    //}
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

static Expression addExpr(
            Expression& left, 
            Expression& right)
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
