#ifndef ASTINTERPRETER_HPP
#define ASTINTERPRETER_HPP

#include "Expr.hpp"

class AstInterpreter : public Visitor
{
    // Can return any type of value

    void *result = nullptr; // Value ("Hello", 2, etc.)
    TokenInfo::Type type;   // Type of the literal (string, number, etc.)

    bool isTruthy(void *value);
    bool isEqual(void *left, void *right, TokenInfo::Type leftType, TokenInfo::Type rightType);

    // Runtime error checkers for binary and unary operations
    void checkNumberOperand(const Token &op, TokenInfo::Type rightType);
    void checkNumberOperands(const Token &op, TokenInfo::Type leftType, TokenInfo::Type rightType);

  public:
    // Start interpreting the expression
    void setInterpretResult(const std::unique_ptr<Expr> &expr);

    void visitBinaryExpr(const Binary &expr) override;
    void visitUnaryExpr(const Unary &expr) override;
    void visitLiteralExpr(const Literal &expr) override;
    void visitGroupingExpr(const Grouping &expr) override;

    // Get the result of the interpretation
    void *getResult();
    TokenInfo::Type getResultType();

    /*
     * Clean up pointers when they are no longer needed. For example, left and right void * in visitBinaryExpr
     * for temporary values.
     */
    void cleanUp(void *pointerToFree);

    // Calls setInterpretResult() (and getResult) to begin interpreting the AST
    void evaluate(const std::unique_ptr<Expr> &expr);
    std::string stringify(void *result, TokenInfo::Type type);
};

#endif // !ASTINTERPRETER_HPP
