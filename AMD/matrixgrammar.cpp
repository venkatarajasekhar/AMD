#include <AMD/matrixgrammar.hpp>

namespace AMD {
namespace detail {

void BinaryOp::operator()(spirit::utree& parent, 
                          spirit::utree const& rhs) const
{
    spirit::utree lhs;
    lhs.swap(parent);
    parent.push_back(spirit::utf8_symbol_range_type(&d_op, &d_op+1));
    parent.push_back(lhs);
    parent.push_back(rhs);
}

void UnaryOp::operator()(spirit::utree& expr, 
                         spirit::utree const& rhs) const
{
    expr.clear();
    expr.push_back(spirit::utf8_symbol_range_type(&d_op, &d_op+1));
    expr.push_back(rhs);
}

} // namespace detail

} // namespace AMD



