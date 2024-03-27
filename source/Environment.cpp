#include "headers/Environment.hpp"
#include "headers/RuntimeError.hpp"

std::pair<std::shared_ptr<void>, TokenInfo::Type> Environment::get(Token name)
{
    // Don't use values[ ] because it will create a new entry if it doesn't exist.
    if (values.find(name.getLexeme()) != values.end())
    {
        return values[name.getLexeme()];
    }

    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}

void Environment::define(std::string name, std::shared_ptr<void> value, TokenInfo::Type type)
{
    values[name] = std::make_pair(value, type);
}

// Key difference: do not create a new var if it doesn't exist
void Environment::assign(Token name, std::shared_ptr<void> value, TokenInfo::Type type)
{
    if (values.find(name.getLexeme()) != values.end())
    {
        values[name.getLexeme()] = std::make_pair(value, type);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}
