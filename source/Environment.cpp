#include "headers/Environment.hpp"
#include "headers/LoxCallable.hpp"
#include "headers/RuntimeError.hpp"

std::pair<std::shared_ptr<void>, TokenInfo::Type> Environment::get(Token name)
{

    // Don't use values[ ] because it will create a new entry if it doesn't exist.
    if (values.find(name.getLexeme()) != values.end())
        return values[name.getLexeme()];

    // Check enclosing env recursively.
    if (enclosing != nullptr)
        return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}

std::shared_ptr<LoxCallable> Environment::getCallable(Token name)
{

    if (callables.find(name.getLexeme()) != callables.end())
        return callables[name.getLexeme()];

    if (enclosing != nullptr)
        return enclosing->getCallable(name);

    throw RuntimeError(name, "Undefined function '" + name.getLexeme() + "'.");
}

void Environment::defineVar(std::string name, std::shared_ptr<void> value, TokenInfo::Type type)
{
    values[name] = std::make_pair(value, type);
}

void Environment::defineFun(std::string name, std::shared_ptr<LoxCallable> &callable)
{
    callables[name] = callable;
}

// Key difference: do not create a new var if it doesn't exist
void Environment::assign(Token name, std::shared_ptr<void> &value, TokenInfo::Type type)
{
    if (values.find(name.getLexeme()) != values.end())
    {
        values[name.getLexeme()] = std::make_pair(value, type);
        return;
    }

    // Check enclosing env recursively.
    if (enclosing != nullptr)
    {
        enclosing->assign(name, value, type);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
}
