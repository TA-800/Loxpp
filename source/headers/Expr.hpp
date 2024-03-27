#ifndef Expr_HPP
#define Expr_HPP
#include "Token.hpp"
#include <memory>

// TODO: Ternary
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;

// Create visitor interface. Every subclass of Expr will implement Visitor to have access to accept method.
// Visitor can return any type T, example: Integer, String (to try to print an expression), void, etc.
// Visitor will have a visit method for each supported subclass of Expr.
// Example: visitBinaryExpr, visitGroupingExpr, visitLiteralExpr, visitUnaryExpr

class ExprVisitor
{
  public:
    virtual void visitBinaryExpr(const Binary &Expr) = 0;
    virtual void visitGroupingExpr(const Grouping &Expr) = 0;
    virtual void visitLiteralExpr(const Literal &Expr) = 0;
    virtual void visitUnaryExpr(const Unary &Expr) = 0;
    virtual void visitVariableExpr(const Variable &Expr) = 0;
};

// Abstract class def
class Expr
{
  public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor &visitor) = 0;
};

// Subclasses
class Binary : public Expr
{
  public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitBinaryExpr(*this);
    }
};
class Grouping : public Expr
{
  public:
    std::unique_ptr<Expr> expression;

    Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression))
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitGroupingExpr(*this);
    }
};
class Literal : public Expr
{
  public:
    void *value;
    TokenInfo::Type type; // (so visitor can know how to cast the value to the correct type)

    Literal(void *value, TokenInfo::Type type) : value(value), type(type)
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitLiteralExpr(*this);
    }
};
class Unary : public Expr
{
  public:
    Token op;
    std::unique_ptr<Expr> right;

    Unary(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right))
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitUnaryExpr(*this);
    }
};
class Variable : public Expr
{
  public:
    Token name;
    // Type of value held by the variable (e.g. STRING, NUMBER, CLASS, etc.) is stored in the environment

    // Initialize name of variable and its value type to NIL initially
    Variable(Token name) : name(name)
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        // variable expression is simply the value of the variable
        // e.g. var x = 2 then x would return 2
        visitor.visitVariableExpr(*this);
    }
};
#endif
