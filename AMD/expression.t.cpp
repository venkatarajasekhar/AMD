#include <AMD/expression.hpp>

#define BOOST_TEST_MODULE ExpressionTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE ( generateExpression )
{
    AMD::Expression testExpression;

    // This should generate an exception as we do not implement A''. When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("A'_")));

    // This should generate an exception as we do not implement A''. When
    // we fix this, we will no longer fail this test
    BOOST_CHECK_NO_THROW ((testExpression = AMD::generateExpression("A_'")));

}
