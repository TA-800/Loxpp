#ifndef Stmt_HPP
#define Stmt_HPP

#include "Expr.hpp"
#include "Token.hpp"
#include <memory>

class Var;
class While; // Side effect is looping
class If;    // Side effect is conditional branching
class Block; // Side effect is grouping multiple stmts (into one)
class Print; // Side effect is printing to the console
class Expression;

// Create visitor interface. Every tree walker (interpreter, printer) that implements StmtVisitor will be able to do
// something useful with different types of Stmts it wants to parse.
class StmtVisitor
{
  public:
    virtual void visitIfStmt(const If &Stmt) = 0;
    virtual void visitWhileStmt(const While &Stmt) = 0;
    virtual void visitBlockStmt(const Block &Stmt) = 0;
    virtual void visitExpressionStmt(const Expression &Stmt) = 0;
    virtual void visitPrintStmt(const Print &Stmt) = 0;
    virtual void visitVarStmt(const Var &Stmt) = 0;
};
class Stmt
{
  public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor &visitor) = 0;
};
class If : public Stmt
{
  public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitIfStmt(*this);
    }
};
class While : public Stmt
{
  public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitWhileStmt(*this);
    }
};
class Block : public Stmt
{
  public:
    // The container for the statements in the block
    std::vector<std::unique_ptr<Stmt>> statements;

    Block(std::vector<std::unique_ptr<Stmt>> statements)
    {
        for (auto &stmt : statements)
        {
            this->statements.push_back(std::move(stmt));
        }
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitBlockStmt(*this);
    }
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
