#ifndef Stmt_HPP
#define Stmt_HPP
#include "Expr.hpp"
#include "Token.hpp"
#include <memory>

class Block;

// Expression evaluates to a value
class Expression;

// Statement that has a side effect
class Print;

// Statement that defines a variable
class Var;

// Create visitor interface. Every tree walker that implements StmtVisitor will be able to do something useful with
// different types of Stmts it wants to parse.
class StmtVisitor
{
  public:
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

// Block acts a single statement but contains multiple statements
class Block : public Stmt
{
  public:
    // The container for the statements in the block
    std::vector<std::unique_ptr<Stmt>> statements;

    Block(std::vector<std::unique_ptr<Stmt>> statements)
    {
        // vector creates copy of elements, unique_ptr does not allow copy
        // so permanently move elements from statements to this->statements
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
