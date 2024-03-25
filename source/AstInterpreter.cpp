#include "headers/AstInterpreter.hpp"
#include "headers/Loxpp.hpp"
#include "headers/RuntimeError.hpp"
#include <iostream>

/*
 *
 * Use visitor methods to set the result of the interpretation.
 * Then use getResult() to get the result.
 * Works recursively.
 *
 * E.g. Expr = (2 + 3) + 4
 * visitBinaryExpr(Expr.left) (2 + 3)
     * left = getResult() and getType()
     * visitBinaryExpr(Expr.right) 4
     * right = getResult() and getType()

     * then evalutate left and right based on the operator and set that to result (and type)
 */

void AstInterpreter::checkNumberOperand(const Token &op, TokenInfo::Type rightType)
{
    if (rightType == TokenInfo::Type::NUMBER)
        return;

    throw RuntimeError(op, "Operand must be a number.");
}

void AstInterpreter::checkNumberOperands(const Token &op, TokenInfo::Type leftType, TokenInfo::Type rightType)
{
    if (leftType == TokenInfo::Type::NUMBER && rightType == TokenInfo::Type::NUMBER)
        return;

    throw RuntimeError(op, "Operands must be numbers.");
}

void AstInterpreter::cleanUp()
{
    // void ptrs cannot be deleted. Cast to the correct type before deleting.

    if (result != nullptr)
    {
        if (type == TokenInfo::Type::STRING)
        {
            delete static_cast<std::string *>(result);
        }
        else if (type == TokenInfo::Type::NUMBER)
        {
            delete static_cast<double *>(result);
        }
    }

    result = nullptr;
    type = TokenInfo::Type::NIL;
}

void AstInterpreter::setInterpretResult(const std::unique_ptr<Expr> &expr)
{
    expr->accept(*this);
}

void *AstInterpreter::getResult()
{
    return result;
}

TokenInfo::Type AstInterpreter::getResultType()
{
    return type;
}

// Simplest interpretable expression
// For a literal expression, the value is the literal itself
void AstInterpreter::visitLiteralExpr(const Literal &expr)
{
    result = expr.value;
    type = expr.type;
}

// Grouping expression
void AstInterpreter::visitGroupingExpr(const Grouping &expr)
{
    // Grouping expression is just a wrapper around another expression.
    // Simply interpret the expression inside the grouping expression
    setInterpretResult(expr.expression);
}

bool AstInterpreter::isEqual(void *left, void *right, TokenInfo::Type leftType, TokenInfo::Type rightType)
{
    if (left == nullptr && right == nullptr)
        return true;

    // If bools, check if they are equal
    if (leftType != rightType)
        return false;

    // If both are numbers, check if they are equal
    if (leftType == TokenInfo::Type::NUMBER)
        return *static_cast<double *>(left) == *static_cast<double *>(right);

    // If both are strings, check if they are equal
    if (leftType == TokenInfo::Type::STRING)
        return *static_cast<std::string *>(left) == *static_cast<std::string *>(right);

    return false;
}

bool AstInterpreter::isTruthy(void *value)
{
    if (value == nullptr)
        return false;

    // If it's a number, check if it's not equal to 0
    if (type == TokenInfo::Type::NUMBER)
        return *static_cast<double *>(value) != 0;

    // If it's a string, check if it's not empty
    else if (type == TokenInfo::Type::STRING)
        return !static_cast<std::string *>(value)->empty();

    // If's a boolean holding false value
    else if (type == TokenInfo::Type::FALSE)
        return false;

    return true;
}

