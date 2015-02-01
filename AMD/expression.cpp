#include <AMD/expression.hpp>

namespace AMD {

Expression generateExpression(const std::string& exprString)
{
    detail::MatrixGrammar<std::string::const_iterator> myParser;
    Expression myExpr;
    std::string::const_iterator iter = exprString.begin();                         
    std::string::const_iterator end  = exprString.end();
    bool result = phrase_parse(iter, 
                               end, 
                               myParser, 
                               boost::spirit::ascii::space, 
                               myExpr);
    
    // FIXME: Should exceptions be typed?
    if (!result) {
        throw std::runtime_error("Parsing failed");
    } else if (iter != end) {
        throw std::runtime_error("Parsing failed at: " + 
                                  std::string(iter, end));
    }

    return myExpr;
}

} // namespace AMD