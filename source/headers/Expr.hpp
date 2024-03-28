#ifndef Expr_HPP
#define Expr_HPP
#include "Token.hpp"
#include <memory>

// TODO: Ternary
class Assign;
class Binary;
class Grouping;
class Literal;
class Logical;
class Unary;
class Variable;

// Create visitor interface. Every subclass of Expr will implement Visitor to have access to accept method.
// Visitor can return any type T, example: Integer, String (to try to print an expression), void, etc.
// Visitor will have a visit method for each supported subclass of Expr.
// Example: visitBinaryExpr, visitGroupingExpr, visitLiteralExpr, visitUnaryExpr

class ExprVisitor
{
  public:
    virtual void visitAssignExpr(const Assign &Expr) = 0;
    virtual void visitBinaryExpr(const Binary &Expr) = 0;
    virtual void visitGroupingExpr(const Grouping &Expr) = 0;
    virtual void visitLiteralExpr(const Literal &Expr) = 0;
    virtual void visitLogicalExpr(const Logical &Expr) = 0;
    virtual void visitUnaryExpr(const Unary &Expr) = 0;
    virtual void visitVariableExpr(const Variable &Expr) = 0;
};
class Expr
{
  public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor &visitor) = 0;
};
class Assign : public Expr
{
  public:
    Token name;
    std::unique_ptr<Expr> value;

    Assign(Token name, std::unique_ptr<Expr> &value) : name(name), value(std::move(value))
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitAssignExpr(*this);
    }
};
class Binary : public Expr
{
  public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
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

    Grouping(std::unique_ptr<Expr> &expression) : expression(std::move(expression))
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
    TokenInfo::Type type;
    void *value;

    Literal(void *value, TokenInfo::Type type) : type(type), value(value)
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitLiteralExpr(*this);
    }
};

class Logical : public Expr
{
  public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Logical(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }
    void accept(ExprVisitor &visitor) override
    {
        visitor.visitLogicalExpr(*this);
    }
};
class Unary : public Expr
{
  public:
    Token op;
    std::unique_ptr<Expr> right;

    Unary(Token op, std::unique_ptr<Expr> &right) : op(op), right(std::move(right))
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
    // Type of value held by the variable (e.g. STRING, NUMBER, CLASS, etc.) is stored in the environment
    Token name;

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
