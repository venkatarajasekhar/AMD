#include <AMD/expression.hpp>

#define BOOST_TEST_MODULE ExpressionTest
#include <boost/test/unit_test.hpp>

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

bool validateExprTestHelper(const std::string& exprString, 
                            AMD::ExpressionType expectedOutput)
{
    AMD::ExpressionType validateResult;
    AMD::detail::MatrixGrammar<std::string::const_iterator> myParser;
    AMD::Expression myExpr;
    std::string exprStringRR = AMD::toRightRecursiveRep(exprString);
    
    std::string::const_iterator iter = exprStringRR.begin();
    std::string::const_iterator end  = exprStringRR.end();
    bool result = phrase_parse(iter, 
                               end, 
                               myParser, 
                               boost::spirit::ascii::space, 
                               myExpr);
    validateResult = AMD::validateExpr(myExpr);

    return (validateResult  == expectedOutput);
}

BOOST_AUTO_TEST_CASE ( validateExpr )
{

    // Matrix Expressions
    BOOST_CHECK(validateExprTestHelper("X", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X + A", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X * lgdt(A)", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X * tr(A)", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X o A", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X'_", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X_'", AMD::MATRIX));
    BOOST_CHECK(validateExprTestHelper("X'_ * lgdt(A_')", AMD::MATRIX));

    // Scalar Expressions
    BOOST_CHECK(validateExprTestHelper("1.2", AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("1.2 + 3.14", AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("1.2 * 3.14", AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("1.2 / 3.14", AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("-1.2 - 3.14",AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("-1.2 - tr(-A)", AMD::SCALAR));
    BOOST_CHECK(validateExprTestHelper("-1.2 * tr(A'_) * lgdt(X_)", AMD::SCALAR));

    // Invalid Expressions
    BOOST_CHECK_THROW(validateExprTestHelper("1.2 + M", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("tr(1.2)", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("lgdt(-3.14)", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("tr(tr(X))", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("tr(lgdt(X))", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("lgdt(tr(X))", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("lgdt(lgdt(X))", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("tr(M) + M", AMD::INVALID), AMD::Exception);
    BOOST_CHECK_THROW(validateExprTestHelper("tr(M) o M", AMD::INVALID), AMD::Exception);
}
