#include <AMD/derivative.hpp>

#define BOOST_TEST_MODULE DerivativeTest
#include <boost/test/unit_test.hpp>

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

    boost::shared_ptr<ExpressionTree> sol1 = 
        generateDerivativeExpression(node3, identity, 'X');

    BOOST_CHECK (sol1 == identity);

    ///// Original tree: lgdt(X')
    boost::shared_ptr<ExpressionTree>
        node4(new ExpressionTree("lgdt",node2,nil));

    // Derivative tree:
    boost::shared_ptr<ExpressionTree> 
        der1(new ExpressionTree("_",node1,nil));

    boost::shared_ptr<ExpressionTree> sol2 = 
        generateDerivativeExpression(node4, identity, 'X');

    BOOST_CHECK (sol2 == der1);

    ///// Original tree: tr(X+X)
    boost::shared_ptr<ExpressionTree>
        node5(new ExpressionTree("tr",node1,node1));

    // Derivative tree:
    boost::shared_ptr<ExpressionTree> 
        der2(new ExpressionTree("+",identity,identity));

    boost::shared_ptr<ExpressionTree> sol3 = 
        generateDerivativeExpression(node5, identity, 'X');

    BOOST_CHECK (sol3 == der2);
}
