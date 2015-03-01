/// @file expressiontree.hpp
/// Contains AMD::expressiontree

#ifndef AMD_EXPRESSION_TREE_HPP
#define AMD_EXPRESSION_TREE_HPP

#include <AMD/tree.hpp>

namespace AMD { namespace detail {

class ExpressionTree : public Tree
{
public:
    explicit ExpressionTree (const std::string& info, 
                             const boost::shared_ptr<Tree>& left, 
                             const boost::shared_ptr<Tree>& right);
    ///< Construct an expression tree that holds not only a Tree structure
    ///  but also an info string, which we strongly suggest is one 
    ///  of the operations (+,-,*,',_, tr, lgdt) or A-Z or a double value.

    virtual ~ExpressionTree();
    ///< Virtual destructor
};

} } // namespace AMD::detail

#endif // AMD_EXPRESSION_TREE_HPP
