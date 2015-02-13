#include <AMD/expressiontree.hpp>
#include <stdio.h>

namespace AMD { namespace detail {

ExpressionTree::ExpressionTree (const std::string& info, 
                                const boost::shared_ptr<Tree>& left, 
                                const boost::shared_ptr<Tree>& right) :
                                Tree(info, left, right)
{
    
}

ExpressionTree::~ExpressionTree()
{
}


} } // namespace AMD::detail