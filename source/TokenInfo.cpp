#include "headers/TokenInfo.hpp"
#include <algorithm>
#include <string>
#include <vector>

// Static keyword does not have to be repeated in the definition.
const std::vector<std::string> TokenInfo::TypeStrings = {

    // Single-character tokens.
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH",
    "STAR",

    // One or two character tokens.
    "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",

    // Literals.
    "IDENTIFIER", "STRING", "NUMBER",

    // Keywords.
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR", "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR",
    "WHILE", "BREAK", "UNINITIALIZED",

    // EOF reserved word
    "END_OF_FILE"};

std::string TokenInfo::getTypeString(Type type)
{
    return TypeStrings[type];
}

TokenInfo::Type TokenInfo::getKeywordOrIdentifier(std::string &type)
{
    // From "and" to "uninitialized" are keywords.
    for (int i = 22; i < 40; i++)
    {
        // Lowercase the string from TypeStrings vector.
        std::string lower = TypeStrings[i];
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        // If the string from TypeStrings vector matches the input string, return the corresponding Type.
        if (lower == type)
        {
            // Cast i to to a Type enum.
            return static_cast<TokenInfo::Type>(i);
        }
    }

    // If no match is found, return IDENTIFIER.
    return TokenInfo::Type::IDENTIFIER;
}
