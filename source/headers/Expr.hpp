#ifndef Expr_HPP
#define Expr_HPP

/*
 * Example code:
 *
abstract class Expr {
    interface Visitor<R> {
        R visitAssignExpr(AssignExpr expr);
        R visitBinaryExpr(BinaryExpr expr);
    }

    abstract <R> R accept(Visitor<R> visitor);
}

class AssignExpr extends Expr {
    String variable;
    Expr value;

    AssignExpr(String variable, Expr value) {
        this.variable = variable;
        this.value = value;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
        return visitor.visitAssignExpr(*this);
    }
}

class BinaryExpr extends Expr {
    Expr left;
    Expr right;
    String operator;

    BinaryExpr(Expr left, Expr right, String operator) {
        this.left = left;
        this.right = right;
        this.operator = operator;
    }

    @Override
    <R> R accept(Visitor<R> visitor) {
        return visitor.visitBinaryExpr(*this);
    }
}

class Evaluator implements Expr.Visitor<Integer> {
    @Override
    public Integer visitAssignExpr(AssignExpr expr) {
        // Evaluate assignment expression
        // Example: evaluate expression and assign to a variable
        return 0; // Dummy value for illustration
    }

    @Override
    public Integer visitBinaryExpr(BinaryExpr expr) {
        // Evaluate binary expression
        // Example: evaluate left and right operands based on operator
        return 0; // Dummy value for illustration
    }
}

public class Main {
    public static void main(String[] args) {
        Expr expression = new BinaryExpr(
            new AssignExpr("x", new LiteralExpr(5)),
            new AssignExpr("y", new LiteralExpr(10)),
            "+"
        );

        Evaluator evaluator = new Evaluator();
        expression.accept(evaluator);
    }
}
 *
 *
 */

#include "Token.hpp"
#include <memory>

// Forward declaration
// TODO: Ternary
class Binary;
class Grouping;
class Literal;
class Unary;

// Create visitor interface. Every subclass of Expr will implement Visitor to have access to accept method.
// Visitor can return any type T, example: Integer, String (to try to print an expression), void, etc.
// Visitor will have a visit method for each supported subclass of Expr.
// Example: visitBinaryExpr, visitGroupingExpr, visitLiteralExpr, visitUnaryExpr

class Visitor
{
  public:
    virtual void visitBinaryExpr(const Binary &expr) = 0;
    virtual void visitGroupingExpr(const Grouping &expr) = 0;
    virtual void visitLiteralExpr(const Literal &expr) = 0;
    virtual void visitUnaryExpr(const Unary &expr) = 0;
};

// Abstract class definition
class Expr
{
  public:
    virtual ~Expr() = default;
    virtual void accept(Visitor &visitor) = 0;
};

// Concrete class definitions
class Binary : public Expr
{

  public:
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    void accept(Visitor &visitor) override
    {
        visitor.visitBinaryExpr(*this);
    }
};

class Unary : public Expr
{

  public:
    Unary(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right))
    {
    }

    Token op;
    std::unique_ptr<Expr> right;

    void accept(Visitor &visitor) override
    {
        visitor.visitUnaryExpr(*this);
    }
};

class Literal : public Expr
{
  public:
    Literal(void *value, TokenInfo::Type type) : value(value), type(type)
    {
    }

    void *value;
    TokenInfo::Type type; // (so visitors can know what type of literal it is, e.g. PrintVisitor can safely cast to int
                          // or string before printing)

    void accept(Visitor &visitor) override
    {
        visitor.visitLiteralExpr(*this);
    }
};

class Grouping : public Expr
{
  public:
    Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression))
    {
    }

    std::unique_ptr<Expr> expression;

    void accept(Visitor &visitor) override
    {
        visitor.visitGroupingExpr(*this);
    }
};

#endif
