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

void AstInterpreter::setInterpretResult(const std::unique_ptr<Expr> &expr)
{
    expr->accept(*this);
}

std::shared_ptr<void> &AstInterpreter::getResult()
{
    return result;
}

TokenInfo::Type AstInterpreter::getResultType()
{
    return type;
}

// Simplest interpretable expression
// For a literal expression, the value is the literal itself
// Be careful, freeing everything like left and right ptrs that get their result in the end from a literal value
// will also free the literal value itself held by Tokens array.
void AstInterpreter::setResult(std::shared_ptr<void> &toSet, void *toGet, TokenInfo::Type type)
{
    switch (type)
    {
    case TokenInfo::Type::NUMBER: {
        toSet = std::shared_ptr<void>(new double(*(static_cast<double *>(toGet))));
        break;
    }
    case TokenInfo::Type::STRING: {
        toSet = std::shared_ptr<void>(new std::string(*(static_cast<std::string *>(toGet))));
        break;
    }
    case TokenInfo::Type::TRUE: {
        toSet = std::shared_ptr<void>(new bool(true));
        break;
    }
    case TokenInfo::Type::FALSE: {
        toSet = std::shared_ptr<void>(new bool(false));
        break;
    }
    case TokenInfo::Type::NIL: {
        toSet = std::shared_ptr<void>(nullptr);
        break;
    }
    }
}

void AstInterpreter::setResult(std::shared_ptr<void> &toSet, const std::shared_ptr<void> &toGet, TokenInfo::Type type)
{
    switch (type)
    {
    case TokenInfo::Type::NUMBER: {
        toSet = std::shared_ptr<void>(new double(*(static_cast<double *>(toGet.get()))));
        break;
    }
    case TokenInfo::Type::STRING: {
        toSet = std::shared_ptr<void>(new std::string(*(static_cast<std::string *>(toGet.get()))));
        break;
    }
    case TokenInfo::Type::TRUE:
    case TokenInfo::Type::FALSE: {
        toSet = std::shared_ptr<void>(new bool(*(static_cast<bool *>(toGet.get()))));
        break;
    }
    }
}

void AstInterpreter::visitLiteralExpr(const Literal &expr)
{
    type = expr.type;
    setResult(result, expr.value, type);
}

// Grouping expression
void AstInterpreter::visitGroupingExpr(const Grouping &expr)
{
    // Grouping expression is just a wrapper around another expression.
    // Simply interpret the expression inside the grouping expression
    setInterpretResult(expr.expression);
}

bool AstInterpreter::isEqual(const std::shared_ptr<void> &left, const std::shared_ptr<void> &right,
                             TokenInfo::Type leftType, TokenInfo::Type rightType)
{
    if (left == nullptr && right == nullptr)
        return true;

    // If bools, check if they are equal
    if (leftType != rightType)
        return false;

    // If both are numbers, check if they are equal
    if (leftType == TokenInfo::Type::NUMBER)
        return *static_cast<double *>(left.get()) == *static_cast<double *>(right.get());

    // If both are strings, check if they are equal
    if (leftType == TokenInfo::Type::STRING)
        return *static_cast<std::string *>(left.get()) == *static_cast<std::string *>(right.get());

    return false;
}

