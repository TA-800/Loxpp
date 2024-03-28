#ifndef PARSER_HPP
#define PARSER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"

/* Grammar:
 *
 * program → declaration* EOF ;
 *
 * declaration → varDeclaration | statement ;
 * varDeclaration → "var" IDENTIFIER ( "=" expression )? ";" ;
 *
 * statement → ifStmt | printStmt | expressionStmt | block ;
 *
 * ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;
 *
 * block → "{" declaration* "}" ;
 *
 * expressionStatement → expression ";" ;
 *
 * expression → assignment ;
 * assignment → IDENTIFIER "=" assignment | equality ;
 * equality → comparison ( ( "!=" | "==" ) comparison )* ;
 * comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 * term → factor ( ( "-" | "+" ) factor )* ;
 * factor → unary ( ( "/" | "*" ) unary )* ;
 * unary → ( "!" | "-" ) unary | primary ;
 * primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER;
 *
 */

/*
 * Reads tokens from the input source code and constructs an Abstract Syntax Tree (AST) out of it.
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

    Token consume(TokenInfo::Type type, const std::string &message);

    /*
     * Expression parsing.
     * Consume tokens to form AST nodes of expressions.
     */

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    // Syncronize the parser after an error
    void synchronize();

    /*
     * Statement parsing.
     * Consume tokens to form AST nodes of statements.
     */

    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> expressionStatement();

  public:
    // Constructor. Takes (a reference to but does not modify) a vector of tokens to parse.
    Parser(const std::vector<Token> &tokens) : tokens(tokens)
    {
    }

    /*
     * Begin parsing the tokens into AST nodes (statements | declarations | expressions ) that represent the source
     * code and can be executed.
     */
    std::vector<std::unique_ptr<Stmt>> parse();
};

#endif