// Unary expression
void AstInterpreter::visitUnaryExpr(const Unary &expr)
{
    // Interpret the right expression on which the unary operator is then applied
    setInterpretResult(expr.right);
    TokenInfo::Type rightType = getResultType();
    void *right = getResult();

    cleanUp();

    switch (expr.op.getType())
    {

    case TokenInfo::Type::BANG: {
        // Negate
        bool newVal = !isTruthy(right);
        result = new bool(newVal);
        result = (void *)result;
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        break;
    }
    case TokenInfo::Type::MINUS: {
        checkNumberOperand(expr.op, rightType);
        result = new double(-(*static_cast<double *>(right)));
        result = (void *)result;
        type = TokenInfo::Type::NUMBER;
        break;
    }
    }

    return;
}

// Binary expression
void AstInterpreter::visitBinaryExpr(const Binary &expr)
{
    // Get left evaluation
    setInterpretResult(expr.left);
    TokenInfo::Type leftType = getResultType();
    void *left = getResult();

    // Get right evaluation
    setInterpretResult(expr.right);
    TokenInfo::Type rightType = getResultType();
    void *right = getResult();

    cleanUp();

    // Switch operator
    switch (expr.op.getType())
    {

    // Comparison operators
    case TokenInfo::Type::GREATER: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new bool(*static_cast<double *>(left) > *static_cast<double *>(right));
        break;
    }
    case TokenInfo::Type::GREATER_EQUAL: {

        checkNumberOperands(expr.op, leftType, rightType);
        result = new bool(*static_cast<double *>(left) >= *static_cast<double *>(right));
        break;
    }
    case TokenInfo::Type::LESS: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new bool(*static_cast<double *>(left) < *static_cast<double *>(right));
        break;
    }
    case TokenInfo::Type::LESS_EQUAL: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new bool(*static_cast<double *>(left) <= *static_cast<double *>(right));
        break;
    }

    case TokenInfo::Type::BANG_EQUAL: {
        bool newVal = !isEqual(left, right, leftType, rightType);
        break;
    }
    case TokenInfo::Type::EQUAL_EQUAL: {
        bool newVal = isEqual(left, right, leftType, rightType);
        break;
    }

    case TokenInfo::Type::MINUS: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new double(*static_cast<double *>(left) - *static_cast<double *>(right));
        break;
    }

        // Since + can do add & string concat, the ++ is overLOADED.
    case TokenInfo::Type::PLUS: {
        // If both are numbers, add them
        if (leftType == TokenInfo::Type::NUMBER && rightType == TokenInfo::Type::NUMBER)
            result = new double(*static_cast<double *>(left) + *static_cast<double *>(right));

        // Concatenate strings
        else if (leftType == TokenInfo::Type::STRING && rightType == TokenInfo::Type::STRING)
            result = new std::string(*static_cast<std::string *>(left) + *static_cast<std::string *>(right));
        break;
    }

        // If none of the above conditions are met, we are attempting to add two heterogeneous types. Throw error.
        throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");

    case TokenInfo::Type::SLASH: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new double(*static_cast<double *>(left) / *static_cast<double *>(right));
        break;
    }

    case TokenInfo::Type::STAR: {
        checkNumberOperands(expr.op, leftType, rightType);
        result = new double(*static_cast<double *>(left) * *static_cast<double *>(right));
        break;
    }
    }
    return;
}

void AstInterpreter::evaluate(const std::unique_ptr<Expr> &expr)
{
    try
    {
        setInterpretResult(expr);
        void *result = getResult();
        TokenInfo::Type type = getResultType();
        std::cout << stringify(result, type) << "\n";
    }
    catch (RuntimeError &error)
    {
        Loxpp::runtimeError(error);
    }
}

std::string AstInterpreter::stringify(void *result, TokenInfo::Type type)
{
    if (result == nullptr)
        return "nil";

    if (type == TokenInfo::Type::NUMBER)
        return std::to_string(*static_cast<double *>(result));

    if (type == TokenInfo::Type::STRING)
        return *static_cast<std::string *>(result);

    if (type == TokenInfo::Type::TRUE)
        return "true";

    if (type == TokenInfo::Type::FALSE)
        return "false";

    return "nil";
}
