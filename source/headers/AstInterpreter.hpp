#ifndef ASTINTERPRETER_HPP
#define ASTINTERPRETER_HPP

#include "Expr.hpp"

class AstInterpreter : public Visitor
{
    // Can return any type of value

    TokenInfo::Type type;         // Type of the literal (string, number, etc.)
    std::shared_ptr<void> result; // Value ("Hello", 2, etc.)
    std::shared_ptr<void> left;
    std::shared_ptr<void> right;

    bool isTruthy(const std::shared_ptr<void> &value, TokenInfo::Type type);
    bool isEqual(const std::shared_ptr<void> &left, const std::shared_ptr<void> &right, TokenInfo::Type leftType,
                 TokenInfo::Type rightType);

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
    std::shared_ptr<void> &getResult();
    TokenInfo::Type getResultType();

    // Get value from void pointer and set it to shared_ptr
    void setResult(std::shared_ptr<void> &value, void *ptr, TokenInfo::Type type);
    void setResult(std::shared_ptr<void> &value, std::shared_ptr<void> &ptr, TokenInfo::Type type);

    // Free pointers
    /* void freePointer(void *&ptr); */

    // Calls setInterpretResult() (and getResult) to begin interpreting the AST
    void evaluate(const std::unique_ptr<Expr> &expr);
    std::string stringify(const std::shared_ptr<void> &result, TokenInfo::Type type);
};

#endif // !ASTINTERPRETER_HPP
