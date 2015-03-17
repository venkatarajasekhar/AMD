#ifndef AMD_MATRIX_GRAMMAR_HPP
#define AMD_MATRIX_GRAMMAR_HPP

#include <boost/lexical_cast.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/make_shared.hpp>

#include <AMD/expressiontree.hpp>
#include <AMD/log.hpp>

// Uncomment the define if you want debug messages for the grammar
// #define BOOST_SPIRIT_DEBUG

namespace AMD {

namespace detail {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;


/// Struct for handling matrix to scalar expressions including tr, lgdt
struct MatToScalOp
{
    /// @brief A structure that is used to provide information about the 
    ///        return type of the void function
    /// @tparam T1 Used by boost::spirit
    /// @tparam T2 Used by boost::spirit2
    template <typename T1, typename T2 = void>
    struct result { 
        typedef void type; 
        ///< The result of the MatToScal operation. Will be void.
        /// Needed for Boost to typecheck when called
    };
    
    
    const std::string d_op;
    ///< Stores the character representation of the current operation.

    MatToScalOp(const std::string& op) : d_op(op) {}
    ///< Constructs a new unary operation.
    /// @param[in] op unary operation symbol

    void operator()(boost::shared_ptr<ExpressionTree>& parent, 
                    boost::shared_ptr<ExpressionTree> const& rhs) const;
    ///< Modifies the tree such that the passed in parent becomes the operator
    ///  and the child is the right hand side
    /// 
    ///  @param[in] parent the current lhs of the tree
    ///  @param[in] rhs    the current rhs of the tree
};

static boost::phoenix::function<MatToScalOp> const trace = MatToScalOp("tr");
static boost::phoenix::function<MatToScalOp> const lgdt = MatToScalOp("lgdt");
///< MatToScal functions called by the parser's grammar rules
///  Phoenix validates the typing for boost which calls these functions

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
    
    
    const std::string d_op;
    ///< Stores the character representation of the current operation.

    UnaryOp(const std::string& op) : d_op(op) {}
    ///< Constructs a new unary operation.
    /// @param[in] op unary operation symbol

    void operator()(boost::shared_ptr<ExpressionTree>& parent, 
                    boost::shared_ptr<ExpressionTree> const& rhs) const;
    ///< Modifies the tree such that the passed in parent becomes the operator
    ///  and the child is the right hand side
    /// 
    ///  @param[in] parent the current lhs of the tree
    ///  @param[in] rhs    the current rhs of the tree
};


static boost::phoenix::function<UnaryOp> const neg = UnaryOp("-");
static boost::phoenix::function<UnaryOp> const trans = UnaryOp("\'");
static boost::phoenix::function<UnaryOp> const inv = UnaryOp("_");
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
    
    const std::string d_op;
    ///< Stores the character representation of the current operation.

    BinaryOp(const std::string& op) : d_op(op) {}
    ///< Constructs a new binary operation.
    /// @param[in] op binary operation symbol

    void operator()(boost::shared_ptr<ExpressionTree>& parent, 
                    boost::shared_ptr<ExpressionTree> const& rhs) const;
    ///< Modifies the tree such that the passed in parent becomes the left 
    ///  hand node and the operation becomes the parent
    /// 
    ///  @param[in] parent the current lhs of the tree
    ///  @param[in] rhs    the current rhs of the tree
};

static boost::phoenix::function<BinaryOp> const plus = BinaryOp("+");
static boost::phoenix::function<BinaryOp> const minus = BinaryOp("-");
static boost::phoenix::function<BinaryOp> const times = BinaryOp("*");
static boost::phoenix::function<BinaryOp> const divide = BinaryOp("/");
static boost::phoenix::function<BinaryOp> const elem_wise_times = BinaryOp("o");
///< Binary operation functions called during parsing of matrix expressions
///  Phoenix validates the typing for boost which calls these functions
//

template <typename T>
struct LeafOp
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
    
    ///  Creates an ExpressionTree from an upper case character
    /// 
    ///  @param[in] parent qi::_val
    ///  @param[in] rhs    a matrix character that was parsed
    ///  @tparam    T      any terminal (char or double)
    void operator()(boost::shared_ptr<ExpressionTree>& parent, 
                    const T& rhs) const;
};

