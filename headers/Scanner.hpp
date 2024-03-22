#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"

class Scanner
{

    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

  public:
    Scanner(const std::string &source) : source(source)
    {
    }

    /*
     * To recognize lexemes and convert them into tokens.
     */
    void scanToken();
    /*
     * Scan source code char-by-char and return a vector of tokens.
     */
    std::vector<Token> &scanTokens();
    /*
     * Check if we have reached the end of the source code.
     */
    bool isAtEnd();

  private:
    /*
     * Consume char in source code and return it. Move current pointer to next char.
     * For input.
     */
    char advance();

    /*
     * Add token to vector of tokens.
     * For output.
     */
    void addToken(TokenInfo::Type type);
    /*
     * Add token to vector of tokens. To be used when token has a literal value.
     * For output.
     */
    void addToken(TokenInfo::Type type, void *literal);
};

#endif // !SCANNER_HPP
