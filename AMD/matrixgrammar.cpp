#include <AMD/matrixgrammar.hpp>

namespace AMD {
namespace detail {

static boost::shared_ptr<ExpressionTree> nil;

void BinaryOp::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                          boost::shared_ptr<ExpressionTree> const& rhs) const
{

    boost::shared_ptr<ExpressionTree> lhs(new ExpressionTree(&d_op, nil, nil));
    lhs.swap(parent);
    parent->setLeftChild(lhs);
    parent->setRightChild(rhs);
}

void UnaryOp::operator()(boost::shared_ptr<ExpressionTree>& expr, 
                         boost::shared_ptr<ExpressionTree> const& rhs) const
{
    expr->setInfo(&d_op);
    expr->setLeftChild(rhs);
    expr->setRightChild(nil);
}

} // namespace detail

} // namespace AMD



