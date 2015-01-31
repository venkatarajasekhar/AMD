#include <iostream>
#include <string>
#include <exception>

#include <AMD/expression.hpp>

int main()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "Expression parser...\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "Type an expression...or [q or Q] to quit\n\n";

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        try {
            AMD::expression my_expr = AMD::parse(str);
            std::cout << "Parsing succeeded: " << my_expr << "\n";
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
