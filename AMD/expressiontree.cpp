#include <AMD/expressiontree.hpp>
#include <AMD/exception.hpp>

#include <tr1/unordered_set>

namespace AMD { namespace detail {

class Operations
{
    public:
    std::tr1::unordered_set<std::string> bOp;
    std::tr1::unordered_set<std::string> uOp;

    Operations()
    {
        bOp.insert("+");
        bOp.insert("-");
        bOp.insert("o");
        uOp.insert("'");
        uOp.insert("_");
        uOp.insert("tr");
        uOp.insert("lgdt");
    }

     // provide some way to get at letters_
};

static Operations operations; 

ExpressionTree::ExpressionTree (const std::string& info, 
                                const boost::shared_ptr<Tree>& left, 
                                const boost::shared_ptr<Tree>& right) :
                                Tree(info, left, right)
{

    if (operations.bOp.count(info)) {
        // binary op check
        if (!(left) || !(right)) {
            throw AMD::ExceptionImpl(
                APPEND_LOCATION("from ExpressionTree constructor"),
                "Incorrect use of binary operator",
                AMD_INVALID_EXPRESSION);
        }
    } else if (operations.uOp.count(info)) {
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