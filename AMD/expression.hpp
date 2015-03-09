/// @file expression.hpp
/// Contains AMD::Parser

#ifndef AMD_EXPRESSION_HPP
#define AMD_EXPRESSION_HPP

#include <string>
#include <stdexcept>

#include <boost/spirit/include/qi.hpp>

#include <AMD/expressiontree.hpp>
#include <AMD/matrixgrammar.hpp>
#include <AMD/log.hpp>

namespace AMD {


// Define the expression type to be utree for now.
typedef boost::shared_ptr<detail::ExpressionTree> Expression;
///< typedef for the expression tree that is parsed by Parser

std::ostream& operator<<(std::ostream& os, const Expression& e);
///< print an Expression to an output stream
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
bool compareExpectedExpressions(const std::string& exprString, 
                                const std::string& trueParsedString);
///< Takes in a string, attempts to parse it, then compares
///  printed output to what is the true parsed string value
///  to determine if parsing works correctly on given exprString

} // namespace AMD

#endif // AMD_EXPRESSION_HPP
