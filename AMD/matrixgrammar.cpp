#include <AMD/matrixgrammar.hpp>
#include <AMD/log.hpp>

namespace AMD {
namespace detail {

static boost::shared_ptr<ExpressionTree> nil;

void BinaryOp::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                          boost::shared_ptr<ExpressionTree> const& rhs) const
{
    LOG_INFO << "Processing BinaryOp of type " << d_op;
    boost::shared_ptr<ExpressionTree> lhs(new ExpressionTree(d_op,parent,rhs));
    lhs.swap(parent);
}

void UnaryOp::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                         boost::shared_ptr<ExpressionTree> const& rhs) const
{
    LOG_INFO << "Processing UnaryOp of type " << d_op; 
    if(!parent){
        boost::shared_ptr<ExpressionTree> nil;
        parent = boost::make_shared<ExpressionTree>(d_op, rhs, nil);
    } else {
        parent->setInfo(d_op);
        parent->setLeftChild(rhs);
        parent->setRightChild(nil);
    }
}

void MatToScal::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                         boost::shared_ptr<ExpressionTree> const& rhs) const
{
    LOG_INFO << "Processing MatToScal of type " << d_op; 
    if(!parent){
        boost::shared_ptr<ExpressionTree> nil;
        parent = boost::make_shared<ExpressionTree>(d_op, rhs, nil);
    } else {
        parent->setInfo(d_op);
        parent->setLeftChild(rhs);
        parent->setRightChild(nil);
    }
}

} // namespace detail

} // namespace AMD
