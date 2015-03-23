#include <sstream>
#include <string.h>

#include <AMD/expression.hpp>

namespace AMD {

Expression generateExpression(const std::string& exprString)
{
    detail::MatrixGrammar<std::string::const_iterator> myParser;
    Expression myExpr;

    LOG_INFO << "Generating expression from " << exprString;

    // Expression string with right recursive grammar
    std::string exprStringRR = toRightRecursiveRep(exprString);
    
    std::string::const_iterator iter = exprStringRR.begin();
    std::string::const_iterator end  = exprStringRR.end();
    bool result = phrase_parse(iter, 
                               end, 
                               myParser, 
                               boost::spirit::ascii::space, 
                               myExpr);

    if (!result) {
        LOG_ERROR << "Parsing failed";
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from ExpressionTree constructor"),
                    "Parsing failed",
                    AMD_INVALID_EXPRESSION);

    } else if (iter != end) {
        LOG_ERROR << ("Parsing failed at: " + std::string(iter, end));
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from ExpressionTree constructor"),
                    ("Parsing failed at: " + std::string(iter, end)).c_str(),
                    AMD_INVALID_EXPRESSION);
    }

    LOG_TRACE << "Finished generating expression";

    return myExpr;
}

std::string toRightRecursiveRep(const std::string& exprString)
{

    
    LOG_TRACE << "Preprocessing exprString " << __LINE__ << " "<< __FILE__;
    std::size_t first_inv = exprString.find("_");
    std::size_t first_trans = exprString.find("\'");

    if(first_inv != std::string::npos || first_trans != std::string::npos)
    {
        // The index of the first operator
        int first = std::min(first_inv, first_trans);

        // get begin and end indices of the operand substring
        int begin;
        int end = first;
        if(exprString[first - 1] == ')')
            begin = findMatchingParen(exprString, first - 1);
        else
            begin = first - 1;

        std::string op_str;
        if(exprString[first] == '\'')
            op_str = "(trans(" + exprString.substr(begin, end - begin) + "))";
        else if(exprString[first] == '_')
            op_str = "(inv(" + exprString.substr(begin, end - begin) + "))";

        std::string prefix = exprString.substr(0, begin);
        std::string suffix = exprString.substr(end + 1, std::string::npos);

        return toRightRecursiveRep(prefix + op_str + suffix);
    }
    else{
        LOG_TRACE << "Preprocessing exprString returning base case" << 
            __LINE__ << " "<< __FILE__;
        return exprString;
    }
}

int findMatchingParen(const std::string& exprString, int index)
{
    LOG_TRACE << "finding Matching Parenthesis " << __LINE__ << " "<< __FILE__;
    int matching = -1;
    int open_paren_count = 0;
    int close_paren_count = 1;

    // Start from the close parenthesis and scan to the left
    for(int i = index - 1; i >= 0; i--){
        if(exprString[i] == ')')
            close_paren_count += 1;
        else if(exprString[i] == '(')
            open_paren_count += 1;
        if(open_paren_count - close_paren_count == 0){ // arrived at the match
            matching = i;
            break;
        }
    }
    if(matching == -1){
        // If no match is found throw an exception
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from ExpressionTree findMatchingParen"),
                    "Expression has unmatched parentheses",
                    AMD_INVALID_EXPRESSION);
    }
    return matching;
}

std::ostream& operator<<(std::ostream& os, const Expression& e)
{
    LOG_TRACE << "Printing Expression";
    e->print(os);
    return os;
}

} // namespace AMD
