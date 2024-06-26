#include "headers/Scanner.hpp"
#include "headers/Loxpp.hpp"
#include <cctype>
#include <string>

std::vector<Token> &Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        // We are at the beginning of the next lexeme.

        // Keep pushing start pointer forward to the next lexeme
        // as we finish scanning the current lexeme into a token.
        start = current;

        // Scan token begins reading until a token is scanned and added to tokens list. It will push current forward
        // with advance().
        scanToken();
    }

    auto nilptr = std::shared_ptr<void>();
    tokens.push_back(Token(TokenInfo::Type::END_OF_FILE, "", nilptr, line));

    return tokens;
}

bool Scanner::isAtEnd()
{

    return current >= source.length();
}

void Scanner::scanToken()
{

    char c = advance();

    switch (c)
    {

    // Single character lexemes
    case '(':
        addToken(TokenInfo::Type::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenInfo::Type::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenInfo::Type::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenInfo::Type::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenInfo::Type::COMMA);
        break;
    case '.':
        addToken(TokenInfo::Type::DOT);
        break;
    case '-':
        addToken(TokenInfo::Type::MINUS);
        break;
    case '+':
        addToken(TokenInfo::Type::PLUS);
        break;
    case ';':
        addToken(TokenInfo::Type::SEMICOLON);
        break;
    case '*':
        addToken(TokenInfo::Type::STAR);
        break;

    // Double character lexemes
    case '!':
        addToken(match('=') ? TokenInfo::Type::BANG_EQUAL : TokenInfo::Type::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenInfo::Type::EQUAL_EQUAL : TokenInfo::Type::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenInfo::Type::LESS_EQUAL : TokenInfo::Type::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenInfo::Type::GREATER_EQUAL : TokenInfo::Type::GREATER);
        break;

    // Comment or division
    case '/':
        // Comment
        if (match('/'))
        {
            // As long as we are on the same line and it's not the endof the source code
            while (peek() != '\n' && !isAtEnd())
                // Don't add comments to tokens
                // Just simply advance the current pointer
                advance();
        }
        // Multi-line comment
        else if (match('*'))
        {
            multiLineComment();
        }
        // Division
        else
        {
            addToken(TokenInfo::Type::SLASH);
        }
        break;

    // Ignore whitespace. This will end loop and move to next lexeme in the outer while loop.
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;

    // String literals
    // When double quote encountered, string literal begins
    case '\"':
        string();
        break;

    // Unrecognized character
    default:
        if (isdigit(c))
        {
            number();
        }
        else if (isAlpha(c))
        {
            identifier();
        }
        else
        {
            Loxpp::error(line, "Unexpected character.");
        }
        break;
    }
}

char Scanner::advance()
{
    return source[current++];
}

void Scanner::addToken(TokenInfo::Type type)
{
    auto nilptr = std::shared_ptr<void>();
    addToken(type, nilptr);
}

void Scanner::addToken(TokenInfo::Type type, std::shared_ptr<void> &literal)
{
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
}

bool Scanner::match(char expected)
{
    if (isAtEnd() || source[current] != expected)
        return false;

    current++;
    return true;
}

char Scanner::peek()
{
    // If we are at the end of the source code
    if (isAtEnd())
        return '\0';

    // Return the next character
    return source[current];
}

char Scanner::peekNext()
{
    // If we are at the end of the source code
    if (current + 1 >= source.length())
        return '\0';

    // Return the next character
    return source[current + 1];
}

void Scanner::string()
{

    // As long as we are not at the end of the source code and we haven't encountered the closing double quote
    while (peek() != '"' && !isAtEnd())
    {
        // If we encounter a newline, we are in a multi-line string
        if (peek() == '\n')
            line++;

        // Consume but don't save it anywhere to add to tokens
        advance();
    }

    // Unterminated string
    if (isAtEnd())
    {
        Loxpp::error(line, "Unterminated string.");
        return;
    }

    // The closing quote. Consume it.
    advance();

    // Remove surrounding quotes to add purely the string value to tokens

    // Create string in memory
    std::string str = source.substr(start + 1, current - (start + 1) - 1);
    std::shared_ptr<void> string = std::make_shared<std::string>(str);
    addToken(TokenInfo::Type::STRING, string);
}

void Scanner::number()
{
    // Keep consuming (moving current forward) until we find something not a digit
    while (isdigit(peek()))
        advance();

    // If we encounter decimal point and next char after is digit,
    // then continue consumption until we find something not a digit
    if (peek() == '.' && isdigit(peekNext()))
    {
        // Consume the '.'
        advance();

        // Consume the digit after the '.'
        while (isdigit(peek()))
            advance();
    }

    // Parse string into double and store in tokens
    std::shared_ptr<void> number = std::make_shared<double>(std::stod(source.substr(start, current)));
    addToken(TokenInfo::Type::NUMBER, number);
}

void Scanner::multiLineComment()
{

    int nesting = 1;

    while (!isAtEnd())
    {
        if (peek() == '*' && peekNext() == '/')
        {
            nesting--;

            if (nesting == 0)
            {
                // Consume the '*'
                // Consume the '/'
                advance();
                advance();
                return;
            }
        }

        if (peek() == '/' && peekNext() == '*')
        {
            nesting++;
            // Consume the '/'
            // Consume the '*'
            advance();
            advance();
            continue;
        }

        if (peek() == '\n')
            line++;

        advance();
    }

    // Unterminated multi-line comment
    Loxpp::error(line, nesting == 1 ? "Unterminated multi-line comment." : "Unterminated nested multi-line comment");
    return;
}

bool Scanner::isAlpha(char c)
{
    return isalpha(c) || c == '_';
}

bool Scanner::isAlphaNumeric(char c)
{
    return isAlpha(c) || isdigit(c);
}

void Scanner::identifier()
{
    // Keep consuming until we find something not an alphanumeric character
    while (isAlphaNumeric(peek()))
        advance();

    // Check if current lexeme between start & current is a reserved keyword with TokenInfo typeString
    std::string text = source.substr(start, current - start);
    TokenInfo::Type type = TokenInfo::getKeywordOrIdentifier(text);
    addToken(type);
}
