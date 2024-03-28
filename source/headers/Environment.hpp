#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "Token.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Environment
{

    // Pointer to the enclosing environment.
    const std::shared_ptr<Environment> enclosing;
    // Don't use unique_ptr for enclosing because e.g. local env pointing to global env, local destroyed at some point,
    // global should still exist

    // Store variables in a hash table.
    // Key: variable
    // Value: pair of value and type
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenInfo::Type>> values;

  public:
    // For global environment.
    Environment() : enclosing(nullptr)
    {
    }

    // For local environments.
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing)
    {
    }

    // Get the value of a variable in the current (local) environment.
    std::pair<std::shared_ptr<void>, TokenInfo::Type> get(Token name);

    // Define a variable in the current environment.
    void define(std::string name, std::shared_ptr<void> value, TokenInfo::Type type);

    // Assign a new value to a variable in the current environment.
    void assign(Token name, std::shared_ptr<void> value, TokenInfo::Type type);
};

#endif
