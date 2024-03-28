#include <string>
#include <vector>

/*
 * Utility class simply to hold information about tokens.
 * Is not meant to be instantiated. Just accessed for information like what possible types of tokens exist in Loxpp.
 */
class TokenInfo
{
    const static std::vector<std::string> TypeStrings;

  public:
    enum Type
    {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
        UNINITIALIZED,

        // EOF reserved word
        END_OF_FILE
    };

    static std::string getTypeString(Type type);

    static TokenInfo::Type getKeywordOrIdentifier(std::string &text);
};
