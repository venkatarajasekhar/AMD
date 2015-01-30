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

namespace client {

    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    template <typename Iterator>
    bool consume_double (Iterator first, Iterator last)
    {
        bool ret = qi::phrase_parse(first, 
                                    last, 
                                    qi::double_,
                                    ascii::space);

        if (first != last) return false;
        return ret;
    }

    template <typename Iterator>
    bool consume_complex (Iterator first, Iterator last)
    {
        std::vector<double> result;
        bool ret = qi::phrase_parse(first, 
                                    last, 
                                    // Grammar begin
                                    
                                         qi::lit('(')
                                            >> // followed by
                                         qi::double_ 
                                            >>
                                         -( qi::lit(',')
                                               >> 
                                            qi::double_ )
                                            >>
                                         qi::lit(')')
                                            | // or just a number
                                         qi::double_,
                                   
                                    // Grammar end
                                    ascii::space,
                                    result);

        if (first != last) return false;
        std::copy(result.begin(), 
                  result.end(), 
                  std::ostream_iterator<double>(std::cout, " "));
        return ret;
    }

    template <typename Iterator>
    bool stuff_numbers (Iterator first, Iterator last)
    {
        std::vector<double> result;
#if 0 // THIS TEDIOUS VERSION IS EQUAL TO
        bool ret = qi::phrase_parse(first, 
                                    last, 
                                    // Grammar begin
                                    
                  qi::double_[phoenix::push_back(phoenix::ref(result),qi::_1)] 
                     >>
                     *(
                  qi::lit(',')
                     >> 
                  qi::double_[phoenix::push_back(phoenix::ref(result),qi::_1)] 
                       ),
                                   
                                    // Grammar end
                                    ascii::space);
#else
        bool ret = qi::phrase_parse(first, 
                                    last, 
                                    // Grammar begin
                                    
#if 0 // THE EXPRESSION BELOW CAN BE SIMPLIFIED AS 
                  qi::double_ >> *( qi::lit(',') >> qi::double_ ),
#else
                  qi::double_ % qi::lit(','),
#endif
                                   
                                    // Grammar end
                                    ascii::space,
                                    result);
#endif

        if (first != last) return false;
        std::copy(result.begin(), 
                  result.end(), 
                  std::ostream_iterator<double>(std::cout, " "));
        return ret;
    }

}

int main(int argc, char** argv)
{
    std::cout << "//////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tA comma separated list parser for Spirit...\n\n";
    std::cout << "//////////////////////////////////////////////////////\n\n";

    std::cout << "Give me a comma separated list of numbers.\n";
    std::cout << "Type [q or Q] to quit\n\n";

    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        if (client::stuff_numbers(str.begin(), str.end()))
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
