#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "Token.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

class Environment
{

    // Pointer to the enclosing environment.
    const std::shared_ptr<Environment> enclosing;
    // Don't use unique_ptr for enclosing because e.g. local env pointing to global env, local destroyed at some point,
    // global should still exist

    // Store variables in a hash table.
    // Key: variable, Value: pair of value and type
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, TokenInfo::Type>> values;

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
    std::shared_ptr<Environment> clone(std::shared_ptr<Environment> &toClone)
    {
        std::shared_ptr<Environment> newEnv = std::make_shared<Environment>(toClone->enclosing);

        for (auto &[key, value] : toClone->values)
        {

            // If the value is a function, clone it.
            // This is necessary because functions have a unique_pointer to their declaration.

            // TODO: Classes?
            if (value.second == TokenInfo::Type::FUN)
            {
                // When saved to dict, it was upcasted to LoxCallable before casted to void
                // So, we need to downcast it to LoxCallable before casting to LoxFunction

                /* auto callable =
                 * std::static_pointer_cast<LoxFunction>(std::static_pointer_cast<LoxCallable>(value.first)); */

                // Clone the function

                /* std::unique_ptr<Function> clonedFunc = std::unique_ptr<Function>(static_cast<Function
                 * *>(callable->declaration->clone().release())); */

                // Create a new LoxFunction object with the cloned function (cast it to LoxCallable)

                /* std::shared_ptr<LoxCallable> clonedCallable = std::make_shared<LoxFunction>(clonedFunc); */

                // Save the cloned function to the new environment

                /* newEnv->defineVar(key, clonedCallable, TokenInfo::Type::FUN); */
            }
            else
            {
                newEnv->defineVar(key, value.first, value.second);
            }
        }
    }
};

#endif
