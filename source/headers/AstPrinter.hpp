#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP

#include "Expr.hpp"

class AstPrinter : public ExprVisitor
{

    std::string result = "";

  public:
    // Start constructing the string representation of the expression
    void setPrintResult(const std::unique_ptr<Expr> &expr);

    void visitBinaryExpr(const Binary &expr) override;
    void visitUnaryExpr(const Unary &expr) override;
    void visitLiteralExpr(const Literal &expr) override;
    void visitGroupingExpr(const Grouping &expr) override;

    // Get the string representation of the expression once completed
    const std::string &get() const;
};

#endif // ASTPRINTER_HPP
