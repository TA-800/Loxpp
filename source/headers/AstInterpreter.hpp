#ifndef ASTINTERPRETER_HPP
#define ASTINTERPRETER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"

class AstInterpreter : public ExprVisitor, StmtVisitor
{
    // Can return any type of value

    TokenInfo::Type type;         // Type of the literal (string, number, etc.)
    std::shared_ptr<void> result; // Value ("Hello", 2, etc.)

    bool isTruthy(const std::shared_ptr<void> &value, TokenInfo::Type type);
    bool isEqual(const std::shared_ptr<void> &left, const std::shared_ptr<void> &right, TokenInfo::Type leftType,
                 TokenInfo::Type rightType);

    // Runtime error checkers for binary and unary operations
    void checkNumberOperand(const Token &op, TokenInfo::Type rightType);
    void checkNumberOperands(const Token &op, TokenInfo::Type leftType, TokenInfo::Type rightType);

  public:
    /*
     * Interpreter will go through the AST of statements and expressions and set the result var equal to a computed
     * value from the statements or expressions. Use getResult() to get the result of the interpretation.
     */
    void setInterpretResult(const std::unique_ptr<Expr> &expr);                    // For expressions
    void setInterpretResult(const std::vector<std::unique_ptr<Stmt>> &statements); // For statements
    void execute(const std::unique_ptr<Stmt> &stmt);                               // Execute statements line by line

    void visitBinaryExpr(const Binary &expr) override;
    void visitUnaryExpr(const Unary &expr) override;
    void visitLiteralExpr(const Literal &expr) override;
    void visitGroupingExpr(const Grouping &expr) override;

    // Get the result of the interpretation
    std::shared_ptr<void> &getResult();
    TokenInfo::Type getResultType();

    // Get value from void pointer and set it to shared_ptr
    void setResult(std::shared_ptr<void> &toSet, void *toGet, TokenInfo::Type type);
    void setResult(std::shared_ptr<void> &toSet, const std::shared_ptr<void> &toGet, TokenInfo::Type type);

    // Free pointers
    /* void freePointer(void *&ptr); */

    // Calls setInterpretResult() (and getResult) to begin interpreting the AST
    void evaluate(const std::unique_ptr<Expr> &expr);
    virtual void visitExpressionStmt(const Expression &expr) override;
    virtual void visitPrintStmt(const Print &stmt) override;
    std::string stringify(const std::shared_ptr<void> &result, TokenInfo::Type type);
};

#endif // !ASTINTERPRETER_HPP
