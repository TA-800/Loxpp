#ifndef RETURN_HPP
#define RETURN_HPP

#include "TokenInfo.hpp"
#include <memory>
#include <stdexcept>


class ReturnException : public std::runtime_error
{
  public:
    ReturnException(std::shared_ptr<void> &value, TokenInfo::Type type)
        : std::runtime_error("Return statement"), value(value), type(type)
    {
    }

    std::shared_ptr<void> value;
    TokenInfo::Type type;
};

#endif // !RETURN_HPP
