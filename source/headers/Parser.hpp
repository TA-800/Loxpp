#ifndef PARSER_HPP
#define PARSER_HPP

#include "Expr.hpp"
#include "Token.hpp"

/* Grammar:
 *
 * expression → equality ;
 * equality → comparison ( ( "!=" | "==" ) comparison )* ;
 * comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 * term → factor ( ( "-" | "+" ) factor )* ;
 * factor → unary ( ( "/" | "*" ) unary )* ;
 * unary → ( "!" | "-" ) unary | primary ;
 * primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
 *
 */

class Parser
{

    const std::vector<Token> tokens;
    int current = 0;

    /* Match current token with any given types. If true, consume (move to next token) and return true. Otherwise,
     * return false.
     */
    bool match(const std::vector<TokenInfo::Type> &types);
    /* Check if the current token is of the given type.
     * Does not consume, only looks at it.
     */
    bool check(TokenInfo::Type type) const;
    /* Check if next token is EOF (end of source code) */
    bool isAtEnd() const;
    /* Consumes the current token and returns it */
    Token advance();
    /* Returns the current token without consuming it */
    Token peek() const;
    /* Returns the previous token (current - 1) */
    Token previous() const;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    Token consume(TokenInfo::Type type, const std::string &message);
    void error(const Token &token, const std::string &message);

  public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens)
    {
    }
};

#endif
