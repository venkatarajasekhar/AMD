/// @file expression.hpp
/// Contains AMD::Parser

#ifndef AMD_EXPRESSION_HPP
#define AMD_EXPRESSION_HPP

#include <string>
#include <stdexcept>

#include <boost/spirit/include/qi.hpp>

#include <AMD/exception.hpp>
#include <AMD/expressiontree.hpp>
#include <AMD/matrixgrammar.hpp>
#include <AMD/log.hpp>

namespace AMD {

enum ExpressionType { INVALID=-1, SCALAR, MATRIX };
///< An enumeration that tells us the type of expression that was input

// Define the expression type to be utree for now.
typedef boost::shared_ptr<detail::ExpressionTree> Expression;
///<Typedef for the expression tree that is parsed by Parser

std::ostream& operator<<(std::ostream& os, const Expression& e);
///<Print an Expression to an output stream
/// @param[in] os   output stream such as std::cout
/// @param[in] e    that which we need to print

Expression generateExpression(const std::string& exprString);
///< The main method of the file that takes in a string and tries to parse it.
/// @param[in] exprString This is a string that represents a matrix expression
/// @code
/// 
///  std::string str = '-A*B'';
///  try {
///     AMD::Expression myExpr = AMD::generateExpression(str);
///     std::cout << "Parsing succeeded: " << myExpr << "\n";
///     } 
///  catch (const std::exception& e) {
///     std::cout << e.what() << std::endl;
///     }
/// @endcode

ExpressionType validateExpr(const boost::shared_ptr<detail::Tree>& expr);
///< Check the validity of the types of the operands so that expressions that 
///  attempts to incorrectly combine matricies and scalars can be identified.
/// @param[in] expr The Expression representing the matrix expression tree
///                 returned by the parser
/// @return SCALAR, MATRIX, or INVALID.

void postProcess(const boost::shared_ptr<detail::Tree>& expr);
///< Check that this expression is a valid expression (correct types of 
/// operands, scalar tr() or ldgt() function)
/// @param[in] expr The expression representing the matrix expression tree
///                 returned by the parser          

std::string toRightRecursiveRep(const std::string& exprString);
///< Transforms an exprString from containing the operator shorthands ' and _
/// into an equivalent expression using the notation (trans( )) (inv( )).
///
/// @param[in] exprString This is a string that represents a matrix expression
/// @return A string which has been preprocessed to wrap all transpose inverse 
///         operations explicitly with parenthesis
///         eg: "-(A+B)' + A_"  -> -(trans(A+B)) + (inv(A))

int findMatchingParen(const std::string& exprString, int index);
///< Given a matrix string expression and an index to an unmatched closed
/// parenthesis, this function will return the correct index for the matching
/// open parenthesis if it exists, else an exception
///
/// @param[in] exprString The string representing the matrix expression which
///                       contains a closed parenthesis
/// @param[in] index An integer representing the index of a closed parenthesis
///                  inside the string input exprString
/// @return The index of the matching open parenthesis if it exists

std::string preProcess(const std::string& exprString);
///< Performs preprocessing on exprString.
/// Transforms an exprString from containing the operator shorthands ' and _
/// into an equivalent expression using the notation (trans( )) (inv( )).
///
/// @param[in] exprString This is a string that represents a matrix expression
/// @return A string which has been preprocessed to wrap all transpose inverse 
///         operations explicitly with parenthesis
///         eg: "-(A+B)' + A_"  -> -(trans(A+B)) + (inv(A)) 


} // namespace AMD

#endif // AMD_EXPRESSION_HPP
