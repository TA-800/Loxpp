#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP

#include "Stmt.hpp"

class AstInterpreter;
class Environment;

/*
 * Instances of this class represent Lox functions.
 * A value of type FUN in an environment is a LoxFunction object.
 * On those LoxFunction object-type values, methods like call() and arity() can be called.
 */
class LoxFunction
{
  public:
    // clonable
    std::unique_ptr<Function> declaration;
    std::shared_ptr<Environment> closure;

    LoxFunction(std::unique_ptr<Function> &declaration, std::shared_ptr<Environment> &closure)
        : declaration(std::move(declaration)), closure(closure)
    {
    }

    // Call method to execute function that could return a value
    std::pair<std::shared_ptr<void>, TokenInfo::Type> call(
        AstInterpreter &interpreter, const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments);

    // Arity method to get the number of parameters the function has
    int arity() const
    {
        return declaration->params.size();
    }

    std::string toString() const
    {
        return "<fn " + declaration->name.getLexeme() + ">";
    }
};

#endif // LOX_FUNCTION_HPP
