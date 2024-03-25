#include "Loxpp.hpp"
#include "Token.hpp"
#include <stdexcept>

class ParserError : public std::runtime_error
{

  public:
    ParserError(const Token &token, const std::string &message) : std::runtime_error(message)
    {
        Loxpp::error(token, message);

        // And from here an error would be thrown that terminates the program unless caught with a try-catch block
    }
};
