/// @file derivative.hpp
/// Contains AMD::Derivative

#ifndef AMD_DERIVATIVE_HPP
#define AMD_DERIVATIVE_HPP
#endif

#include <string>
#include <stdexcept>

#include <boost/spirit/include/qi.hpp>

#include <AMD/exception.hpp>
#include <AMD/expressiontree.hpp>
#include <AMD/matrixgrammar.hpp>
#include <AMD/log.hpp>

namespace AMD {

Expression generateDerivativeExpression(
                           const Expression& expr, 
                           const std::string targetMatrix);
///< Harness for differentiation, takes in an expression and sets up
//   initial prior depending on whether it's a trace or lgdt expression
//   Then calls the recursive helper method to kickoff differentiation
//   @param[in] expr The expression to be differentiated
//   @param[in] targetMatrix The matrix to differentiate with respect to
//   @return A boost shared_ptr to a Tree representing the derivative expr

Expression generateDerivativeExpressionHelper(
                           const Expression& expr, 
                           const Expression& acc,
                           const std::string targetMatrix);
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
//  @return a shared_ptr to a Tree representing the accumulated derivative

static Expression addExpr(Expression& left, Expression& right);
///< Joins the left and right expression accumulations with a plus operation
/// @param[in] left The ExpressionTree representing the left accumulator
/// @param[in] right The ExpressionTree representing the right accumulator
//  @return A shared pointer to a tree composed of the two inputs combined with 
//  a plus operation node


}//End AMD namespace
