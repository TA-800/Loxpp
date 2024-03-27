#ifndef Stmt_HPP
#define Stmt_HPP
#include "Expr.hpp"
#include "Token.hpp"
#include <memory>

// Types of statements
// Expression evaluates to a value
class Expression;

// Statement that has a side effect
class Print;

// Statement that defines a variable
class Var;

class StmtVisitor
{
  public:
    virtual void visitExpressionStmt(const Expression &stmt) = 0;
    virtual void visitPrintStmt(const Print &stmt) = 0;
    virtual void visitVarStmt(const Var &stmt) = 0;
};
class Stmt
{
  public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor &visitor) = 0;
};
class Expression : public Stmt
{
  public:
    std::unique_ptr<Expr> expression;

    Expression(std::unique_ptr<Expr> expression) : expression(std::move(expression))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitExpressionStmt(*this);
    }
};
class Print : public Stmt
{
  public:
    std::unique_ptr<Expr> expression;

    Print(std::unique_ptr<Expr> expression) : expression(std::move(expression))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitPrintStmt(*this);
    }
};
class Var : public Stmt
{
  public:
    Token name;
    std::unique_ptr<Expr> initializer;

    Var(Token name, std::unique_ptr<Expr> initializer) : name(name), initializer(std::move(initializer))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitVarStmt(*this);
    }
};
#endif
