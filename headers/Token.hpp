#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "TokenInfo.hpp"
#include <string>

class Token
{
    // The actual type of token like keyword, identifier, literal like number or string, etc.
    const TokenInfo::Type type;
    // The actual string in the code that represents the token.
    const std::string lexeme;
    // The value held by the token. Keywords do not have a literal value.
    // The only way to dereference a void pointer is to cast it to another type first.
    void *literal = nullptr;
    // The line number where the token is present.
    const int line;

    /* E.g. " var numb = 5 ; "
     * (var) type = KEYWORD, lexeme = "var", literal = nullptr, line = 1
     * (numb) type = IDENTIFIER, lexeme = "numb", literal = nullptr, line = 1
     * (=) type = EQUAL, lexeme = "=", literal = nullptr, line = 1
     * (5) type = NUMBER, lexeme = "5", literal = 5, line = 1
     * (;) type = SEMICOLON, lexeme = ";", literal = nullptr, line = 1
     */

  public:
    Token(TokenInfo::Type type, const std::string &lexeme, void *literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line)
    {
    }

    // Don't return a reference because a reference comes from the function, and once the function is done being used,
    // the function is "destroyed" and the reference that came from it is now invalid.
    std::string toString() const;
};

#endif // !TOKEN_HPP
