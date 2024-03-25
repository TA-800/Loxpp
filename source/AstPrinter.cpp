#include "headers/AstPrinter.hpp"
#include "headers/Expr.hpp"
#include <memory>

void AstPrinter::setPrintResult(const std::unique_ptr<Expr> &expr)
{
    expr->accept(*this);
}

void AstPrinter::visitBinaryExpr(const Binary &expr)
{
    // Call print again on the left and right expressions to recursively print them if they are also other types of
    // expressions. E.g. ( 2 + 3 ) + 4 where 2 + 3 is also a binary expression
    result += "( " + expr.op.getLexeme() + " ";

    // Left expression
    setPrintResult(expr.left);

    result += " ";

    // Print the right expression
    setPrintResult(expr.right);

    result += " )";

    // result for 2 + 3 = ( + 2 3 )
    // result for ( 2 + 3 ) + 4 = ( + ( + 2 3 ) 4 )
}

void AstPrinter::visitUnaryExpr(const Unary &expr)
{
    result += "( " + expr.op.getLexeme() + " ";
    setPrintResult(expr.right);
    result += " )";
}

// No spaces for literals
void AstPrinter::visitLiteralExpr(const Literal &expr)
{
    // Token has void *literal that can be string (Type::STRING) or number (Type::NUMBER)
    // If nil
    if (expr.type == TokenInfo::Type::NIL || expr.value == nullptr)
        result += "NIL";

    // If the literal is a string
    else if (expr.type == TokenInfo::Type::STRING)
    {
        result += *static_cast<std::string *>(expr.value);
    }

    // If the literal is a number
    else if (expr.type == TokenInfo::Type::NUMBER)
    {
        // Cast to int then convert to string to print
        result += std::to_string(*static_cast<double *>(expr.value));
    }
}

void AstPrinter::visitGroupingExpr(const Grouping &expr)
{
    result += "( group ";
    setPrintResult(expr.expression);
    result += " )";
}

// Return the result
const std::string &AstPrinter::get() const
{
    return result;
};

// test here
/* int main(int argc, char *argv[]) */
/* { */
/*     // Create AstPrinter */
/*     AstPrinter printer; */

/*     // Create Expr = -123 * ( 45.67 ) -> Binary ( Unary (-, 123), *, Grouping ( 45.67 ) ) */
/*     // -> ( * ( - 123 ) ( group 45.67 ) ) */
/*     std::unique_ptr<Expr> binary = std::make_unique<Binary>( */
/*         std::make_unique<Unary>(Token(TokenInfo::MINUS, "-", nullptr, 1), */
/*                                 std::make_unique<Literal>((void *)new double(123), TokenInfo::Type::NUMBER)), */
/*         Token(TokenInfo::STAR, "*", nullptr, 1), */
/*         std::make_unique<Grouping>(std::make_unique<Literal>((void *)new double(45.67), TokenInfo::Type::NUMBER)));
 */

/*     // Very simple expression: 2 + 3 -> Binary ( +, 2, 3) */
/*     // -> ( + 2 3 ) */
/*     /1* std::unique_ptr<Expr> binary = *1/ */
/*     /1*     std::make_unique<Binary>(std::make_unique<Literal>((void *)new double(2), TokenInfo::Type::NUMBER), *1/
 */
/*     /1*                              Token(TokenInfo::PLUS, "+", nullptr, 1), *1/ */
/*     /1*                              std::make_unique<Literal>((void *)new double(3), TokenInfo::Type::NUMBER)); *1/
 */

/*     // In the Printer class, set the print result to the binary expression */
/*     printer.setPrintResult(binary); */

/*     // Get that result from the Printer class and print it */
/*     std::cout << printer.get() << std::endl; */

/*     return 0; */
/* } */
