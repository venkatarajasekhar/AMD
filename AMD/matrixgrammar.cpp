#include <AMD/matrixgrammar.hpp>

namespace AMD {
namespace detail {

boost::shared_ptr<ExpressionTree> nil;

void BinaryOp::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                          boost::shared_ptr<ExpressionTree> const& rhs) const
{

    boost::shared_ptr<ExpressionTree> 
        lhs(spirit::utf8_symbol_range_type(&d_op, &d_op+1), nil, nil);
    *lhs.swap(parent);
    *parent.setLeftChild(lhs);
    *parent.setRightChild(rhs);

    // parent.push_back(spirit::utf8_symbol_range_type(&d_op, &d_op+1));
    // parent.push_back(lhs);
    // parent.push_back(rhs);
}
///< Function used by the BinaryOp struct to process a binary op expression
///  Changes current node in tree to lhs arguement, pushes the operator 
///  followed by the expressions involved

void UnaryOp::operator()(boost::shared_ptr<ExpressionTree>& expr, 
                         boost::shared_ptr<ExpressionTree> const& rhs) const
{
    *expr.setInfo(spirit::utf8_symbol_range_type(&d_op, &d_op+1));
    *expr.setLeftChild(rhs);
    *expr.setRightChild(nil);
    // expr.clear();
    // expr.push_back(spirit::utf8_symbol_range_type(&d_op, &d_op+1));
    // expr.push_back(rhs);
}
///< Function used by the UnaryOp struct to process a unary op
///  As there is no lhs involved in a unary op, it pushes the operator
///  and the affected term

} // namespace detail

} // namespace AMD



