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

std::string toRightRecursiveRep(const std::string& exprString);
///<Transfroms an exprString from containing the operator shorthands ' and _
/// into an equivalent expression using the notation (trans( )) (inv( )).
/// @param[in] exprString This is a string that represents a matrix expression
/// @param[out] std::string A string which has been preprocessed to
/// wrap all transpose inverse operations explicitly with parenthesis
///  ex: "-(A+B)' + A_"  -> -(trans(A+B)) + (inv(A))

int findMatchingParen(const std::string& exprString, int index);
///<Given a matrix string expression and an index to an unmatched closed
/// parenthesis, this function will return the correct index for the matching
/// open parenthesis if it exists, else an exception
/// @param[in] exprString The string representing the matrix expression which
/// contains a closed parenthesis
/// @param[in] index An integer representing the index of a closed parenthesis
/// inside the string input exprString
/// @param[out] int The index of the matching open parenthesis if it exists
/// else an exception will be thrown

bool compareExpectedExpressions(const std::string& exprString, 
                                const std::string& trueParsedString);
///< Takes in a string, attempts to parse it, then compares
///  printed output to what is the true parsed string value
///  to determine if parsing works correctly on given exprString
///  @param[in] exprString A string representing a matrix expression to be
///  parsed as a test of the parser
///  @param[in] trueParsedString The string representation of the
///  expressionTree which represents the correct parsed value of the matrix
///  expression
///  @param[out] Bool A boolean value representing whether the parser correctly
///  parsed the exprString and had results exactly the same as the 
///  trueParsedString

} // namespace AMD

#endif // AMD_EXPRESSION_HPP
