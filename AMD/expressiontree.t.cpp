#include <AMD/expressiontree.hpp>
#include <AMD/exception.hpp>
#include <AMD/log.hpp>


#define BOOST_TEST_MODULE TreeTest
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

BOOST_AUTO_TEST_CASE ( Constructor )
{
    typedef class AMD::detail::ExpressionTree ExprTree;
    boost::shared_ptr<ExprTree> nil;

    /// Invalid construction using non operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeC = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeA = 
                boost::make_shared<ExprTree>("A",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("Y",nodeC,nodeA);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: +

    /// Invalid use of + binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("+",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of + binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("+",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of + binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("+",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of + binary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("+",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()


    /// TESTS: -

    /// Invalid use of - unary/binary operator: Should have a left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("-",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of - unary operator: Should have a left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("-",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of - unary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("-",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of - binary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("-",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: *

    /// Invalid use of * binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("*",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of * binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("*",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of * binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("*",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of * binary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("*",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: o

    /// Invalid use of o binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("o",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of + binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("o",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of o binary operator: Should have two children
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("o",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of o binary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("o",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: '

    /// Invalid use of ' unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("\'",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of ' unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("\'",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of ' unary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("\'",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of ' unary operator: should only have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("\'",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    
    /// TESTS: _

    /// Invalid use of _ unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("_",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of _ unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("_",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of _ unary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("_",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of _ unary operator: should only have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("_",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: tr

    /// Invalid use of tr unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("tr",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of tr unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("tr",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of tr unary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("tr",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of tr unary operator: should only have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("tr",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// TESTS: lgdt

    /// Invalid use of lgdt unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("lgdt",nil,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of lgdt unary operator: Should have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("lgdt",nil,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Valid use of lgdt unary operator
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("lgdt",nodeX,nil);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

    /// Invalid use of lgdt unary operator: should only have one left child
    AMD_START_TRY_BLOCK()
    boost::shared_ptr<ExprTree> nodeX = 
                boost::make_shared<ExprTree>("C",nil,nil);
    boost::shared_ptr<ExprTree> nodeY = 
                boost::make_shared<ExprTree>("lgdt",nodeX,nodeX);
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()

}


BOOST_AUTO_TEST_CASE ( MirrorEqual )
{
    // Ensures that the expression tree supports mirror equality for
    // commutative operations
    std::string commutative = "+-o"; // TODO add more ops
    std::string non_commutative = "*/"; // TODO add more ops

    typedef class AMD::detail::Tree Tree;
    boost::shared_ptr<Tree> nil;
    boost::shared_ptr<Tree> node1 = 
                boost::make_shared<Tree>("N",nil,nil);
    boost::shared_ptr<Tree> node2 = 
                boost::make_shared<Tree>("M",nil,nil);

    // The roots of the two mirror trees
    boost::shared_ptr<Tree> node4;
    boost::shared_ptr<Tree> node5;

    for(std::string::iterator op = commutative.begin();
        op != commutative.end(); ++op) {
        node4 = boost::make_shared<Tree>(std::string(1,*op),node1,node2);
        node5 = boost::make_shared<Tree>(std::string(1,*op),node2,node1);
        BOOST_CHECK (*node4 == *node5);
    }

    for(std::string::iterator op = non_commutative.begin();
        op != non_commutative.end(); ++op) {
        node4 = boost::make_shared<Tree>(std::string(1,*op),node1,node2);
        node5 = boost::make_shared<Tree>(std::string(1,*op),node2,node1);
        BOOST_CHECK (!(*node4 == *node5));
    }


}
