#include "headers/Expr.hpp"
#include <iostream>

class AstPrinter : public Visitor
{
    // Some return type that can be get( )-ed and printed
    std::string result;

  public:
    // Return a string representation of the expression
    std::string setPrintResult(const std::unique_ptr<Expr> &expr)
    {
        expr->accept(*this);
        return result;
    }

    void visitBinaryExpr(const Binary *expr) override
    {
        // Call print again on the left and right expressions to recursively print them if they are also other types of
        // expressions. E.g. ( 2 + 3 ) + 4 where 2 + 3 is also a binary expression

        result +=
            "( " + expr->op.getLexeme() + " " + setPrintResult(expr->left) + " " + setPrintResult(expr->right) + " )";

        // result for 2 + 3 = ( + 2 3 )
        // result for ( 2 + 3 ) + 4 = ( + ( + 2 3 ) 4 )
    }

    void visitUnaryExpr(const Unary *expr) override
    {
        result += "( " + expr->op.getLexeme() + " " + setPrintResult(expr->right) + " )";
    }

    void visitLiteralExpr(const Literal *expr) override
    {
        // Token has void *literal that can be string (Type::STRING) or number (Type::NUMBER)
        // If nil
        if (expr->type == TokenInfo::Type::NIL || expr->value == nullptr)
            result += "( nil )";

        // If the literal is a string
        else if (expr->type == TokenInfo::Type::STRING)
        {
            result += "( " + *static_cast<std::string *>(expr->value) + " )";
        }

        // If the literal is a number
        else if (expr->type == TokenInfo::Type::NUMBER)
        {
            // Cast to int then convert to string to print
            result += "( " + std::to_string(*static_cast<int *>(expr->value)) + " )";
        }
    }

    void visitGroupingExpr(const Grouping *expr) override
    {
        result += "( group " + setPrintResult(expr->expression) + " )";
    }

    // Return the result
    const std::string &get() const
    {
        return result;
    }
};

// test here
int main(int argc, char *argv[])
{
    // Create AstPrinter
    AstPrinter printer;

    // Create Expr = -123 * (45.67) -> Binary ( Unary (-, 123), *, Grouping ( Literal (45.67) ) )
    std::unique_ptr<Expr> binary = std::make_unique<Binary>(
        std::make_unique<Unary>(Token(TokenInfo::MINUS, "-", nullptr, 1),
                                std::make_unique<Literal>(new int(123), TokenInfo::Type::NUMBER)),
        Token(TokenInfo::STAR, "*", nullptr, 1),
        std::make_unique<Grouping>(std::make_unique<Literal>(new std::string("45.67"), TokenInfo::Type::STRING)));

    // In the Printer class, set the print result to the binary expression
    printer.setPrintResult(binary);

    // Get that result from the Printer class and print it
    std::cout << printer.get() << std::endl;

    return 0;
}
