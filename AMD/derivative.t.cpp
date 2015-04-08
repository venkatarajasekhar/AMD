#include <AMD/derivative.hpp>

#define BOOST_TEST_MODULE DerivativeTest
#include <boost/test/unit_test.hpp>

bool compareDerivativeExpressions(const std::string& exprString, 
                                const std::string& trueParsedString)
{
    std::ostringstream stream;
    AMD_START_TRY_BLOCK()
    AMD::Expression myExpr = AMD::generateDerivativeExpression(exprString);
    stream << *myExpr;
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    std::string parsedString = stream.str();
    return !(parsedString.compare(trueParsedString));
}


BOOST_AUTO_TEST_CASE ( derivatives )
{
    typedef class AMD::detail::ExpressionTree ExpressionTree;
    boost::shared_ptr<ExprTree> nil;
    
    // Test: Simple operations
    BOOST_CHECK (compareDerivativeExpressions("tr(X')","\"I\"")) 
    BOOST_CHECK (compareDerivativeExpressions("lgdt(X')","(_ \"X\")")) 
    BOOST_CHECK (compareDerivativeExpressions("tr(X+X)","(+ \"I\" \"I\")")) 
    BOOST_CHECK (compareDerivativeExpressions("tr(X-X)","(- \"I\" \"I\")")) 
    BOOST_CHECK (compareDerivativeExpressions("tr(X*X)","(+ (* \"I\" (' \"X\")) (* (' \"X\") \"I\"))")) 
    BOOST_CHECK (compareDerivativeExpressions("tr(XoX)","(+ (o \"X\" \"I\") (o \"X\" \"I\"))"))
}
