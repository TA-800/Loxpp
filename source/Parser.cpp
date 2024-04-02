#include "headers/Parser.hpp"
#include "headers/Loxpp.hpp"
#include "headers/ParserError.hpp"
#include "headers/Stmt.hpp"
#include <memory>

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

// declaration → varDeclaration | statement ;
std::unique_ptr<Stmt> Parser::declaration()
{
    try
    {
        // If the current token is a var, then it is a variable declaration
        if (match({TokenInfo::Type::VAR}))
            return varDeclaration();

        if (match({TokenInfo::Type::FUN}))
            return function("function");

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

// function → "fun" IDENTIFIER "(" parameters? ")" block ;
// parameters → IDENTIFIER ( "," IDENTIFIER )* ;
std::unique_ptr<Function> Parser::function(const std::string &kind)
{
    // Get name of function
    Token name = consume(TokenInfo::Type::IDENTIFIER, "Expect " + kind + " name.");

    // Parse param list
    consume(TokenInfo::Type::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Token> params;

    if (!check(TokenInfo::Type::RIGHT_PAREN))
    {
        do // Parse param identifiers one by one and add to list
        {
            if (params.size() >= 255)
            {
                Loxpp::error(peek(), "Cannot have more than 255 parameters.");
            }

            params.push_back(consume(TokenInfo::Type::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenInfo::Type::COMMA})); // as long as there are commas
    }

    consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after parameters.");

    // Parse function body (block code)
    consume(TokenInfo::Type::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    std::vector<std::unique_ptr<Stmt>> body = block();

    // After we get block, we now have function (name) with params and body.
    // Create function node for the AST
    return std::make_unique<Function>(name, params, body);
}

// varDeclaration → "var" IDENTIFIER ( "=" expression )? ";" ;
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
    return std::make_unique<Var>(name, initializer);
}

std::unique_ptr<Stmt> Parser::statement()
{
    // Check if current token is an IF statement
    if (match({TokenInfo::Type::IF}))
        return ifStatement();

    // Check if the current token is a return statement
    if (match({TokenInfo::Type::RETURN}))
        return returnStatement();

    // Check if the current token is a print statement
    if (match({TokenInfo::Type::PRINT}))
        return printStatement();

    // Check if the current token is a for statement
    if (match({TokenInfo::Type::FOR}))
    {
        auto forStmt = forStatement();
        loopDepth--;
        return forStmt;
    }

    // Check if the current token is a while statement
    if (match({TokenInfo::Type::WHILE}))
    {
        auto whileStmt = whileStatement();
        loopDepth--;
        return whileStmt;
    }

    // Check if the current token is a break statement
    if (match({TokenInfo::Type::BREAK}))
        return breakStatement();

    if (match({TokenInfo::Type::LEFT_BRACE}))
    {
        // Block is a type of statement (containing multiple statements)
        auto b = block();
        return std::make_unique<Block>(b);
    }

    // Otherwise, it is an expression statement
    return expressionStatement();
}

std::vector<std::unique_ptr<Stmt>> Parser::block()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenInfo::Type::RIGHT_BRACE) && !isAtEnd())
    {
        statements.push_back(declaration());
    }

    consume(TokenInfo::Type::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

// ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;
std::unique_ptr<Stmt> Parser::ifStatement()
{

    consume(TokenInfo::Type::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after 'if'.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenInfo::Type::ELSE}))
        elseBranch = statement();

    return std::make_unique<If>(condition, thenBranch, elseBranch);
}

// forStmt → "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
std::unique_ptr<Stmt> Parser::forStatement()
{

    consume(TokenInfo::Type::LEFT_PAREN, "Expect '(' after 'for'.");

    // INITIALIZER --------

    std::unique_ptr<Stmt> initializer;
    if (match({TokenInfo::Type::SEMICOLON}))
        initializer = nullptr;

    else if (match({TokenInfo::Type::VAR}))
        initializer = varDeclaration();

    else
        initializer = expressionStatement();

    // CONDITION --------

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenInfo::Type::SEMICOLON))
        condition = expression();

    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after loop condition.");

    // INCREMENT --------

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenInfo::Type::RIGHT_PAREN))
        increment = expression();

    consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after for clauses.");

    // BODY --------

    loopDepth++;
    std::unique_ptr<Stmt> body = statement();

    // DESUGAR --------
    // Use while loop to simulate for loop
    // Take the whole for loop clauses and body and put them in a while loop

    std::vector<std::unique_ptr<Stmt>> whileBody;
    whileBody.push_back(std::move(body));
    if (increment != nullptr)
    {
        // Take the increment assignment (e.g. i = i + 1) and make it the last thing to take place in the while
        // (disguised as for) loop
        whileBody.push_back(std::make_unique<Expression>(increment));
    }

    if (condition == nullptr)
    {
        // If there is no condition, make it true (infinite loop)
        auto nilptr = std::shared_ptr<void>();
        condition = std::make_unique<Literal>(nilptr, TokenInfo::Type::TRUE);
    }

    // Convert whileBody to a block statement
    std::unique_ptr<Stmt> whileBodyBlock = std::make_unique<Block>(whileBody);

    // Take the condition and body and put them in a while loop
    std::unique_ptr<Stmt> whileLoop = std::make_unique<While>(condition, whileBodyBlock);

    // Construct the final for loop statement
    std::vector<std::unique_ptr<Stmt>> forStmt;
    if (initializer != nullptr)
    {
        // If there is an initializer, put it before the while loop.
        // Compile everything into a block statement -> initializer -> while with condition { body }
        forStmt.push_back(std::move(initializer)); // initializer
    }
    forStmt.push_back(std::move(whileLoop)); // while loop

    return std::make_unique<Block>(forStmt); // convert into a block statement
}

