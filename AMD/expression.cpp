#include <sstream>
#include <string.h>
#include <ctype.h>
#include <utility>  

#include <AMD/expression.hpp>

namespace AMD {

Expression generateExpression(const std::string& exprString)
{
    detail::MatrixGrammar<std::string::const_iterator> myParser;
    Expression expr;

    LOG_INFO << "Generating expression from " << exprString;

    std::string exprStringRR = preProcess(exprString);
    
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
    
    postProcess(expr);

    LOG_TRACE << "Finished generating expression";

    return expr;
}

template <typename MatrixType>
void evaluate(const Expression& expr, 
              std::map<std::string, 
              boost::variant<double, boost::shared_ptr<MatrixType> > >& matMap){
    typedef boost::shared_ptr<MatrixType> MatrixPtrType;
    typedef AMD::MatrixAdaptor_t<MatrixType> MatrixAdaptorType;

    const detail::Tree& root = *expr;
    std::ostringstream stream;
    stream << root;
    std::string key = stream.str();

    // 1. Base case: If leaf node, then it has to be matrix or scalar
    if (false==root.left() && false==root.right()) { 
        LOG_INFO << "Found leaf node";

        // if (std::string::npos!=root.info().find_first_of("0123456789")) {
        //     // Scalar
        // }

        if (std::string::npos != 
            root.info().find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")) {
            // Matrix
            if(matMap.find(key) == matMap.end()) {
                LOG_ERROR << "Evaluate failed, Undefined Matrix: " 
                    << root.info();
            }

        }
    }

    // unary op (+, -, tr, lgdt, _, '')
    if (root.left() && false==root.right()) { 
        LOG_INFO << "Found unary operation: " << root.info();

        evaluate(root.left(), matMap);

        stream.str("");
        stream << *root.left();
        if(matMap.find(stream.str()) == matMap.end()) {
            LOG_ERROR << "Evaluate failed, Undefined Expression: " 
                    << stream.str();
        }
        boost::variant<double, boost::shared_ptr<MatrixType> > val = 
            matMap[stream.str()];

        if ("+" == root.info()) {
            matMap[key] = val;
        }

        else if ("-" == root.info()) { }

        else if ("tr" == root.info()) { }

        else if ("lgdt" == root.info()) { }

        else if ("_" == root.info()) { }

        else if ("\'" == root.info()) { }
    }

    // binary op (+, -, o, *)
    if(root.left() && root.right()){
        LOG_INFO << "Found binary operation: " << root.info();

        std::ostringstream stream;

        stream << *root.left();
        if(matMap.find(stream.str()) == matMap.end()) {
            LOG_ERROR << "Evaluate failed, Undefined Expression: " 
                    << stream.str();
        }
        boost::variant<double, boost::shared_ptr<MatrixType> > leftVal = 
            matMap[stream.str()];

        stream.str("");

        stream << *root.right();
        if(matMap.find(stream.str()) == matMap.end()) {
            LOG_ERROR << "Evaluate failed, Undefined Expression: " 
                    << stream.str();
        }
        boost::variant<double, boost::shared_ptr<MatrixType> > rightVal = 
            matMap[stream.str()];

        if ("+" == root.info()) { 

            if(leftVal.which() == 0){ 
                // Operands are doubles
                matMap[key] = *leftVal + *rightVal;
            }
            else{
                // Operands are matrices
                MatrixPtrType result;
                MatrixAdaptorType::add(*leftVal, *rightVal, result);
                matMap[key] = result;
            }

        }

        else if ("-" == root.info()) { }

        else if ("o" == root.info()) { }

        else if ("*" == root.info()) { }

    }

    LOG_ERROR << "Evaluate failed, Invalid tree";

}

ExpressionType validateExpr(const boost::shared_ptr<detail::Tree>& expr)
{
    const detail::Tree& root = *expr;

    // 1. Base case: If leaf node, then it has to be matrix or scalar
    if (false==root.left() && false==root.right()) { 
        LOG_INFO << "Found leaf node";

        if (std::string::npos!=root.info().find_first_of("0123456789")) {
            return SCALAR;
        }

        if (std::string::npos != 
            root.info().find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")) {
            return MATRIX;
        }

        LOG_ERROR << "Leaf node is neither a number nor an alphabet";
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from validateExpr"),
                    "Invalid leaf node",
                    AMD_INVALID_EXPRESSION);
    }

