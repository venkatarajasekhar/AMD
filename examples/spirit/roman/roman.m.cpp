#include <iostream>
#include <string>
#include <vector>

#define BOOST_SPIRIT_DEBUG
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
 
//#include <boost/fusion/tuple.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/variant/recursive_variant.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/variant/apply_visitor.hpp>

// qi::parse cannot do white space skipping. qi::phrase_parse can do white 
// space skipping. As it is more powerful, we will always us qi::phrase_parse.
namespace client {

    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    // When the grammar you are trying to parse becomes complicated, it is 
    // best to ensure that we break down our language into small rules that 
    // can then be chained together to form productions. This is achieved 
    // in boost::spirit::qi using qi::rule and qi::grammar constructs.
    //
    // Different versions are:
    // qi::rule<Iterator> --- can only be used with qi::parse
    // qi::rule<Iterator, Skipper> --- can be used with qi::phrase_parse
    // qi::rule<Iterator, Skipper, Signature> --- most useful form
    //
    // The Signature specifies the attributes of the rule. The attributes are
    // synthesised (output) attribute and inherited (input) attributes. Once
    // a rule is declared, a variable of the rule type can be assigned to any
    // parser expression as long as the attribute of that expression is 
    // convertible to the rule type.
    //
    // A qi::grammar has the same template arguments as the rule. It 
    // encapsulates one or more rules. A grammar is declared by declaring one
    // or more rules and giving the base class the start rule.

    struct hundreds_ : qi::symbols<char, int>
    {
        hundreds_()
        {
            add
             ("C",   100)
             ("CC",  200)
             ("CCC", 300)
             ("CD",  400)
             ("D",   500)
             ("DC",  600)
             ("DCC", 700)
             ("DCCX",800)
             ("CM",  900)
             ;
        }
    } hundreds;

    struct tens_ : qi::symbols<char, int>
    {
        tens_ ()
        {
            add
                ("X"    , 10)
                ("XX"   , 20)
                ("XXX"  , 30)
                ("XL"   , 40)
                ("L"    , 50)
                ("LX"   , 60)
                ("LXX"  , 70)
                ("LXXX" , 80)
                ("XC"   , 90)
                ;
        }

    } tens;

    template <typename Iterator>
    struct roman_grammar : qi::grammar<Iterator, unsigned()>
    {
        roman_grammar() : roman_grammar::base_type(start)
        {

            start = qi::eps [qi::_val = 0] >>
                 (
                    +qi::lit('M') [qi::_val += 1000]
                    || hundreds [qi::_val += qi::_1] 
                    || tens [qi::_val += qi::_1]
                 );
        }

        qi::rule<Iterator, unsigned()> start;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tRoman Numerals Parser\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type a Roman Numeral ...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;
    typedef client::roman_grammar<iterator_type> roman;

    roman roman_parser; // Our grammar

    std::string str;
    unsigned result;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        //[tutorial_roman_grammar_parse
        bool r = parse(iter, end, roman_parser, result);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "result = " << result << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(iter, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \": " << rest << "\"\n";
            std::cout << "-------------------------\n";
        }
        //]
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
