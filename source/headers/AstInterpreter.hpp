#ifndef ASTINTERPRETER_HPP
#define ASTINTERPRETER_HPP

#include "Expr.hpp"
#include <utility>

class AstInterpreter : public Visitor
{
    // Can return any type of value

    void *result = nullptr;                                     // Value ("Hello", 2, etc.)
    TokenInfo::Type type;                                       // Type of the literal (string, number, etc.)
    std::vector<std::pair<void *, TokenInfo::Type>> tempValues; // Temporary values to free after use

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
     * Add a value to the temporary values vector to free after use
     */
    void toCleanUp(void *&pointerToFree, TokenInfo::Type type);

    /*
     * Free all temporary values created by the interpreter to evaluate a result.
     * Use only after tokens are not needed anymore because interpreter holds references & pointers to them.
     */
    void cleanUp();

    // Calls setInterpretResult() (and getResult) to begin interpreting the AST
    void evaluate(const std::unique_ptr<Expr> &expr);
    std::string stringify(void *result, TokenInfo::Type type);
};

#endif // !ASTINTERPRETER_HPP
