#include <AMD/expression.hpp>
#include <sstream>
#include <string.h>
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

bool compareExpectedExpressions(const std::string& exprString, 
                                const std::string& trueParsedString)
{
    std::ostringstream stream;
    try {
        AMD::Expression myExpr = AMD::generateExpression(exprString);
        stream << *myExpr;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    std::string parsedString = stream.str();
    return !(parsedString.compare(trueParsedString));
}


std::ostream& operator<<(std::ostream& os, const Expression& e)
{
    std::cout << "Here" << std::endl;
    e->print(os);
    return os;
}

} // namespace AMD
