#include <iostream>
#include <string>
#include <exception>
#include <cctype>
#include <boost/lexical_cast.hpp>

#include <AMD/expression.hpp>
#include <AMD/log.hpp>
#include <AMD/derivative.hpp>


int main()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "Expression parser...\n";
    std::cout << "/////////////////////////////////////////////////////////\n";

    std::string logLevel;
    std::cout << "Enter a logging level [1,2,3,4,5]: ";
    std::getline (std::cin, logLevel);
    AMD::setLogLevel(boost::lexical_cast<int>(logLevel));

    std::cout << "Type an expression...or [q or Q] to quit\n\n";
    std::string str, differentiate, matrix;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        LOG_DEBUG << "Entered string is : " << str;
        AMD::Expression myExpr;
        bool parsed = false;
        try {
            myExpr = AMD::generateExpression(str);
            std::cout << "Parsing succeeded: " << *myExpr << "\n";
            parsed = true;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        if (!parsed) 
            continue;
        std::cout << "Differentiate parsed expression? Enter y/n:\n";

        while (std::getline(std::cin, differentiate))
        {
            LOG_DEBUG << "Differentiation decision is : " << differentiate;

            if (differentiate[0] == 'N' || differentiate[0] == 'n')
                break;
            else if(differentiate[0] == 'Y' || differentiate[0] == 'y')
            {
                std::cout << "Enter target matrix to " <<
                "differentiate expression with respect to:\n";
             
                while (std::getline(std::cin, matrix))
                { 
                    LOG_DEBUG << "Entered target matrix is : " << matrix;
                    
                    if (matrix.length() != 1 || isupper(matrix[0]) == 0){
                        std::cout << "Improper target matrix entered\n";
                        break;
                    }
                    else {
                        try {
                            AMD::Expression derivative = AMD::generateDerivativeExpression(myExpr, "A");
                            std::cout << "Differentiation succeeded: " << *derivative << "\n";
                        } catch (const std::exception& e) {
                            std::cout << e.what() << std::endl;
                        }
                        break;
                    }
                }
                break;   
            }
        }
        std::cout << "Type an expression...or [q or Q] to quit\n\n";
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