/// Cannot move this function to the cpp file because we have a template T
template <typename T>
void LeafOp<T>::operator()(boost::shared_ptr<ExpressionTree>& parent, 
                           const T& rhs) const
{
    LOG_INFO << "Processing leaf: " << rhs;
    const std::string leafVal = boost::lexical_cast<std::string>(rhs);
    parent = boost::shared_ptr<ExpressionTree> 
                 (new ExpressionTree(leafVal,
                                     boost::shared_ptr<ExpressionTree>(),
                                     boost::shared_ptr<ExpressionTree>()));
}

static boost::phoenix::function<LeafOp<char> > const charLeafOp = 
                                                               LeafOp<char>();
static boost::phoenix::function<LeafOp<double> > const doubleLeafOp = 
                                                             LeafOp<double>();

template <typename Iterator>
struct MatrixGrammar : qi::grammar<Iterator, 
                                   ascii::space_type, 
                                   boost::shared_ptr<ExpressionTree>() >
{
    private:
    qi::rule<Iterator, 
             ascii::space_type, 
             boost::shared_ptr<ExpressionTree>() > d_expression;
    ///< Rule to handle main return type of matrix grammar composed of one
    ///  or more terms in sequence
    
    qi::rule<Iterator, 
             ascii::space_type, 
             boost::shared_ptr<ExpressionTree>() > d_term;
    ///< Rule to handle a constant or matrix factor that multiplies or divides
    ///  something in an expression

    qi::rule<Iterator, 
             ascii::space_type, 
             boost::shared_ptr<ExpressionTree>() > d_factor;
    ///< Creates a rule for a factor defined as an upper case letter 
    ///  representing a matrix or a double representing a constant
    ///  or a parenthetical expression or a series of factors
    ///  or a negated factor


    qi::rule<Iterator, 
             ascii::space_type, 
             boost::shared_ptr<ExpressionTree>() > d_invtran;
    ///< Defines a rule for inverse and/or transpose operators 

    qi::rule<Iterator, 
             ascii::space_type, 
             boost::shared_ptr<ExpressionTree>() > d_literal;
    ///< Defines the basic unit of expression: an uppercase character or double

    public:
    MatrixGrammar();
    ///< The parser grammar is defined as follows: 
    ///  expression = term | term + term | term - term
    ///  term = invtran | invtran * invtran | invtran / invtran
    ///  invtran = factor' | factor_ | factor
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
        d_invtran                         [qi::_val = qi::_1]
        >> *(   ('*' >> d_invtran         [times(qi::_val, qi::_1)])
            |   ('/' >> d_invtran         [divide(qi::_val, qi::_1)])
            |   ('o' >> d_invtran         [elem_wise_times(qi::_val, qi::_1)])
            )
        ;
    
     d_invtran = 
          ("trans(" >> d_expression [trans(qi::_val, qi::_1)] >> ')')
       |  ("inv(" >> d_expression  [inv  (qi::_val, qi::_1)] >> ')')
       |  d_factor  [qi::_val = qi::_1]
       ;

     d_factor =
            d_literal                        [qi::_val = qi::_1]
       |   '(' >> d_expression               [qi::_val = qi::_1] >> ')'
       |   ('-' >> d_expression                  [neg(qi::_val, qi::_1)])
       |   ('+' >> d_expression                  [qi::_val = qi::_1])
       |   ("tr(" >> d_expression            [trace(qi::_val, qi::_1)] >> ')')
       |   ("lgdt(" >> d_expression          [lgdt(qi::_val, qi::_1)] >> ')')
       ;

     d_literal =
       qi::upper                             [charLeafOp(qi::_val, qi::_1)]
       |   qi::double_                       [doubleLeafOp(qi::_val,qi::_1)]
       ;
    

    BOOST_SPIRIT_DEBUG_NODE(d_expression);
    BOOST_SPIRIT_DEBUG_NODE(d_term);
    BOOST_SPIRIT_DEBUG_NODE(d_invtran);
    BOOST_SPIRIT_DEBUG_NODE(d_factor);
    BOOST_SPIRIT_DEBUG_NODE(d_literal);
  
    ///< Code for debugging the grammar and seeing what was attempted
}

}   // namespace detail

}   // namespace AMD

#endif // AMD_MATRIX_GRAMMAR_HPP
