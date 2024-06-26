#include "headers/LoxFunction.hpp"
#include "headers/AstInterpreter.hpp"
#include "headers/Environment.hpp"
#include "headers/ReturnException.hpp"

std::pair<std::shared_ptr<void>, TokenInfo::Type> LoxFunction::call(
    AstInterpreter &interpreter, const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments)
{
    // Make closure (the env that was active during function definition) the environment of the function execution
    std::shared_ptr<Environment> funcEnv = std::make_shared<Environment>(closure);

    for (int i = 0; i < declaration->params.size(); i++)
    {
        // Define the parameters in the new environment. Set arguments to be values of the parameters
        funcEnv->defineVar(declaration->params[i].getLexeme(), arguments[i].first, arguments[i].second);
    }

    try
    {
        interpreter.executeBlock(declaration->body, funcEnv);
    }
    catch (const ReturnException &e)
    {
        return std::make_pair(e.value, e.type);
    }

    return std::make_pair(nullptr, TokenInfo::Type::NIL);
}
