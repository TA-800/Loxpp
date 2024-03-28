#include "headers/AstInterpreter.hpp"
#include "headers/Environment.hpp"
#include "headers/Loxpp.hpp"
#include "headers/RuntimeError.hpp"
#include <iostream>
#include <memory>

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

void AstInterpreter::setInterpretResult(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    try
    {
        for (auto &stmt : statements)
        {
            execute(stmt); // Execute whatever statement we got -> expressionStmt, printStmt, varStmt
        }
    }
    catch (RuntimeError &error)
    {
        Loxpp::runtimeError(error);
    }
}

void AstInterpreter::execute(const std::unique_ptr<Stmt> &stmt)
{
    // Figure out what kind of statement we got and run it (printStmt, expressionStmt, varStmt)
    stmt->accept(*this);
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

void AstInterpreter::visitVariableExpr(const Variable &expr)
{
    // Get the value of the variable from the environment
    std::pair<std::shared_ptr<void>, TokenInfo::Type> value =
        environment->get(expr.name); // Can throw error if not defined

    // Set the result to the value of the variable
    setResult(result, value.first, value.second);
    type = value.second;
}

void AstInterpreter::visitAssignExpr(const Assign &expr)
{
    // Evaluate the right hand side of the assignment
    setInterpretResult(expr.value);

    // Set the value of the variable in the environment
    environment->assign(expr.name, getResult(), getResultType());
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

bool AstInterpreter::evaluate(const std::unique_ptr<Expr> &expr)
{
    try
    {
        setInterpretResult(expr);
        return true;
    }
    catch (RuntimeError &error)
    {
        Loxpp::runtimeError(error);
        return false;
    }
}

void AstInterpreter::visitExpressionStmt(const Expression &stmt)
{
    evaluate(stmt.expression);
};

void AstInterpreter::visitPrintStmt(const Print &stmt)
{
    bool successEval = evaluate(stmt.expression);
    // don't print if evaluation of some expression (like undefined variable) was not successful
    // error is thrown but caught by evaluate so stringifyResult line is still executed
    // and will print whatever was the last successfully evaluated value stored by Interpreter
    if (successEval)
        std::cout << stringifyResult(result, type) << "\n";
};

void AstInterpreter::visitBlockStmt(const Block &stmt)
{
    // Create new local environment for block
    std::shared_ptr<Environment> localEnv = std::make_shared<Environment>(this->environment);
    executeBlock(stmt.statements, localEnv);
}

void AstInterpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>> &statements,
                                  const std::shared_ptr<Environment> &localEnv)
{
    // Save the current environment (scope) before entering block (local) scope
    std::shared_ptr<Environment> previous = this->environment;

    try
    {
        this->environment = localEnv;

        for (const std::unique_ptr<Stmt> &statement : statements)
        {
            execute(statement);
        }
    }
    catch (RuntimeError &error)
    {
    }

    // Restore the previous environment (scope) before exiting block (local) scope
    this->environment = previous;
}

void AstInterpreter::visitVarStmt(const Var &stmt)
{
    std::shared_ptr<void> value = nullptr;

    if (stmt.initializer)
    {
        // initializer is an expression. E.g. var a = 5; initializer is literal expression '5'
        setInterpretResult(stmt.initializer);
        // Get evaluated value
        value = getResult();
    }

    environment->define(stmt.name.getLexeme(), value, getResultType());
}

std::string AstInterpreter::stringifyResult(const std::shared_ptr<void> &result, TokenInfo::Type type)
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
