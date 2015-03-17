#include <AMD/expression.hpp>

#define BOOST_TEST_MODULE ExpressionTest
#include <boost/test/unit_test.hpp>


bool compareExpectedExpressions(const std::string& exprString, 
                                const std::string& trueParsedString)
{
    std::ostringstream stream;
    AMD_START_TRY_BLOCK()
    AMD::Expression myExpr = AMD::generateExpression(exprString);
    stream << *myExpr;
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    std::string parsedString = stream.str();
    return !(parsedString.compare(trueParsedString));
}

//Do we still need this if we have the regression test?
BOOST_AUTO_TEST_CASE ( generateExpression )
{
    AMD::Expression testExpression;

    // This should generate an exception as we do not implement A'_. When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("A'_")));

    // This should generate an exception as we do not implement A_'. When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("A_'")));

    // This should generate an exception as we do not implement A o B. When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("A o B")));

    
    // This should generate an exception as we do not implement tr(A). When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("tr(A)")));
    
    // This should generate an exception as we do not implement lgdt(A). When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression=AMD::generateExpression("lgdt(A)")));
}


/*  Regression test that checks whether the parser is functioning correctly
 *  compareExpectedExpressions takes in two input strings which represent
 *  the input to parse followed by the true parsed string to compare against
 */

BOOST_AUTO_TEST_CASE ( parseExpression )
{ 
    
    //single matrix test
    BOOST_CHECK(compareExpectedExpressions("A", "\"A\"") == 1);
    
    //binary op expressions
    BOOST_CHECK(compareExpectedExpressions("A+B", "(+ \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("A-B", "(- \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("A*B", "(* \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("AoB", "(o \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("A/B", "(/ \"A\" \"B\")")); 
    
    //multiple expressions
    BOOST_CHECK(compareExpectedExpressions("A-B+C", 
            "(+ (- \"A\" \"B\") \"C\")")); 
    
    //unary op expressions
    BOOST_CHECK(compareExpectedExpressions("tr(B)", "(tr \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("lgdt(B)", "(lgdt \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("B'", "(' \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("B_", "(_ \"B\")"));     
   

    //tricky unary op combinations
    BOOST_CHECK(compareExpectedExpressions("-B'", "(- (' \"B\"))") == 1);     
    BOOST_CHECK(compareExpectedExpressions("B''", "(' (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("-B'_", "(- (_ (' \"B\")))"));     
   
    
    //combination of binary and unary op expressions
    BOOST_CHECK(compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("A+B'", "(+ \"A\" (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("-A'/-B'", 
            "(/ (- (\' \"A\")) (- (\' \"B\")))"));     
    
    //complex tr expressions
    BOOST_CHECK(compareExpectedExpressions("tr(A)*tr(B)", 
            "(* (tr \"A\") (tr \"B\"))")); 
    BOOST_CHECK(compareExpectedExpressions("tr(A)*tr(B)+7*4*A", 
            "(+ (* (tr \"A\") (tr \"B\")) (* (* \"7\" \"4\") \"A\"))"));
    
    //testing negation priority  
    BOOST_CHECK(compareExpectedExpressions("-A'+tr(lgdt(A*B/C))", 
            "(+ (- (' \"A\") (tr (lgdt (/ (* \"B\" \"C\") \"D\")))))"));  
    
    //testing negation priority  
    BOOST_CHECK(compareExpectedExpressions("-A'+tr(lgdt(A*B/C))", 
            "(+ (- (' \"A\") (tr (lgdt (/ (* \"B\" \"C\") \"D\")))))"));
    
    BOOST_CHECK(compareExpectedExpressions("--A'+tr(lgdt(A*B/C))", 
            "(+ ( - (- (' \"A\")) (tr (lgdt (/ (* \"B\" \"C\") \"D\")))))"));
    
    BOOST_CHECK(compareExpectedExpressions("-B*C'", 
            "(* (- \"B\") (' \"C\"))"));

    BOOST_CHECK(compareExpectedExpressions("-(tr(B)*tr(B)+-tr(B))", 
            "(- (+ (* (tr \"B\") (tr \"B\")) (- (tr \"B\"))))"));

} 
