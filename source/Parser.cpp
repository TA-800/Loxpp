#include "headers/Parser.hpp"
#include "headers/ParserError.hpp"

Token Parser::previous() const
{
    return tokens[current - 1];
}

Token Parser::peek() const
{
    return tokens[current];
}

Token Parser::advance()
{
    // Move foward
    if (!isAtEnd())
    {
        current++;
    }

    // and return the previous token
    return previous();
}

bool Parser::isAtEnd() const
{
    return peek().getType() == TokenInfo::Type::END_OF_FILE;
}

bool Parser::check(TokenInfo::Type type) const
{
    if (isAtEnd())
    {
        return false;
    }

    return peek().getType() == type;
}

bool Parser::match(const std::vector<TokenInfo::Type> &types)
{
    for (const auto &type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

// Run tokens with grammar rules to form expressions
// TODO: Ternary grammar rule

std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
    // Once we have entered this function (because "var" token was matched), we expect an identifier
    Token name = consume(TokenInfo::Type::IDENTIFIER, "Expect variable name.");

    // The value it is initialized to (value can come from an expression -> literal like 1, 2, "hello" or binary like 1
    // + 2 etc.)
    std::unique_ptr<Expr> initializer = nullptr;

    if (match({TokenInfo::Type::EQUAL}))
        initializer = expression();

    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after variable declaration.");

    // Create a vari
    return std::make_unique<Var>(name, std::move(initializer));
}

// declaration → "var" IDENTIFIER ( "=" expression )? ";" ;
std::unique_ptr<Stmt> Parser::declaration()
{
    try
    {
        // If the current token is a var, then it is a variable declaration
        if (match({TokenInfo::Type::VAR}))
            return varDeclaration();

        // Else some other kind of statement (print, expression, etc.)
        return statement();
    }
    catch (const ParserError &e)
    {
        // If there was an error in parsing tokens, don't throw error but attempt to
        // "synchronize"
        synchronize();

        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::statement()
{
    // Check if the current token is a print statement
    if (match({TokenInfo::Type::PRINT}))
        return printStatement();

    // Otherwise, it is an expression statement
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement()
{
    // Some expression whose value has been computed (interpreted)
    std::unique_ptr<Expr> value = expression();

    // After a statement, there should be a semicolon
    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after value.");

    // Return a valid statement
    return std::make_unique<Print>(std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
    // Some expression whose value has been computed (interpreted)
    std::unique_ptr<Expr> value = expression();

    // After a statement, there should be a semicolon
    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after expression.");

    // Return a valid statement
    return std::make_unique<Expression>(std::move(value));
}

// equality → comparison ( ( "!=" | "==" ) comparison )* ;
std::unique_ptr<Expr> Parser::equality()
{

    std::unique_ptr<Expr> expr = comparison();

    // while the current token is either '!=' or '=='
    while (match({TokenInfo::Type::BANG_EQUAL, TokenInfo::Type::EQUAL_EQUAL}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
std::unique_ptr<Expr> Parser::comparison()
{
    std::unique_ptr<Expr> expr = term();

    while (match(
        {TokenInfo::Type::GREATER, TokenInfo::Type::GREATER_EQUAL, TokenInfo::Type::LESS, TokenInfo::Type::LESS_EQUAL}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// term → factor ( ( "-" | "+" ) factor )* ;
std::unique_ptr<Expr> Parser::term()
{
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenInfo::Type::MINUS, TokenInfo::Type::PLUS}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// factor → unary ( ( "/" | "*" ) unary )* ;
std::unique_ptr<Expr> Parser::factor()
{
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenInfo::Type::SLASH, TokenInfo::Type::STAR}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// unary → ( "!" | "-" ) unary | primary ;
std::unique_ptr<Expr> Parser::unary()
{
    if (match({TokenInfo::Type::BANG, TokenInfo::Type::MINUS}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }
    else
    {
        return primary();
    }
}

// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
std::unique_ptr<Expr> Parser::primary()
{
    if (match({TokenInfo::Type::FALSE}))
        return std::make_unique<Literal>(nullptr, TokenInfo::Type::FALSE);
    if (match({TokenInfo::Type::TRUE}))
        return std::make_unique<Literal>(nullptr, TokenInfo::Type::TRUE);
    if (match({TokenInfo::Type::NIL}))
        return std::make_unique<Literal>(nullptr, TokenInfo::Type::NIL);

    if (match({TokenInfo::Type::NUMBER, TokenInfo::Type::STRING}))
        return std::make_unique<Literal>((void *)previous().getLiteral(), previous().getType());

    // If we find an identifier, then it is a variable
    if (match({TokenInfo::Type::IDENTIFIER}))
        return std::make_unique<Variable>(previous());

    // If we find a left parenthesis, we should find right else throw an error
    if (match({TokenInfo::Type::LEFT_PAREN}))
    {
        // After finding left paren, parse the inside expression (will consume tokens)
        std::unique_ptr<Expr> expr = expression();

        // After parsing the expression, we should find a right parenthesis
        consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after expression.");

        // If we've reached here, we've found a valid grouping
        return std::make_unique<Grouping>(std::move(expr));
    }

    // If we've reached here, then throw an error because we couldn't find a valid expression to put into AST
    throw ParserError(peek(), "Expect expression.");
}

Token Parser::consume(TokenInfo::Type type, const std::string &message)
{
    if (check(type))
    {
        return advance();
    }

    throw ParserError(peek(), message);
}

void Parser::synchronize()
{
    advance();

    // Keep consuming tokens until we find a statement boundary
    while (!isAtEnd())
    {
        if (previous().getType() == TokenInfo::Type::SEMICOLON)
            return;

        switch (peek().getType())
        {
        case TokenInfo::Type::CLASS:
        case TokenInfo::Type::FUN:
        case TokenInfo::Type::VAR:
        case TokenInfo::Type::FOR:
        case TokenInfo::Type::IF:
        case TokenInfo::Type::WHILE:
        case TokenInfo::Type::PRINT:
        case TokenInfo::Type::RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}

// Parse

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    try
    {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!isAtEnd())
        {
            statements.push_back(declaration());
        }

        return statements;
    }
    catch (const ParserError &error)
    {
        return {};
    }
}
