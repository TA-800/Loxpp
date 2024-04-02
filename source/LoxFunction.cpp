#include "headers/LoxFunction.hpp"
#include "headers/AstInterpreter.hpp"
#include "headers/ReturnException.hpp"


std::pair<std::shared_ptr<void>, TokenInfo::Type> LoxFunction::call(
    AstInterpreter &interpreter, const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments)
{
    // Duplicate current environment and make it into new one for function onto which local params will be added
    // TODO: Create new duplicate, not a reference
    std::shared_ptr<Environment> environment = interpreter.getGlobals();

    for (int i = 0; i < declaration->params.size(); i++)
    {
        // Define the parameters in the new environment. Set arguments to be values of the parameters
        environment->defineVar(declaration->params[i].getLexeme(), arguments[i].first, arguments[i].second);
    }

    try
    {
        interpreter.executeBlock(declaration->body, environment);
    }
    catch (const ReturnException &e)
    {
        return std::make_pair(e.value, e.type);
    }

    return std::make_pair(nullptr, TokenInfo::Type::NIL);
}
