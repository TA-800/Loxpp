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