// whileStmt → "while" "(" expression ")" statement ;
std::unique_ptr<Stmt> Parser::whileStatement()
{
    consume(TokenInfo::Type::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after condition.");

    loopDepth++;
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<While>(condition, body);
}

// breakStmt → "break" ";" ;
std::unique_ptr<Stmt> Parser::breakStatement()
{
    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after 'break'.");

    if (loopDepth == 0) // If there is no loop to break out of
        Loxpp::error(previous(), "Cannot use 'break' outside of a loop.");

    return std::make_unique<Break>();
}

// returnStmt → "return" expression? ";" ;
std::unique_ptr<Stmt> Parser::returnStatement()
{
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;

    // If there is a value to return
    if (!check(TokenInfo::Type::SEMICOLON))
        value = expression();

    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after return value.");

    return std::make_unique<Return>(keyword, value);
}

// printStatement → "print" expression ";" ;
std::unique_ptr<Stmt> Parser::printStatement()
{
    // Some expression whose value has been computed (interpreted)
    std::unique_ptr<Expr> value = expression();

    // After a statement, there should be a semicolon
    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after value.");

    // Return a valid statement
    return std::make_unique<Print>(value);
}

// expressionStatement → expression ";" ;
std::unique_ptr<Stmt> Parser::expressionStatement()
{
    // Some expression that has been formed into an AST Expr node
    std::unique_ptr<Expr> value = expression();

    // After a statement, there should be a semicolon
    consume(TokenInfo::Type::SEMICOLON, "Expect ';' after expression.");

    // Return a valid statement
    return std::make_unique<Expression>(value);
}

std::unique_ptr<Expr> Parser::expression()
{
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
    std::unique_ptr<Expr> expr = logicalOr();

    // If we have an equal sign, then it is an assignment
    if (match({TokenInfo::Type::EQUAL}))
    {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        // Check if expr is a variable expression
        // If it was then expr returned from equality would be pointer of Variable class
        // Reference: https://stackoverflow.com/a/307801
        if (dynamic_cast<Variable *>(expr.get()) != nullptr)
        {
            Token name = static_cast<Variable *>(expr.get())->name;
            return std::make_unique<Assign>(name, value);
        }

        // If it was not a variable expression, and we're at this point (because we had a equals '=' sign for
        // assignment) then error
        Loxpp::error(equals, "Invalid assignment target.");
        // don't throw error because parser is not in "confused" state
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalOr()
{
    std::unique_ptr<Expr> expr = logicalAnd();

    while (match({TokenInfo::Type::OR}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = logicalAnd();
        expr = std::make_unique<Logical>(expr, op, right);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd()
{
    std::unique_ptr<Expr> expr = equality();

    while (match({TokenInfo::Type::AND}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<Logical>(expr, op, right);
    }

    return expr;
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
        expr = std::make_unique<Binary>(expr, op, right);
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
        expr = std::make_unique<Binary>(expr, op, right);
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
        expr = std::make_unique<Binary>(expr, op, right);
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
        expr = std::make_unique<Binary>(expr, op, right);
    }

    return expr;
}

// unary → ( "!" | "-" ) unary | call ;
std::unique_ptr<Expr> Parser::unary()
{
    if (match({TokenInfo::Type::BANG, TokenInfo::Type::MINUS}))
    {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, right);
    }
    else
        return call();
}

// call → primary ( "(" arguments? ")" )* ;
std::unique_ptr<Expr> Parser::call()
{
    std::unique_ptr<Expr> expr = primary();

    while (true)
    {
        if (match({TokenInfo::Type::LEFT_PAREN}))
        {
            expr = finishCall(expr);
        }
        else
            break;
    }

    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> &callee)
{
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenInfo::Type::RIGHT_PAREN))
    {
        do
        {
            if (arguments.size() >= 255)
                Loxpp::error(peek(), "Cannot have more than 255 arguments.");
            arguments.push_back(expression());

        } while (match({TokenInfo::Type::COMMA}));
    }

    Token paren = consume(TokenInfo::Type::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<Call>(callee, paren, arguments);
}

// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER;
std::unique_ptr<Expr> Parser::primary()
{
    if (match({TokenInfo::Type::FALSE}))
    {
        auto nilptr = std::shared_ptr<void>();
        return std::make_unique<Literal>(nilptr, TokenInfo::Type::FALSE);
    }
    if (match({TokenInfo::Type::TRUE}))
    {
        auto nilptr = std::shared_ptr<void>();
        return std::make_unique<Literal>(nilptr, TokenInfo::Type::TRUE);
    }
    if (match({TokenInfo::Type::NIL}))
    {
        auto nilptr = std::shared_ptr<void>();
        return std::make_unique<Literal>(nilptr, TokenInfo::Type::NIL);
    }

    if (match({TokenInfo::Type::NUMBER, TokenInfo::Type::STRING}))
    {
        auto prevLiteral = previous().getLiteral();
        return std::make_unique<Literal>(prevLiteral, previous().getType());
    }

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
        return std::make_unique<Grouping>(expr);
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
