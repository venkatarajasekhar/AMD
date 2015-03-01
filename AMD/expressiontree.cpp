#include <set>

#include <AMD/expressiontree.hpp>
#include <AMD/exception.hpp>
#include <AMD/log.hpp>

#define USE_BOOST_SPIRIT 1

namespace AMD { namespace detail {

/// A class that is used to hold all the valid validOps strings that can 
/// be used in AMD. This is a simple way of setting up a static string set.
class ValidOperations
{
    public:
    std::set<std::string> bOp;
    std::set<std::string> uOp;

    ValidOperations()
    {
        bOp.insert("+");
        bOp.insert("-");
        bOp.insert("*");
        bOp.insert("o");
        uOp.insert("-");
        uOp.insert("\'");
        uOp.insert("_");
        uOp.insert("tr");
        uOp.insert("lgdt");
    }

     // provide some way to get at letters_
};

static ValidOperations validOps; 

ExpressionTree::ExpressionTree (const std::string& info, 
                                const boost::shared_ptr<Tree>& left, 
                                const boost::shared_ptr<Tree>& right) :
                                Tree(info, left, right)
{
    LOG_TRACE << "Expression constructor: ";
    LOG_TRACE << info;
    if (left)  LOG_TRACE << left;  else LOG_TRACE << "NULL";
    if (right) LOG_TRACE << right; else LOG_TRACE << "NULL";

    if (validOps.bOp.count(info)) {
        // binary op check. we don't check for errors for "-" because it 
        // might be a unary minus op. this is checked for below.
        if (("-"!=info) && (!(left) || !(right))) {
#if USE_BOOST_SPIRIT==1
            LOG_ERROR <<  "Incorrect use of binary operator";
#else
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of binary operator",
                AMD_INVALID_EXPRESSION);
#endif
        }
    } else if (validOps.uOp.count(info)) {
        // unary op check
        if (!(left) || (right)) {
#if USE_BOOST_SPIRIT==1
            LOG_ERROR <<  "Incorrect use of unary operator";
#else
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of unary operator",
                AMD_INVALID_EXPRESSION);
#endif
        }
    } else if (left || right) {
        // leaf node
#if USE_BOOST_SPIRIT==1
        LOG_ERROR << "Not an operator, matrix, or float";
#else
        throw AMD::ExceptionImpl(
            APPEND_LOCATION("from ExpressionTree constructor"),
            "Not an operator, matrix, or float",
            AMD_INVALID_EXPRESSION);
#endif
    }

    this->d_info = info;
    this->d_left = left;
    this->d_right = right;
}

ExpressionTree::~ExpressionTree()
{
}

} } // namespace AMD::detail
