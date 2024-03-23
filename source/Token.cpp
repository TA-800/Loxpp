#include "headers/Token.hpp"

std::string Token::toString() const
{

    std::string result = "Type: " + TokenInfo::getTypeString(type) + ", Literal: ";

    if (literal != nullptr)
    {
        if (type == TokenInfo::Type::STRING)
        {
            // Static cast to string type
            // Cast void pointer to string pointer using static_cast, then dereference it to get the value.
            std::string str = *(static_cast<std::string *>(literal));
            result += str;
        }

        else if (type == TokenInfo::Type::NUMBER)
        {
            int num = *(static_cast<int *>(literal));
            result += std::to_string(num);
        }
        else
        {
            result += " Null";
        }
    }

    return result + ", Lexeme: " + lexeme;
}

void Token::freeLiteral()
{
    if (literal != nullptr)
    {
        if (type == TokenInfo::Type::STRING)
        {
            delete static_cast<std::string *>(literal);
        }
        else if (type == TokenInfo::Type::NUMBER)
        {
            delete static_cast<int *>(literal);
        }
    }
}
