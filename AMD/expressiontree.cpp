#include <AMD/expressiontree.hpp>
#include <AMD/exception.hpp>

#include <set>
#include <tr1/unordered_set>

namespace AMD { namespace detail {

ExpressionTree::ExpressionTree (const std::string& info, 
                                const boost::shared_ptr<Tree>& left, 
                                const boost::shared_ptr<Tree>& right) :
                                Tree(info, left, right)
{
    std::tr1::unordered_set<std::string> binaryOp;
    binaryOp.insert("+");
    binaryOp.insert("-");
    binaryOp.insert("o");

    std::tr1::unordered_set<std::string> unaryOp;
    unaryOp.insert("'");
    unaryOp.insert("_");
    unaryOp.insert("tr");
    unaryOp.insert("lgdt");

    if (binaryOp.count(info)) {
        // binary op check
        if (!(left) || !(right)) {
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of binary operator",
                AMD_INVALID_EXPRESSION);
        }
    } else if (unaryOp.count(info)) {
        // unary op check
        if (!(left) || (right)) {
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of unary operator",
                AMD_INVALID_EXPRESSION);
        }
    } else if (info == "-") {
        // negation or minus case
        if (!(left)) {
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of negation operator",
                AMD_INVALID_EXPRESSION);
        }
    } else if ((left) || (right)) {
        // leaf node
        throw AMD::ExceptionImpl(
            APPEND_LOCATION("from ExpressionTree constructor"),
            "No operator and not matrix or float",
            AMD_INVALID_EXPRESSION);
    } 

    this->d_info = info;
    this->d_left = left;
    this->d_right = right;
}

ExpressionTree::~ExpressionTree()
{
}


} } // namespace AMD::detail