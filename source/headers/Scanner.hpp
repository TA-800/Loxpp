#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"

class Scanner
{

    // The source code string
    const std::string source;

    // Collection of tokens from the source code.
    std::vector<Token> tokens;

    // Pointers to keep track of where we are in the source code.

    // Pointer to track beginning of lexeme being scanned.
    int start = 0;
    // Pointer to track current character being scanned.
    int current = 0;
    // Pointer to track current line number, useful for telling user error location
    int line = 1;
    /*
     * To recognize lexemes and convert them into tokens.
     */
    void scanToken();
    /*
     * Helper method to check if we have reached the end of the source code.
     */
    bool isAtEnd();

  private:
    /*
     * Consume char in source code and return it. Move current pointer to next char.
     * For input.
     */
    char advance();

    /*
     * When this is called, tell the scanner to add to its token vector whatever falls between start and current
     * pointers at the moment.
     * For output.
     */
    void addToken(TokenInfo::Type type);
    /*
     * Add token to vector of tokens. To be used when token has a literal value.
     * For output.
     */
    void addToken(TokenInfo::Type type, void *literal);

    /*
     * Check if next char matches expected char.
     * If true, consume it and return true.
     * Else, return false.
     *
     * Example: !=
     * char c = advance(); // c = '!'; current = 1 -> 2
     * if match('=') then source[2] == '=' -> true, consume it, move curr forward, and return true.
     */
    bool match(char expected);

    /*
     * Show me next character but don't consume it, simply return what's next after the currently consumed character.
     * For input.
     */
    char peek();
    char peekNext();

    /*
     * Recognize string literals. Similar to how characters in a comment are continuously being consumed without being
     * saved anywhere to reach end of comment.
     */
    void string();

    void multiLineComment();

    /*
     * Recognize numbers. Similar to string and comment.
     */
    void number();

    /*
     * Recognize identifiers and keywords.
     */
    void identifier();
    // Letters or underscores
    bool isAlpha(char c);
    // Letters, underscores, or digits
    bool isAlphaNumeric(char c);

  public:
    Scanner(const std::string &source) : source(source)
    {
    }

    /*
     * Scan source code char-by-char and return a vector of tokens.
     */
    std::vector<Token> &scanTokens();

    /*
     * Free memory allocated for tokens.
     */
    void freeTokens();
};

#endif // !SCANNER_HPP
