#include "headers/Token.hpp"

std::string Token::toString() const
{

    std::string result = "Type: " + TokenInfo::getTypeString(type) + ", Literal: ";

    if (literal != nullptr)
    {
        if (type == TokenInfo::Type::STRING)
        {
            // Static cast to string type
            std::string str = *(std::static_pointer_cast<std::string>(literal));
            result += str;
        }

        else if (type == TokenInfo::Type::NUMBER)
        {
            double num = *(std::static_pointer_cast<double>(literal));
            result += std::to_string(num);
        }
        else
        {
            result += " Null";
        }
    }

    return result + ", Lexeme: " + lexeme;
}
