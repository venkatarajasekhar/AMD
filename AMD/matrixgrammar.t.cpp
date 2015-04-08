#include <AMD/matrixgrammar.hpp>

#define BOOST_TEST_MODULE MatrixGrammarTest
#include <boost/test/unit_test.hpp>

bool compareExpectedExpressions(const std::string& exprString, 
                                const std::string& trueParsedString)
{
    std::ostringstream stream;
    AMD_START_TRY_BLOCK()

    AMD::detail::MatrixGrammar<std::string::const_iterator> myParser;
    AMD::Expression expr;

    LOG_INFO << "Generating expression from " << exprString;

    std::string exprStringRR = AMD::preProcess(exprString);
    
    std::string::const_iterator iter = exprStringRR.begin();
    std::string::const_iterator end  = exprStringRR.end();
    bool result = phrase_parse(iter, 
                               end, 
                               myParser, 
                               boost::spirit::ascii::space, 
                               expr);

    if (false == result) {
        LOG_ERROR << "Parsing failed";
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from generateExpression"),
                    "Parsing failed",
                    AMD_INVALID_EXPRESSION);

    } else if (iter != end) {
        LOG_ERROR << ("Parsing failed at: " + std::string(iter, end));
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from generateExpression"),
                    ("Parsing failed at: " + std::string(iter, end)).c_str(),
                    AMD_INVALID_EXPRESSION);
    } 

    LOG_TRACE << "Finished generating expression";

    stream << *expr;
    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
    std::string parsedString = stream.str();
    return !(parsedString.compare(trueParsedString));
}

/*  Regression test that checks whether the parser is functioning correctly
 *  compareExpectedExpressions takes in two input strings which represent
 *  the input to parse followed by the true parsed string to compare against
 */

BOOST_AUTO_TEST_CASE ( parseExpression )
{ 
    
    //single matrix test
    BOOST_CHECK(compareExpectedExpressions("A", "\"A\"") == 1);
    
    //binary op expressions
    BOOST_CHECK(compareExpectedExpressions("A+B", "(+ \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("A-B", "(- \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("A*B", "(* \"A\" \"B\")")); 
    BOOST_CHECK(compareExpectedExpressions("AoB", "(o \"A\" \"B\")")); 
    
    //multiple expressions
    BOOST_CHECK(compareExpectedExpressions("A-B+C", 
            "(+ (- \"A\" \"B\") \"C\")")); 
    
    //unary op expressions
    BOOST_CHECK(compareExpectedExpressions("tr(B)", "(tr \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("lgdt(B)", "(lgdt \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("B'", "(' \"B\")"));     
    BOOST_CHECK(compareExpectedExpressions("B_", "(_ \"B\")"));     
   

    //tricky unary op combinations
    BOOST_CHECK(compareExpectedExpressions("-B'", "(- (' \"B\"))") == 1);     
    BOOST_CHECK(compareExpectedExpressions("B''", "(' (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("-B'_", "(- (_ (' \"B\")))"));
    BOOST_CHECK(compareExpectedExpressions("-+-A", "(- (- \"A\"))"));   
   
    
    //combination of binary and unary op expressions
    BOOST_CHECK(compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("A+B'", "(+ \"A\" (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("A'+B'", 
            "(+ (' \"A\") (' \"B\"))"));     
    BOOST_CHECK(compareExpectedExpressions("-A'*-B'", 
            "(* (- (\' \"A\")) (- (\' \"B\")))"));     
    
    //complex tr expressions
    BOOST_CHECK(compareExpectedExpressions("tr(A)*tr(B)", 
            "(* (tr \"A\") (tr \"B\"))")); 
    // BOOST_CHECK(compareExpectedExpressions("tr(A)*tr(B)+7*4*A", 
    //         "(+ (* (tr \"A\") (tr \"B\")) (* (* \"7\" \"4\") \"A\"))"));
    
    //testing negation priority  
    // BOOST_CHECK(compareExpectedExpressions("-A'+tr(lgdt(A*B/C))", 
    //         "(+ (- (' \"A\")) (tr (lgdt (/ (* \"A\" \"B\") \"C\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-A'+A","(+ (- (' \"A\")) \"A\")")); 

    //TODO: Confirm division supported as well as associativity of ops
    // BOOST_CHECK(compareExpectedExpressions("--A'+tr(lgdt(A*B/C))", 
    //         "(+ (- (- (' \"A\"))) (tr (lgdt (/ (* \"A\" \"B\") \"C\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-B*C'", 
            "(* (- \"B\") (' \"C\"))"));
    BOOST_CHECK(compareExpectedExpressions("-(tr(B)*tr(B)+-tr(B))", 
            "(- (+ (* (tr \"B\") (tr \"B\")) (- (tr \"B\"))))"));
    BOOST_CHECK(compareExpectedExpressions("-A'*(B+((C)))", 
            "(* (- (' \"A\")) (+ \"B\" \"C\"))")); 
    BOOST_CHECK(compareExpectedExpressions("-(-A+-B)", 
            "(- (+ (- \"A\") (- \"B\")))")); 
   
    //check associativity of subtraction
    BOOST_CHECK(compareExpectedExpressions("A+B-C", 
            "(- (+ \"A\" \"B\") \"C\")")); 
    BOOST_CHECK(compareExpectedExpressions("A+B-C-D-E", 
            "(- (- (- (+ \"A\" \"B\") \"C\") \"D\") \"E\")")); 
} 