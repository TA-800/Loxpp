#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "Token.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Environment
{

    // Store variables in a hash table.
    // Key: variable
    // Value: pair of value and type
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenInfo::Type>> values;

  public:
    // Get the value of a variable in the current environment.
    std::pair<std::shared_ptr<void>, TokenInfo::Type> get(Token name);

    // Define a variable in the current environment.
    void define(std::string name, std::shared_ptr<void> value, TokenInfo::Type type);

    // Assign a new value to a variable in the current environment.
    void assign(Token name, std::shared_ptr<void> value, TokenInfo::Type type);
};

#endif
