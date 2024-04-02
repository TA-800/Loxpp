#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "LoxFunction.hpp"
#include "Token.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

class Environment
{

    // Pointer to the enclosing environment.
    std::shared_ptr<Environment> enclosing;
    // Don't use unique_ptr for enclosing because e.g. local env pointing to global env, local destroyed at some point,
    // global should still exist

    // Store variables in a hash table.
    // Key: variable, Value: pair of value and type
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenInfo::Type>> values;

    std::shared_ptr<void> makeCopy(std::shared_ptr<void> &value, TokenInfo::Type type)
    {
        switch (type)
        {
        case TokenInfo::Type::STRING:
            return std::make_shared<std::string>(*std::static_pointer_cast<std::string>(value));
        case TokenInfo::Type::NUMBER:
            return std::make_shared<double>(*std::static_pointer_cast<double>(value));
        case TokenInfo::Type::TRUE:
        case TokenInfo::Type::FALSE:
            return std::make_shared<bool>(*std::static_pointer_cast<bool>(value));
        default:
            return nullptr;
        }
    }

  public:
    // For global environment.
    Environment() : enclosing(nullptr)
    {
    }

    // For local environments.
    Environment(std::shared_ptr<Environment> &enclosing) : enclosing(enclosing)
    {
    }

    // Get the value of a variable in the current (local) environment.
    std::pair<std::shared_ptr<void>, TokenInfo::Type> get(Token name);

    // Define a variable in the current environment.
    void defineVar(std::string name, std::shared_ptr<void> value, TokenInfo::Type type);

    // Assign a new value to a variable in the current environment.
    void assign(Token name, std::shared_ptr<void> &value, TokenInfo::Type type);

    // Clone (for function calls)
    std::shared_ptr<Environment> clone()
    {

        std::shared_ptr<Environment> newEnv = std::make_shared<Environment>(enclosing);

        for (auto &[key, value] : values)
        {
            // If the value is a function, clone it.
            // This is necessary because functions have a unique_pointer to their declaration.

            if (value.second == TokenInfo::Type::FUN)
            {
                auto callable = std::static_pointer_cast<LoxFunction>(value.first);

                // Clone the function

                std::unique_ptr<Function> clonedFunc =
                    std::unique_ptr<Function>(static_cast<Function *>(callable->declaration->clone().release()));
                std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(clonedFunc);

                // Save the cloned function to the new environment

                newEnv->defineVar(key, function, TokenInfo::Type::FUN);
            }
            else
            {
                std::shared_ptr<void> valueCopy = makeCopy(value.first, value.second);
                newEnv->defineVar(key, valueCopy, value.second);
            }
        }

        return newEnv;
    }
};

#endif
