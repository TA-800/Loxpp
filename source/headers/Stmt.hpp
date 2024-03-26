#ifndef Stmt_HPP
#define Stmt_HPP
#include "Expr.hpp"
#include <memory>

class Expression;
class Print;

class StmtVisitor
{
  public:
    virtual void visitExpressionStmt(const Expression &expr) = 0;
    virtual void visitPrintStmt(const Print &expr) = 0;
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
#endif
