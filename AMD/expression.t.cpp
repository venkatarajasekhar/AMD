#include <AMD/expression.hpp>

#define BOOST_TEST_MODULE ExpressionTest
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE ( parseExpression )
{ 
    BOOST_CHECK(AMD::compareExpectedExpressions("A", "\"A\"") == 1);
    BOOST_CHECK(AMD::compareExpectedExpressions("A+B", "(+ \"A\" \"B\")")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("A-B", "(- \"A\" \"B\")")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("A*B", "(* \"A\" \"B\")")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("A-B+C", 
            "(+ (- \"A\" \"B\") \"C\")")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("AoB", "(o \"A\" \"B\")")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("tr(B)", "(tr \"B\")"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("lgdt(B)", "(lgdt \"B\")"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("B'", "(' \"B\")"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("B_", "(_ \"B\")"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("A+B'", "(+ \"A\" (' \"B\"))"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("-B'", "(- (' \"B\"))") == 1);     
    BOOST_CHECK(AMD::compareExpectedExpressions("-A'/-B'", 
            "(/ (- (\' \"A\")) (- (\' \"B\")))"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("tr(A)*tr(B)", 
            "(* (tr \"A\") (tr \"B\"))")); 
    BOOST_CHECK(AMD::compareExpectedExpressions("tr(A)*tr(B)+7*4*A", 
            "(+ (* (tr \"A\") (tr \"B\")) (* (* \"7\" \"4\") \"A\"))"));  
    BOOST_CHECK(AMD::compareExpectedExpressions("B''", "(' (' \"B\"))"));     
    BOOST_CHECK(AMD::compareExpectedExpressions("-B'_", "(- (_ (' \"B\")))"));     
} 
