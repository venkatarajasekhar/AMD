#include <AMD/expressiontree.hpp>

#define BOOST_TEST_MODULE TreeTest
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

BOOST_AUTO_TEST_CASE ( Constructor )
{
    typedef class AMD::detail::ExpressionTree ExprTree;
    boost::shared_ptr<ExprTree> nil;
    boost::shared_ptr<ExprTree> nodeC = boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeB = boost::make_shared<ExprTree>("B",nodeC,nil);
    boost::shared_ptr<ExprTree> nodeA = boost::make_shared<ExprTree>("A",nodeB,nil);
    boost::shared_ptr<ExprTree> nodeY = boost::make_shared<ExprTree>("Y",nil,nil);
    boost::shared_ptr<ExprTree> nodeZ = boost::make_shared<ExprTree>("Z",nil,nodeY);
    (*nodeA.get()).swap(*nodeZ.get());

    //   this        other
    //    a            z 
    //   b     <->      y
    //  c

    BOOST_CHECK (nodeA.get()->info() == "Z");
    BOOST_CHECK (nodeZ.get()->info() == "A");
}

