#ifndef Expr_HPP
#define Expr_HPP
#include "Token.hpp"
class Expr
{
  public:
    virtual ~Expr() = default;
};
class Binary : public Expr
{
    Expr left;
    Token op;
    Expr right;

    Binary(Expr &left, Token &op, Expr &right) : left(left), op(op), right(right)
    {
    }
};
class Grouping : public Expr
{
    Expr expression;

    Grouping(Expr &expression) : expression(expression)
    {
    }
};
class Literal : public Expr
{
    void *value;

    Literal(void *value) : value(value)
    {
    }
};
class Unary : public Expr
{
    Token op;
    Expr right;

    Unary(Token &op, Expr &right) : op(op), right(right)
    {
    }
};
#endif
