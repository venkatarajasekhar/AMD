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

