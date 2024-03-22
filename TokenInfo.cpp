#include "headers/TokenInfo.hpp"
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
    "WHILE",

    // EOF reserved word
    "END_OF_FILE"};

std::string TokenInfo::getTypeString(Type type)
{
    return TypeStrings[type];
}
