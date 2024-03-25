#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include "Token.hpp"
#include <stdexcept>

class RuntimeError : public std::runtime_error
{

  public:
    RuntimeError(Token token, std::string message) : token(token), std::runtime_error(message)
    {
        // And from here an error would be thrown that terminates the program unless caught with a try-catch block
    }
    Token token;
};

#endif // !RUNTIMEERROR_HPP
