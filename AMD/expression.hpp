#ifndef AMD_EXPRESSION_HPP
#define AMD_EXPRESSION_HPP

#define BOOST_SPIRIT_DEBUG

#include <string>
#include <stdexcept>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace AMD {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace spirit = boost::spirit;

    struct expr
    {
        template <typename T1, typename T2 = void>
        struct result { typedef void type; };

        expr(char op) : op(op) {}

        void operator()(spirit::utree& expr, spirit::utree const& rhs) const
        {
            spirit::utree lhs;
            lhs.swap(expr);
            expr.push_back(spirit::utf8_symbol_range_type(&op, &op+1));
            expr.push_back(lhs);
            expr.push_back(rhs);
        }

        char const op;
    };
    static boost::phoenix::function<expr> const plus = expr('+');
    static boost::phoenix::function<expr> const minus = expr('-');
    static boost::phoenix::function<expr> const times = expr('*');
    static boost::phoenix::function<expr> const divide = expr('/');

    struct negate_expr
    {
        template <typename T1, typename T2 = void>
        struct result { typedef void type; };

        void operator()(spirit::utree& expr, spirit::utree const& rhs) const
        {
            char const op = '-';
            expr.clear();
            expr.push_back(spirit::utf8_symbol_range_type(&op, &op+1));
            expr.push_back(rhs);
        }
    };
    static boost::phoenix::function<negate_expr> neg;

    struct transpose_expr
    {
        template <typename T1, typename T2 = void>
        struct result { typedef void type; };

        void operator()(spirit::utree& expr, spirit::utree const& rhs) const
        {
            char const op = '\'';
            expr.clear();
            expr.push_back(spirit::utf8_symbol_range_type(&op, &op+1));
            expr.push_back(rhs);
        }
    };
    static boost::phoenix::function<transpose_expr> trans;

    struct inverse_expr
    {
        template <typename T1, typename T2 = void>
        struct result { typedef void type; };

        void operator()(spirit::utree& expr, spirit::utree const& rhs) const
        {
            char const op = '_';
            expr.clear();
            expr.push_back(spirit::utf8_symbol_range_type(&op, &op+1));
            expr.push_back(rhs);
        }
    };
    static boost::phoenix::function<inverse_expr> inv;

    template <typename Iterator>
    struct parser : qi::grammar<Iterator, 
                                ascii::space_type, 
                                spirit::utree()>
    {
        parser() : parser::base_type(expression)
        {

            expression =
                term                            [qi::_val = qi::_1]
                >> *(   ('+' >> term            [plus(qi::_val, qi::_1)])
                    |   ('-' >> term            [minus(qi::_val, qi::_1)])
                    )
                ;

            term =
                invtran                          [qi::_val = qi::_1]
                >> *(   ('*' >> invtran          [times(qi::_val, qi::_1)])
                    |   ('/' >> invtran          [divide(qi::_val, qi::_1)])
                    )
                ;

            // FIXME
            // Consider "A'_'", which means trans(inv(trans(A)))
            // "A" is a invtran, which is followed by construction 
            // of multiple unary operations around it.
            invtran = // order of the specification is important
                  (factor [trans(qi::_val, qi::_1)] >> '\'')   
               |  (factor [inv(qi::_val, qi::_1)]   >> '_')    
               |  factor             [qi::_val = qi::_1]
              ;

            // FIXME
            // Consider "-A'", which means -(trans(A)). The current 
            // construction parses this as trans(-A), which is incorrect.
            factor =
              qi::upper                             [qi::_val = qi::_1]
              |   qi::double_                       [qi::_val = qi::_1]
              |   '(' >> expression                 [qi::_val = qi::_1] >> ')'
              |   ('-' >> factor                    [neg(qi::_val, qi::_1)])
              |   ('+' >> factor                    [qi::_val = qi::_1])
              ;

            BOOST_SPIRIT_DEBUG_NODE(expression);
            BOOST_SPIRIT_DEBUG_NODE(term);
            BOOST_SPIRIT_DEBUG_NODE(factor);
            BOOST_SPIRIT_DEBUG_NODE(invtran);
        }

        qi::rule<Iterator, ascii::space_type, spirit::utree()> expression, 
                                                                     term, 
                                                                   factor,
                                                                   invtran;

    };

    // Define the expression type to be utree for now.
    // FIXME: This has to change to an actual tree with nodes
    typedef spirit::utree expression;

    static inline expression parse(const std::string& expr_string)
    {
        parser<std::string::const_iterator> my_parser;
        expression my_expr;
        std::string::const_iterator iter = expr_string.begin();                         
        std::string::const_iterator end  = expr_string.end();
        bool result = phrase_parse(iter, 
                                   end, 
                                   my_parser, 
                                   spirit::ascii::space, 
                                   my_expr);
        
        // FIXME: Should exceptions be typed?
        if (false==result) {
            throw std::runtime_error("Parsing failed");
        } else if (iter != end) {
            throw std::runtime_error("Parsing failed at: " + 
                                      std::string(iter, end));
        }

        return my_expr;
    }

} // namespace AMD

#endif // AMD_EXPRESSION_HPP
