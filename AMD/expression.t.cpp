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
    BOOST_CHECK(compareExpectedExpressions("-+-A", "(- (- \"A\"))"));   
   
    
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
    // BOOST_CHECK(compareExpectedExpressions("tr(A)*tr(B)+7*4*A", 
    //         "(+ (* (tr \"A\") (tr \"B\")) (* (* \"7\" \"4\") \"A\"))"));
    
    //testing negation priority  
    // BOOST_CHECK(compareExpectedExpressions("-A'+tr(lgdt(A*B/C))", 
    //         "(+ (- (' \"A\")) (tr (lgdt (/ (* \"A\" \"B\") \"C\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-A'+A","(+ (- (' \"A\")) \"A\")")); 

    //TODO: Confirm division supported as well as associativity of ops
    // BOOST_CHECK(compareExpectedExpressions("--A'+tr(lgdt(A*B/C))", 
    //         "(+ (- (- (' \"A\"))) (tr (lgdt (/ (* \"A\" \"B\") \"C\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-B*C'", 
            "(* (- \"B\") (' \"C\"))"));
    BOOST_CHECK(compareExpectedExpressions("-(tr(B)*tr(B)+-tr(B))", 
            "(- (+ (* (tr \"B\") (tr \"B\")) (- (tr \"B\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-A'*(B+((C)))", 
            "(* (- (' \"A\")) (+ \"B\" \"C\"))")); 
    BOOST_CHECK(compareExpectedExpressions("-(-A+-B)", 
            "(- (+ (- \"A\") (- \"B\")))")); 
   
    //check associativity of subtraction
    BOOST_CHECK(compareExpectedExpressions("A+B-C", 
            "(- (+ \"A\" \"B\") \"C\")")); 
    BOOST_CHECK(compareExpectedExpressions("A+B-C-D-E", 
            "(- (- (- (+ \"A\" \"B\") \"C\") \"D\") \"E\")")); 
} 

bool RRTestHelper(const std::string& inputString, 
                  const std::string& expectedString)
{
    // toRightRecursiveRep test helper

    bool ret = false;
    AMD_START_TRY_BLOCK()
    ret = AMD::toRightRecursiveRep(inputString) == expectedString;
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    return ret;
}

BOOST_AUTO_TEST_CASE ( toRightRecursiveRep )
{

    // Same string excpected
    BOOST_CHECK(RRTestHelper("", ""));
    BOOST_CHECK(RRTestHelper("A+B", "A+B"));

    // one transformation expected
    BOOST_CHECK(RRTestHelper("A'", "(trans(A))"));
    BOOST_CHECK(RRTestHelper("A_", "(inv(A))"));

    // one transformation expected on right term
    BOOST_CHECK(RRTestHelper("A+B'", "A+(trans(B))"));
    BOOST_CHECK(RRTestHelper("A+B_", "A+(inv(B))"));

    // one transformation expected on whole expr
    BOOST_CHECK(RRTestHelper("(A+B)'", "(trans((A+B)))"));
    BOOST_CHECK(RRTestHelper("(A+B)_", "(inv((A+B)))"));

    // double transformation expected on right term
    BOOST_CHECK(RRTestHelper("A+B'_", "A+(inv((trans(B))))"));
    BOOST_CHECK(RRTestHelper("A+B_'", "A+(trans((inv(B))))"));

    // double transformation expected on whole expr
    BOOST_CHECK(RRTestHelper("(A+B)'_", "(inv((trans((A+B)))))"));
    BOOST_CHECK(RRTestHelper("(A+B)_'", "(trans((inv((A+B)))))"));

    // Expect an exception printed to output
    BOOST_CHECK(!RRTestHelper("A+B)'", "???"));

}

bool MPTestHelper(const std::string& inputString,
                  int inputIndex,
                  int expectedOutput)
{
    // findMatchingParen test helper

    bool ret = false;
    AMD_START_TRY_BLOCK()
    ret = AMD::findMatchingParen(inputString, inputIndex) == expectedOutput;
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    return ret;
}

