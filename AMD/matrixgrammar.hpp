#ifndef AMD_MATRIX_GRAMMAR_HPP
#define AMD_MATRIX_GRAMMAR_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace AMD {

namespace detail {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;

struct UnaryOp
{
    template <typename T1, typename T2 = void>
    struct result { typedef void type; };
    ///< The result of the unary operation. Will be void.
    char const d_op;
    ///< Stores the character representation of the current operation.

    UnaryOp(char op) : d_op(op) {}
    ///< Constructs a new unary operation.
    /// @param[in] op unary operation symbol

    void operator()(spirit::utree& parent, spirit::utree const& rhs) const;
    ///< Modifies the tree such that the passed in parent becomes the operator
    ///  and the child is the right hand side
    /// 
    ///  @param[in] parent the current lhs of the tree
    ///  @param[in] rhs    the current rhs of the tree
};

static boost::phoenix::function<UnaryOp> const neg = UnaryOp('-');
static boost::phoenix::function<UnaryOp> const trans = UnaryOp('\'');
static boost::phoenix::function<UnaryOp> const inv = UnaryOp('_');

/// This struct represents a binary operation that works on 
/// two matrices.
struct BinaryOp
{
    template <typename T1, typename T2 = void>
    struct result { typedef void type; };
    ///< The result of the binary operation. Will be void.
    char const d_op;
    ///< Stores the character representation of the current operation.

    BinaryOp(char op) : d_op(op) {}
    ///< Constructs a new binary operation.
    /// @param[in] op binary operation symbol

    void operator()(spirit::utree& parent, spirit::utree const& rhs) const;
    ///< Modifies the tree such that the passed in parent becomes the left 
    ///  hand node and the operation becomes the parent
    /// 
    ///  @param[in] parent the current lhs of the tree
    ///  @param[in] rhs    the current rhs of the tree
};

static boost::phoenix::function<BinaryOp> const plus = BinaryOp('+');
static boost::phoenix::function<BinaryOp> const minus = BinaryOp('-');
static boost::phoenix::function<BinaryOp> const times = BinaryOp('*');
static boost::phoenix::function<BinaryOp> const divide = BinaryOp('/');

template <typename Iterator>
struct MatrixGrammar : qi::grammar<Iterator, 
                                   ascii::space_type, 
                                   spirit::utree()>
{

    MatrixGrammar();
    ///< The parser grammar is defined as follows: 
    ///  expression = term | term + term | term - term
    ///  term = invtran | invtran * invtran | invtran / invtran
    ///  factor = qi::upper | qi::double | (expression) | -factor | +factor   

    qi::rule<Iterator, ascii::space_type, spirit::utree()> d_expression, 
                                                           d_term, 
                                                           d_factor,
                                                           d_invtran;

};

template <typename Iterator>
MatrixGrammar<Iterator>::MatrixGrammar() : MatrixGrammar::base_type(d_expression)
{

    d_expression =
        d_term                            [qi::_val = qi::_1]
        >> *(   ('+' >> d_term            [plus(qi::_val, qi::_1)])
            |   ('-' >> d_term            [minus(qi::_val, qi::_1)])
            )
        ;

    d_term =
        d_invtran                          [qi::_val = qi::_1]
        >> *(   ('*' >> d_invtran          [times(qi::_val, qi::_1)])
            |   ('/' >> d_invtran          [divide(qi::_val, qi::_1)])
            )
        ;

    // FIXME
    // Consider "A'_'", which means trans(inv(trans(A)))
    // "A" is a invtran, which is followed by construction 
    // of multiple unary operations around it.
    d_invtran = // order of the specification is important
          (d_factor [trans(qi::_val, qi::_1)] >> '\'')   
       |  (d_factor [inv(qi::_val, qi::_1)]   >> '_')    
       |  d_factor             [qi::_val = qi::_1]
      ;

    // FIXME
    // Consider "-A'", which means -(trans(A)). The current 
    // construction parses this as trans(-A), which is incorrect.
    d_factor =
      qi::upper                             [qi::_val = qi::_1]
      |   qi::double_                       [qi::_val = qi::_1]
      |   '(' >> d_expression                 [qi::_val = qi::_1] >> ')'
      |   ('-' >> d_factor                    [neg(qi::_val, qi::_1)])
      |   ('+' >> d_factor                    [qi::_val = qi::_1])
      ;

    BOOST_SPIRIT_DEBUG_NODE(d_expression);
    BOOST_SPIRIT_DEBUG_NODE(d_term);
    BOOST_SPIRIT_DEBUG_NODE(d_factor);
    BOOST_SPIRIT_DEBUG_NODE(d_invtran);
}

}   // namespace detail

}   // namespace AMD

#endif // AMD_MATRIX_GRAMMAR_HPP