    // unary op (+, -, tr, lgdt, _, '')
    if (root.left() && false==root.right()) { 
        LOG_INFO << "Found unary operation";

        ExpressionType exprType = validateExpr(root.left());
        if ("+" == root.info() || "-" == root.info()) {
            return exprType;
        }

        if("tr" == root.info() || "lgdt" == root.info()) {
            if (MATRIX == exprType) return SCALAR;
            throw AMD::ExceptionImpl(
                        APPEND_LOCATION("from validateExpr"),
                        "Invalid operand to tr/lgdt",
                        AMD_INVALID_EXPRESSION);
        }

        if("_" == root.info() || "\'" == root.info()) {
            if (MATRIX == exprType) return MATRIX;
            throw AMD::ExceptionImpl(
                        APPEND_LOCATION("from validateExpr"),
                        "Invalid operand to transpose/inverse",
                        AMD_INVALID_EXPRESSION);
        }
    }

    // binary op (+, -, o, *)
    if(root.left() && root.right()){
        LOG_INFO << "Found binary operation";

        ExpressionType leftExprType  = validateExpr(root.left());
        ExpressionType rightExprType = validateExpr(root.right());

        if("+" == root.info() || "-" == root.info() || "o" == root.info()) {
            if (leftExprType == rightExprType) return leftExprType;
            throw AMD::ExceptionImpl(
                        APPEND_LOCATION("from validateExpr"),
                        "Different type operands to +,-,o",
                        AMD_INVALID_EXPRESSION);
        }

        if ("*" == root.info()) {
            if (leftExprType == rightExprType) return leftExprType;

            if ((MATRIX==leftExprType && SCALAR==rightExprType) || 
                (SCALAR==leftExprType && MATRIX==rightExprType)) {
                return MATRIX;
            }

            throw AMD::ExceptionImpl(
                        APPEND_LOCATION("from validateExpr"),
                        "Weird input to matrix multiplication",
                        AMD_INVALID_EXPRESSION);
        }
    }

    LOG_ERROR << "Postprocessing failed, Invalid tree";
}

void postProcess(const boost::shared_ptr<detail::Tree>& expr)
{
    LOG_INFO << "Begin post-processing";

    if (("tr" != (*expr).info()) && ("lgdt" != (*expr).info()))
    {
        LOG_ERROR << "Postprocessing failed, must be tr() or lgdt() function";
        throw AMD::ExceptionImpl(
                        APPEND_LOCATION("from postProcess"),
                        "Not tr() or lgdt() function",
                        AMD_INVALID_EXPRESSION);
    }
    ExpressionType exprType = validateExpr(expr);

    LOG_INFO << "Post-processing Succeeded";
}

std::string toRightRecursiveRep(const std::string& exprString)
{
    LOG_TRACE << "Preprocessing exprString " << __LINE__ << " " << __FILE__;
    std::size_t first_inv   = exprString.find("_");
    std::size_t first_trans = exprString.find("\'");

    if(first_inv != std::string::npos || first_trans != std::string::npos) {
        // The index of the first operator
        int first = std::min(first_inv, first_trans);

        // get begin and end indices of the operand substring
        int begin;
        int end = first;
        if(exprString[first - 1] == ')') {
            begin = findMatchingParen(exprString, first - 1);
        } else {
            begin = first - 1;
        }

        std::string op_str;
        if(exprString[first] == '\'')
            op_str = "(trans(" + exprString.substr(begin, end - begin) + "))";
        else if(exprString[first] == '_')
            op_str = "(inv(" + exprString.substr(begin, end - begin) + "))";

        std::string prefix = exprString.substr(0, begin);
        std::string suffix = exprString.substr(end + 1, std::string::npos);

        return toRightRecursiveRep(prefix + op_str + suffix);
    } else {
        LOG_TRACE << "Preprocessing exprString returning base case" 
                  << __LINE__ << " "<< __FILE__;
        return exprString;
    }
}

int findMatchingParen(const std::string& exprString, int index)
{
    LOG_TRACE << "finding Matching Parenthesis " << __LINE__ << " "<< __FILE__;
    int matching = -1;
    int open_paren_count = 0;
    int close_paren_count = 1;

    // Start from the close parenthesis and scan to the left
    for(int i = index - 1; i >= 0; i--){
        if(exprString[i] == ')')
            close_paren_count += 1;
        else if(exprString[i] == '(')
            open_paren_count += 1;
        if(open_paren_count - close_paren_count == 0){ // arrived at the match
            matching = i;
            break;
        }
    }

    if (-1 == matching) {
        // If no match is found throw an exception
        throw AMD::ExceptionImpl(
                    APPEND_LOCATION("from findMatchingParen"),
                    "Expression has unmatched parentheses",
                    AMD_INVALID_EXPRESSION);
    }
    return matching;
}

std::string preProcess(const std::string& exprString)
{
    return toRightRecursiveRep(exprString);
}

std::ostream& operator<<(std::ostream& os, const Expression& e)
{
    LOG_TRACE << "Printing Expression";
    e->print(os);
    return os;
}

} // namespace AMD
