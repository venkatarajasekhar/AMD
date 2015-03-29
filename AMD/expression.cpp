#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  


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
    char post_process_result = validateExpr(myExpr);

    if (!result) {
        LOG_ERROR << "Parsing failed";
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from generateExpression"),
                    "Parsing failed",
                    AMD_INVALID_EXPRESSION);

    } else if (iter != end) {
        LOG_ERROR << ("Parsing failed at: " + std::string(iter, end));
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from generateExpression"),
                    ("Parsing failed at: " + std::string(iter, end)).c_str(),
                    AMD_INVALID_EXPRESSION);
    } else if (post_process_result == 'I'){
        LOG_ERROR << "Parsing failed, Invalid operand dimensions";
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from generateExpression"),
                    "Parsing failed, Invalid operand dimensions",
                    AMD_INVALID_EXPRESSION);
    }

    LOG_TRACE << "Finished generating expression";

    return myExpr;
}

char validateExpr(boost::shared_ptr<detail::Tree> myExpr)
{
    detail::Tree root = *myExpr;

    // leaf node
    if(!root.left() && !root.right()){ 
        return (isupper(root.info().c_str()[0]) ? 'M' : 'S');
    }
    // unary op (+, -, tr, lgdt, _, '')
    else if(root.left() && !root.right()){ 
        // validate left
        char L = validateExpr(root.left());

        if(root.info() == "+" || root.info() == "-")
            return L;
        else if(root.info() == "tr" || root.info() == "lgdt")
            return (L == 'M' ? 'S' : 'I');
        else if(root.info() == "_" || root.info() == "'")
            return (L == 'M' ? 'M' : 'I');
    }
    // binary op (+, -, o, *, /)
    else if(root.left() && root.right()){
        // validate left and right
        char L = validateExpr(root.left());
        char R = validateExpr(root.right());
        if(root.info() == "+" || root.info() == "-" || root.info() == "o")
            return (L == R && L != 'I' ? L : 'I');
        else if(root.info() == "*" || root.info() == "/") // FIXME: case S/M
            return (R != 'I' && L != 'I' ? (R == 'M' || L == 'M' ? 'M' : 'S')
                    : 'I');
    }
    else{
         LOG_ERROR << "Postprocessing failed, Invalid tree";
    }
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
                    APPEND_LOCATION("from findMatchingParen"),
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
