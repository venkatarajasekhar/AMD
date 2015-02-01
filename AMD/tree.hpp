/// @file tree.hpp
/// Contains AMD::tree

#ifndef AMD_TREE_HPP
#define AMD_TREE_HPP

#include <boost/shared_ptr.hpp>

namespace AMD {

/// This class represents the symbolic binary expression tree that we parse
/// store within AMD after parsing the string. 
class Tree {
  private:
    // DATA
    std::string d_info;
    ///< Information present in this node: for AMD, this is either a matrix,
    ///  which is from "A" to "Z", or an operation, which is one of 
    ///  "+ - * o ' _ tr or lgdt" or a floating point number represented as
    ///  a string. We represent floating point numbers as a string to make 
    /// the data uniformly a string at every tree node.

    boost::shared_ptr<Tree> d_left;
    ///< A pointer to the right sub-tree. In the case that the operation is 
    ///  a unary operation, the left-subtree is valid and the right sub-tree
    ///  should be empty.

    boost::shared_ptr<Tree> d_right;
    ///< A pointer to the left sub-tree. In the case that the operation is 
    ///  a unary operation, the left-subtree is valid and the right sub-tree
    ///  should be empty.

  public:
    explicit Tree (const std::string& info, 
                   const Tree& left, 
                   const Tree& right);
    ///< Construct a new tree by combining the left and the right trees and
    ///  store the information in the std::string as the reason for the join.
    ///
    ///  @note Join can only happen through an operation, and not a 
    ///        leaf-node such as a constant or variable. That is, you 
    ///        cannot join left["A"] and right["B"] with "C"! You can 
    ///        only join using one of the functions.
    ///
    /// @param[in]  info 
    /// @param[in]  left  
    /// @param[in]  right
                   
    virtual ~Tree();
    ///< Recursively destroy this tree and everything under it

    template <typename OSTREAM>
    virtual OSTREAM& print(OSTREAM& os);

};

#endif // AMD_TREE_HPP