bool AstInterpreter::isTruthy(const std::shared_ptr<void> &value, TokenInfo::Type type)
{
    if (value == nullptr)
        return false;

    // If it's a number, check if it's not equal to 0
    if (type == TokenInfo::Type::NUMBER)
        return *static_cast<double *>(value.get()) != 0;

    // If it's a string, check if it's not empty
    else if (type == TokenInfo::Type::STRING)
        return !static_cast<std::string *>(value.get())->empty();

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
    std::shared_ptr<void> right;
    setResult(right, getResult(), rightType);

    switch (expr.op.getType())
    {

    case TokenInfo::Type::BANG: {
        // Negate
        bool newVal = !isTruthy(right, rightType);
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }
    case TokenInfo::Type::MINUS: {
        checkNumberOperand(expr.op, rightType);
        type = TokenInfo::Type::NUMBER;
        setResult(result, right, rightType);
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
    std::shared_ptr<void> left;
    setResult(left, getResult(), leftType);

    // Get right evaluation
    setInterpretResult(expr.right);
    TokenInfo::Type rightType = getResultType();
    std::shared_ptr<void> right;
    setResult(right, getResult(), rightType);

    // Switch operator
    switch (expr.op.getType())
    {

    // Comparison operators
    case TokenInfo::Type::GREATER: {
        checkNumberOperands(expr.op, leftType, rightType);
        bool newVal = *(static_cast<double *>(left.get())) > *(static_cast<double *>(right.get()));
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }
    case TokenInfo::Type::GREATER_EQUAL: {

        checkNumberOperands(expr.op, leftType, rightType);
        bool newVal = *(static_cast<double *>(left.get())) >= *(static_cast<double *>(right.get()));
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }
    case TokenInfo::Type::LESS: {
        checkNumberOperands(expr.op, leftType, rightType);
        bool newVal = *(static_cast<double *>(left.get())) < *(static_cast<double *>(right.get()));
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }
    case TokenInfo::Type::LESS_EQUAL: {
        checkNumberOperands(expr.op, leftType, rightType);
        bool newVal = *(static_cast<double *>(left.get())) <= *(static_cast<double *>(right.get()));
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }

    case TokenInfo::Type::BANG_EQUAL: {
        bool newVal = !isEqual(left, right, leftType, rightType);
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }
    case TokenInfo::Type::EQUAL_EQUAL: {
        bool newVal = isEqual(left, right, leftType, rightType);
        type = newVal ? TokenInfo::Type::TRUE : TokenInfo::Type::FALSE;
        std::shared_ptr<void> tempResult(new bool(newVal));
        setResult(result, tempResult, type);
        break;
    }

    case TokenInfo::Type::MINUS: {
        checkNumberOperands(expr.op, leftType, rightType);
        type = TokenInfo::Type::NUMBER;
        std::shared_ptr<void> tempResult(
            new double(*static_cast<double *>(left.get()) - *static_cast<double *>(right.get())));
        setResult(result, tempResult, type);
        break;
    }

        // Since + can do add & string concat, the ++ is overLOADED.
    case TokenInfo::Type::PLUS: {
        // If both are numbers, add them
        if (leftType == TokenInfo::Type::NUMBER && rightType == TokenInfo::Type::NUMBER)
        {
            type = TokenInfo::Type::NUMBER;
            std::shared_ptr<void> tempResult(
                new double(*static_cast<double *>(left.get()) + *static_cast<double *>(right.get())));
            setResult(result, tempResult, type);
        }

        // Concatenate strings
        else if (leftType == TokenInfo::Type::STRING && rightType == TokenInfo::Type::STRING)
        {
            type = TokenInfo::Type::STRING;
            std::shared_ptr<void> tempResult(
                new std::string(*static_cast<std::string *>(left.get()) + *static_cast<std::string *>(right.get())));
            setResult(result, tempResult, type);
        }
        // Convert number to string and concat
        else if (leftType == TokenInfo::Type::STRING && rightType == TokenInfo::Type::NUMBER)
        {
            type = TokenInfo::Type::STRING;
            // Convert right to number
            std::shared_ptr<void> tempResult(new std::string(*static_cast<std::string *>(left.get()) +
                                                             std::to_string(*static_cast<double *>(right.get()))));
            setResult(result, tempResult, type);
        }
        else if (leftType == TokenInfo::Type::STRING && rightType == TokenInfo::Type::NUMBER)
        {
            type = TokenInfo::Type::STRING;
            // Convert left to number
            std::shared_ptr<void> tempResult(new std::string(std::to_string(*static_cast<double *>(left.get())) +
                                                             *static_cast<std::string *>(right.get())));
            setResult(result, tempResult, type);
        }
        else
        {
            // If none of the above conditions are met, we are attempting to add two heterogeneous types. Throw
            // error.
            throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
        }
        break;
    }

    case TokenInfo::Type::SLASH: {
        checkNumberOperands(expr.op, leftType, rightType);
        type = TokenInfo::Type::NUMBER;
        double rightVal = *static_cast<double *>(right.get());
        if (rightVal == 0)
            throw RuntimeError(expr.op, "Division by zero.");
        std::shared_ptr<void> tempResult(new double(*static_cast<double *>(left.get()) / rightVal));
        setResult(result, tempResult, type);
        break;
    }

    case TokenInfo::Type::STAR: {
        checkNumberOperands(expr.op, leftType, rightType);
        type = TokenInfo::Type::NUMBER;
        std::shared_ptr<void> tempResult(
            new double(*static_cast<double *>(left.get()) * *static_cast<double *>(right.get())));
        setResult(result, tempResult, type);
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
        std::cout << stringifyAndPrint(result, type) << "\n";
    }
    catch (RuntimeError &error)
    {
        Loxpp::runtimeError(error);
    }
}

std::string AstInterpreter::stringifyAndPrint(const std::shared_ptr<void> &result, TokenInfo::Type type)
{
    if (result == nullptr)
        return "nil";

    if (type == TokenInfo::Type::NUMBER)
        return std::to_string(*static_cast<double *>(result.get()));

    if (type == TokenInfo::Type::STRING)
        return *static_cast<std::string *>(result.get());

    if (type == TokenInfo::Type::TRUE)
        return "true";

    if (type == TokenInfo::Type::FALSE)
        return "false";

    return "nil";
}
