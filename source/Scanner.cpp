#include "headers/Scanner.hpp"
#include "headers/Loxpp.hpp"

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

    tokens.push_back(Token(TokenInfo::Type::END_OF_FILE, "", nullptr, line));

    return tokens;
}

bool Scanner::isAtEnd()
{

    return current >= source.length();
}

void Scanner::scanToken()
{

    // Begin with single character lexemes that could be tokens
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

    // Single or double character lexemes
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
        // Division
        else
        {
            addToken(TokenInfo::Type::SLASH);
        }

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
        Loxpp::error(line, "Unexpected character.");
        break;
    }
}

char Scanner::advance()
{
    return source[current++];
}

void Scanner::addToken(TokenInfo::Type type)
{
    addToken(type, nullptr);
}

void Scanner::addToken(TokenInfo::Type type, void *literal)
{
    std::string text = source.substr(start, current);
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
    std::string str = source.substr(start + 1, current - 2);
    void *string = new std::string(str);
    addToken(TokenInfo::Type::STRING, string);
}

void Scanner::freeTokens()
{
    for (Token token : tokens)
    {
        token.freeLiteral();
    }
}
