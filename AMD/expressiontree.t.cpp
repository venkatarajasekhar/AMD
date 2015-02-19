#include <AMD/expressiontree.hpp>
#include <AMD/exception.hpp>


#define BOOST_TEST_MODULE TreeTest
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

BOOST_AUTO_TEST_CASE ( Constructor )
{
    typedef class AMD::detail::ExpressionTree ExprTree;
    boost::shared_ptr<ExprTree> nil;
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeC = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeA = boost::make_shared<ExprTree>("A",nil,nil);
    boost::shared_ptr<ExprTree> nodeB = boost::make_shared<ExprTree>("'",nodeC,nodeA);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = boost::make_shared<ExprTree>("tr",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = boost::make_shared<ExprTree>("-",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = boost::make_shared<ExprTree>("+",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = boost::make_shared<ExprTree>("*",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeC = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeA = boost::make_shared<ExprTree>("A",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = boost::make_shared<ExprTree>("Y",nodeC,nodeA);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

}


BOOST_AUTO_TEST_CASE ( MirrorEqual )
{
    // Ensures that the expression tree supports mirror equality for
    // commutative operations

    typedef class AMD::detail::Tree Tree;
    boost::shared_ptr<Tree> nil;
    boost::shared_ptr<Tree> node1 = boost::make_shared<Tree>("N",nil,nil);
    boost::shared_ptr<Tree> node2 = boost::make_shared<Tree>("M",nil,nil);

    boost::shared_ptr<Tree> node4 = boost::make_shared<Tree>("+",node1,node2);
    boost::shared_ptr<Tree> node5 = boost::make_shared<Tree>("+",node2,node1);

    boost::shared_ptr<Tree> node6 = boost::make_shared<Tree>("*",node1,node2);
    boost::shared_ptr<Tree> node7 = boost::make_shared<Tree>("*",node2,node1);

    BOOST_CHECK ((*node4 == *node5));
    BOOST_CHECK (!(*node6 == *node7));

}
