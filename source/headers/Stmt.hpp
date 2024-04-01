#ifndef Stmt_HPP
#define Stmt_HPP

#include "Expr.hpp"
#include "Token.hpp"
#include <memory>

class If;
class While;
class Block;
class Break;
class Expression;
class Print;
class Var;
class Function;

class StmtVisitor

{
  public:
    virtual void visitIfStmt(const If &Stmt) = 0;
    virtual void visitWhileStmt(const While &Stmt) = 0;
    virtual void visitBlockStmt(const Block &Stmt) = 0;
    virtual void visitBreakStmt(const Break &Stmt) = 0;
    virtual void visitExpressionStmt(const Expression &Stmt) = 0;
    virtual void visitPrintStmt(const Print &Stmt) = 0;
    virtual void visitVarStmt(const Var &Stmt) = 0;
    virtual void visitFunctionStmt(const Function &Stmt) = 0;
};
class Stmt
{
  public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor &visitor) = 0;
    virtual std::unique_ptr<Stmt> clone() const = 0;
};
class If : public Stmt
{
  public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    If(std::unique_ptr<Expr> &condition, std::unique_ptr<Stmt> &thenBranch, std::unique_ptr<Stmt> &elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    {
    }

    If(std::unique_ptr<Expr> &&condition, std::unique_ptr<Stmt> &&thenBranch, std::unique_ptr<Stmt> &&elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    {
    }

    void accept(StmtVisitor &visitor) override
    {
        visitor.visitIfStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<If>(condition->clone(), thenBranch->clone(), elseBranch->clone());
    }
};
class Break : public Stmt
{
  public:
    // No ctor because no params
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitBreakStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<Break>();
    }
};
class While : public Stmt
{
  public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    While(std::unique_ptr<Expr> &condition, std::unique_ptr<Stmt> &body)
        : condition(std::move(condition)), body(std::move(body))
    {
    }

    While(std::unique_ptr<Expr> &&condition, std::unique_ptr<Stmt> &&body)
        : condition(std::move(condition)), body(std::move(body))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitWhileStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<While>(condition->clone(), body->clone());
    }
};
class Block : public Stmt
{
  public:
    std::vector<std::unique_ptr<Stmt>> statements;

    Block(std::vector<std::unique_ptr<Stmt>> &statements) : statements(std::move(statements))
    {
    }
    Block(std::vector<std::unique_ptr<Stmt>> &&statements) : statements(std::move(statements))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitBlockStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        std::vector<std::unique_ptr<Stmt>> clonedStatements;
        for (const auto &stmt : statements)
        {
            clonedStatements.push_back(stmt->clone());
        }
        return std::make_unique<Block>(std::move(clonedStatements));
    }
};
class Expression : public Stmt
{
  public:
    std::unique_ptr<Expr> expression;

    Expression(std::unique_ptr<Expr> &expression) : expression(std::move(expression))
    {
    }

    Expression(std::unique_ptr<Expr> &&expression) : expression(std::move(expression))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitExpressionStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<Expression>(expression->clone());
    }
};
class Print : public Stmt
{
  public:
    std::unique_ptr<Expr> expression;

    Print(std::unique_ptr<Expr> &expression) : expression(std::move(expression))
    {
    }

    Print(std::unique_ptr<Expr> &&expression) : expression(std::move(expression))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitPrintStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<Print>(expression->clone());
    }
};
class Var : public Stmt
{
  public:
    Token name;
    std::unique_ptr<Expr> initializer;

    Var(Token name, std::unique_ptr<Expr> &initializer) : name(name), initializer(std::move(initializer))
    {
    }

    Var(Token name, std::unique_ptr<Expr> &&initializer) : name(name), initializer(std::move(initializer))
    {
    }
    void accept(StmtVisitor &visitor) override
    {
        visitor.visitVarStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        return std::make_unique<Var>(name, initializer->clone());
    }
};
class Function : public Stmt
{
  public:
    Token name;                              // Name of the function
    std::vector<Token> params;               // Parameters (names)
    std::vector<std::unique_ptr<Stmt>> body; // Body of the function

    Function(Token name, std::vector<Token> &params, std::vector<std::unique_ptr<Stmt>> &body)
        : name(name), params(std::move(params)), body(std::move(body))
    {
    }

    Function(Token name, std::vector<Token> &&params, std::vector<std::unique_ptr<Stmt>> &&body)
        : name(name), params(std::move(params)), body(std::move(body))
    {
    }

    void accept(StmtVisitor &visitor) override
    {
        visitor.visitFunctionStmt(*this);
    }

    std::unique_ptr<Stmt> clone() const override
    {
        std::vector<Token> clonedParams;
        for (const auto &param : params)
        {
            clonedParams.push_back(param);
        }
        std::vector<std::unique_ptr<Stmt>> clonedBody;
        for (const auto &stmt : body)
        {
            clonedBody.push_back(stmt->clone());
        }
        return std::make_unique<Function>(name, std::move(clonedParams), std::move(clonedBody));
    }
};
#endif
