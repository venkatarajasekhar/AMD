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

    ///// Original tree: tr(X')
    boost::shared_ptr<ExpressionTree> 
        node1(new ExpressionTree("X",nil,nil));
    boost::shared_ptr<ExpressionTree> 
        node2(new ExpressionTree("\'",node1,nil));
    boost::shared_ptr<ExpressionTree> 
        node3(new ExpressionTree("tr",node2,nil));

    // Derivative tree: I
    boost::shared_ptr<ExpressionTree> 
        identity(new ExpressionTree("I",nil,nil);

    boost::shared_ptr<ExpressionTree> sol0 = 
        generateDerivativeExpression(node3, identity, 'X');

    BOOST_CHECK (sol0 == identity);

    ///// Original tree: lgdt(X')
    boost::shared_ptr<ExpressionTree>
        node4(new ExpressionTree("lgdt",node2,nil));

    // Derivative tree:
    boost::shared_ptr<ExpressionTree> 
        der1(new ExpressionTree("_",node1,nil));

    boost::shared_ptr<ExpressionTree> sol1 = 
        generateDerivativeExpression(node4, identity, 'X');

    BOOST_CHECK (sol1 == der1);

    ///// Original tree: tr(X+X)
    boost::shared_ptr<ExpressionTree>
        node5(new ExpressionTree("+",node1,node1));
    boost::shared_ptr<ExpressionTree>
        node6(new ExpressionTree("tr",node5,nil));

    // Derivative tree:
    boost::shared_ptr<ExpressionTree> 
        der2(new ExpressionTree("+",identity,identity));

    boost::shared_ptr<ExpressionTree> sol2 = 
        generateDerivativeExpression(node6, identity, 'X');

    BOOST_CHECK (sol2 == der2);

    ///// Original tree: tr(X-X)
    boost::shared_ptr<ExpressionTree>
        node7(new ExpressionTree("-",node1,node1));
    boost::shared_ptr<ExpressionTree>
        node8(new ExpressionTree("tr",node7,nil));

    // Derivative tree:
    boost::shared_ptr<ExpressionTree>
        der3(new ExpressionTree("-",identity,identity));

    boost::shared_ptr<ExpressionTree> sol3 =
        generateDerivativeExpression(node7, identity, 'X');

    BOOST_CHECK (sol3 == der3);

    ///// Original tree: tr(X*X)
    boost::shared_ptr<ExpressionTree>
        node9(new ExpressionTree("*",node1,node1));
    boost::shared_ptr<ExpressionTree>
        node10(new ExpressionTree("tr",node9,nil));

    

}
