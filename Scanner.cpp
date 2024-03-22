#include "headers/Scanner.hpp"

std::vector<Token> &Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        // We are at the beginning of the next lexeme.
        start = current;
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

    // Unrecognized character
    default:
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
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
}