BOOST_AUTO_TEST_CASE ( findMatchingParen )
{

    // Simple
    BOOST_CHECK(MPTestHelper("()", 1, 0));
    BOOST_CHECK(MPTestHelper("(A)", 2, 0));
    BOOST_CHECK(MPTestHelper("B+(A)", 4, 2));

    // Nested Structure
    BOOST_CHECK(MPTestHelper("(((((-(A+B))))))", 15, 0));
    BOOST_CHECK(MPTestHelper("(((((-(A+B))))))", 14, 1));
    BOOST_CHECK(MPTestHelper("(((((-(A+B))))))", 13, 2));
    BOOST_CHECK(MPTestHelper("(((((-(A+B))))))", 12, 3));
    BOOST_CHECK(MPTestHelper("(((((-(A+B))))))", 11, 4));

    // Degenrate structure
    // Expect an exception printed to output
    BOOST_CHECK(!MPTestHelper("A_ + B)))))", 10, 0));

}

bool validateExprTestHelper(const std::string& exprString, char expectedOutput)
{
    char post_process_result;
    AMD::detail::MatrixGrammar<std::string::const_iterator> myParser;
    AMD::Expression myExpr;
    AMD_START_TRY_BLOCK()
    std::string exprStringRR = AMD::toRightRecursiveRep(exprString);
    
    std::string::const_iterator iter = exprStringRR.begin();
    std::string::const_iterator end  = exprStringRR.end();
    bool result = phrase_parse(iter, 
                               end, 
                               myParser, 
                               boost::spirit::ascii::space, 
                               myExpr);
    post_process_result = AMD::validateExpr(myExpr);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    return (post_process_result  == expectedOutput);
}

BOOST_AUTO_TEST_CASE ( validateExpr )
{

    // Matrix Expressions
    BOOST_CHECK(validateExprTestHelper("X", 'M'));
    BOOST_CHECK(validateExprTestHelper("X + A", 'M'));
    BOOST_CHECK(validateExprTestHelper("X * lgdt(A)", 'M'));
    BOOST_CHECK(validateExprTestHelper("X * tr(A)", 'M'));
    BOOST_CHECK(validateExprTestHelper("X o A", 'M'));
    BOOST_CHECK(validateExprTestHelper("X'_", 'M'));
    BOOST_CHECK(validateExprTestHelper("X_'", 'M'));
    BOOST_CHECK(validateExprTestHelper("X'_ * lgdt(A_')", 'M'));

    // Scalar Expressions
    BOOST_CHECK(validateExprTestHelper("1.2", 'S'));
    BOOST_CHECK(validateExprTestHelper("1.2 + 3.14", 'S'));
    BOOST_CHECK(validateExprTestHelper("1.2 * 3.14", 'S'));
    BOOST_CHECK(validateExprTestHelper("1.2 / 3.14", 'S'));
    BOOST_CHECK(validateExprTestHelper("-1.2 - 3.14", 'S'));
    BOOST_CHECK(validateExprTestHelper("-1.2 - tr(-A)", 'S'));
    BOOST_CHECK(validateExprTestHelper("-1.2 * tr(A'_) / lgdt(X_)", 'S'));

    // Invalid Expressions
    BOOST_CHECK(validateExprTestHelper("1.2 + M", 'I'));
    BOOST_CHECK(validateExprTestHelper("tr(1.2)", 'I'));
    BOOST_CHECK(validateExprTestHelper("lgdt(-3.14)", 'I'));
    BOOST_CHECK(validateExprTestHelper("tr(tr(X))", 'I'));
    BOOST_CHECK(validateExprTestHelper("tr(lgdt(X))", 'I'));
    BOOST_CHECK(validateExprTestHelper("lgdt(tr(X))", 'I'));
    BOOST_CHECK(validateExprTestHelper("lgdt(lgdt(X))", 'I'));
    BOOST_CHECK(validateExprTestHelper("tr(M) + M", 'I'));
    BOOST_CHECK(validateExprTestHelper("tr(M) o M", 'I'));

}