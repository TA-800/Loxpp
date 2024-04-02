#include "headers/AstInterpreter.hpp"
#include "headers/BreakError.hpp"
#include "headers/Environment.hpp"
#include "headers/LoxCallable.hpp"
#include "headers/LoxFunction.hpp"
#include "headers/Loxpp.hpp"
#include "headers/ReturnException.hpp"
#include "headers/RuntimeError.hpp"
#include <iostream>
#include <memory>
#include <utility>

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
    if (type == TokenInfo::Type::NIL)
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

void AstInterpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>> &statements,
                                  const std::shared_ptr<Environment> &localEnv)
{
    // Create a pointer to temporarily store the previous environment (scope) before entering block (local) scope
    std::shared_ptr<Environment> previous = this->environment;

    try
    {
        // Set the current environment (scope) to the local environment (scope) for the block
        this->environment = localEnv;

        for (const std::unique_ptr<Stmt> &statement : statements)
        {
            // Execute statements in the block with the local environment (scope)
            execute(statement);
        }
    }
    catch (RuntimeError &error)
    {
    }

    // Restore the previous environment (scope) before exiting block (local) scope
    this->environment = previous;
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

std::shared_ptr<void> &AstInterpreter::getResult()
{
    return result;
}

TokenInfo::Type AstInterpreter::getResultType()
{
    return type;
}

// Simplest interpretable expression
void AstInterpreter::setResult(std::shared_ptr<void> &toSet, std::shared_ptr<void> &toGet, TokenInfo::Type type)
{
    switch (type)
    {
    case TokenInfo::Type::FUN: // TODO: Class
    case TokenInfo::Type::NUMBER:
    case TokenInfo::Type::STRING:
    case TokenInfo::Type::TRUE:
    case TokenInfo::Type::FALSE: {
        toSet = toGet;
        break;
    }
    case TokenInfo::Type::NIL: {
        toSet = std::shared_ptr<void>(nullptr);
        break;
    }
    }
}

void AstInterpreter::visitLiteralExpr(const Literal &expr)
{
    type = expr.type;
    auto val = expr.value;
    setResult(result, val, type);
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
    type = value.second;

    if (type == TokenInfo::Type::UNINITIALIZED)
        throw RuntimeError(expr.name, "Variable used before being initialized.");

    setResult(result, value.first, type);
}

void AstInterpreter::visitAssignExpr(const Assign &expr)
{
    // Evaluate the right hand side of the assignment
    setInterpretResult(expr.value);

    // Set the value of the variable in the environment
    environment->assign(expr.name, getResult(), getResultType());
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
        else if (leftType == TokenInfo::Type::NUMBER && rightType == TokenInfo::Type::STRING)
        {
            type = TokenInfo::Type::STRING;
            // Convert left to number
            std::shared_ptr<void> tempResult(new std::string(std::to_string(*static_cast<double *>(left.get())) +
                                                             *static_cast<std::string *>(right.get())));
            setResult(result, tempResult, type);
        }
        else
            throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");

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

// Helper method to reduce code mess in visitCallExpr
bool AstInterpreter::isCallableType(TokenInfo::Type type)
{
    return type == TokenInfo::Type::FUN || type == TokenInfo::Type::CLASS;
}

// Call expression
void AstInterpreter::visitCallExpr(const Call &expr)
{
    setInterpretResult(expr.callee);              // will call visitCallExpr to ensure callee is of type Fun or Class
    TokenInfo::Type calleeType = getResultType(); // Get the type of the callee (function or class)

    // Check if callee is of a callable type (function or class)
    if (!isCallableType(calleeType))
        throw RuntimeError(expr.paren, "Can only call functions and classes.");

    auto callable = std::static_pointer_cast<LoxCallable>(getResult());

    // Evaluate argument expressions
    std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> arguments;
    for (const auto &arg : expr.arguments)
    {
        setInterpretResult(arg);
        arguments.push_back({getResult(), getResultType()});
    }

    if (arguments.size() != callable->arity())
        throw RuntimeError(expr.paren, "Expected " + std::to_string(callable->arity()) + " arguments but got " +
                                           std::to_string(arguments.size()) + ".");

    // Call the function, its return value will be an expression
    // (e.g. return 1 + 2; will return 3)
    auto [returnValue, returnType] = callable->call(*this, arguments);
    setResult(result, returnValue, returnType);
}

void AstInterpreter::visitExpressionStmt(const Expression &stmt)
{
    evaluate(stmt.expression);
};

void AstInterpreter::visitLogicalExpr(const Logical &expr)
{

    // Evaluate left side of the expression
    evaluate(expr.left);

    if (expr.op.getType() == TokenInfo::Type::OR)
    {
        if (isTruthy(getResult(), getResultType()))
        {
            // If the left side is true, we don't need to evaluate right side
            return;
        }
    }
    else
    {
        if (!isTruthy(getResult(), getResultType()))
        {
            // If left side is false, we don't need to evaluate right side
            return;
        }
    }

    // Evaluate right side of the expression
    evaluate(expr.right);
}

void AstInterpreter::visitIfStmt(const If &stmt)
{

    // Evaluate condition to some value
    evaluate(stmt.condition);

    // If condition is true, execute then branch
    if (isTruthy(getResult(), getResultType()))
        execute(stmt.thenBranch);
    // Else, if there was an else branch, execute it
    else if (stmt.elseBranch != nullptr)
        execute(stmt.elseBranch);
}

void AstInterpreter::visitWhileStmt(const While &stmt)
{

    evaluate(stmt.condition);
    try
    {
        while (isTruthy(getResult(), getResultType()))
        {
            // Execute while loop body
            execute(stmt.body);

            // Run condition check again
            evaluate(stmt.condition);
        }
    }
    catch (BreakError &e)
    {
        // BreakError is intentionally thrown to break out of loop.
        // Just catch it here to gracefully exit the loop, no need to do anything.
    }
}

void AstInterpreter::visitReturnStmt(const Return &stmt)
{
    std::shared_ptr<void> value = nullptr;
    if (stmt.value != nullptr)
    {
        evaluate(stmt.value);
        setResult(value, getResult(), getResultType());
    }

    // use exception to break out to a higher frame in the call stack
    throw ReturnException(getResult(), getResultType());
}

void AstInterpreter::visitBreakStmt(const Break &stmt)
{
    throw BreakError();
}

void AstInterpreter::visitPrintStmt(const Print &stmt)
{
    bool successEval = evaluate(stmt.expression);
    if (successEval)
        std::cout << stringifyResult(result, type) << "\n";
};

void AstInterpreter::visitBlockStmt(const Block &stmt)
{
    // Create new local environment for block
    std::shared_ptr<Environment> localEnv = std::make_shared<Environment>(this->environment);
    executeBlock(stmt.statements, localEnv);
}

// Function declaration (not call)
void AstInterpreter::visitFunctionStmt(const Function &stmt)
{
    // Copy stmt to a new var ptr.
    std::unique_ptr<Function> funcStmtPtr = std::unique_ptr<Function>(static_cast<Function *>(stmt.clone().release()));
    std::shared_ptr<LoxCallable> function = std::make_shared<LoxFunction>(funcStmtPtr);

    // Also define it in variables hashmap (so visitVariableExpr knows it's a function)
    environment->defineVar(stmt.name.getLexeme(), function, TokenInfo::Type::FUN);
}

void AstInterpreter::visitVarStmt(const Var &stmt)
{
    std::shared_ptr<void> value = nullptr;
    TokenInfo::Type valtype = TokenInfo::Type::UNINITIALIZED;

    if (stmt.initializer)
    {
        // initializer is an expression. E.g. var a = 5; initializer is literal expression '5'
        setInterpretResult(stmt.initializer);
        // Get evaluated value
        value = getResult();
        valtype = getResultType();
    }

    environment->defineVar(stmt.name.getLexeme(), value, valtype);
}

std::string AstInterpreter::stringifyResult(const std::shared_ptr<void> &result, TokenInfo::Type type)
{

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
