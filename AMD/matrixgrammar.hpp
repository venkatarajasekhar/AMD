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
// namespace definitions for boost spirit library parser tools

/// Struct for handling unary operator expressions including 
/// inverse, transpose, and negation on matrices
struct UnaryOp
{
    /// @brief A structure that is used to provide information about the 
    ///        return type of the void function
    /// @tparam T1 Used by boost::spirit
    /// @tparam T2 Used by boost::spirit2
    template <typename T1, typename T2 = void>
    struct result { 
        typedef void type; 
        ///< The result of the unary operation. Will be void.
        /// Needed for Boost to typecheck when called
    };
    
    
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
///< Unary operation functions called by the parser's grammar rules
///  Phoenix validates the typing for boost which calls these functions

/// This struct represents a binary operation that works on 
/// two matrices, supports addition, subtraction, multiplication, division
struct BinaryOp
{
    /// @brief A structure that is used to provide information about the 
    ///        return type of the void function
    /// @tparam T1 Used by boost::spirit
    /// @tparam T2 Used by boost::spirit2
    template <typename T1, typename T2 = void>
    struct result { 
        typedef void type; 
        ///< The result of the binary operation. Will be void.
        /// Needed for Boost to typecheck when called
    };
    
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
///< Binary operation functions called during parsing of matrix expressions
///  Phoenix validates the typing for boost which calls these functions

template <typename Iterator>
struct MatrixGrammar : qi::grammar<Iterator, 
                                   ascii::space_type, 
                                   spirit::utree()>
{
    private:
    qi::rule<Iterator, ascii::space_type, spirit::utree()> d_expression;
    ///< Rule to handle main return type of matrix grammar composed of one
    ///  or more terms in sequence
    
    qi::rule<Iterator, ascii::space_type, spirit::utree()> d_term;
    ///< Rule to handle a constant or matrix factor that multiplies or divides
    ///  something in an expression

    qi::rule<Iterator, ascii::space_type, spirit::utree()> d_factor;
    ///< Creates a rule for a factor defined as an upper case letter 
    ///  representing a matrix or a double representing a constant
    ///  or a parenthetical expression or a series of factors
    ///  or a negated factor

    qi::rule<Iterator, ascii::space_type, spirit::utree()> d_invtran;
    ///< Defines a rule for inverse and/or transpose operators 

    public:
    MatrixGrammar();
    ///< The parser grammar is defined as follows: 
    ///  expression = term | term + term | term - term
    ///  term = invtran | invtran * invtran | invtran / invtran
    ///  factor = qi::upper | qi::double | (expression) | -factor | +factor   

};
///< Parser that can iterate over a string of matrix expressions while skipping 
/// intermediary spaces and will create a tree of nodes representing the 
/// parsed matrix expression. 
/// Supports the following matrix operations: 
/// Addition(+) 
/// Subtraction(-)
/// Multiplication(*)
/// Division(/) 
/// Element-Wise Multiplication(o)
/// Negation(-)
/// Transpose(')
/// Inverse(_)


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
    ///< Code for debugging the grammar and seeing what was attempted
}

}   // namespace detail

}   // namespace AMD

#endif // AMD_MATRIX_GRAMMAR_HPP
