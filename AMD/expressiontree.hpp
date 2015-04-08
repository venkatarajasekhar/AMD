/// @file expressiontree.hpp
/// Contains AMD::expressiontree, an extension of our base tree class which
/// supports matrix expressions that utilize +,-,*,transpose, negation,
//  inverse, trace, and logdet
//  Equality differs from base tree class in that mirror equality is supported
//  in ExpressionTree, meaning if A + B and B + A are being evaluated they
//  will return equal
//

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

    bool operator==(const ExpressionTree& other) const; 
    ///< evaluate the equality of this tree to another expression tree
    //   Returns true if the information in both trees is the same,
    //   even if the structure isnt the exact same
    
    bool operator!=(const ExpressionTree& other) const; 
    ///< Ensures that inequality is using the overridden == function
    
    boost::shared_ptr<ExpressionTree> left () const;
    ///< Return the left child stored in this node
    ///  @retval d_left

    boost::shared_ptr<ExpressionTree> right () const;
    ///< Return the right child stored in this node
    ///  @retval d_right

    virtual ~ExpressionTree();
    ///< Virtual destructor
};

} } // namespace AMD::detail

#endif // AMD_EXPRESSION_TREE_HPP
