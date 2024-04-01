#include "headers/LoxFunction.hpp"

using namespace std;

std::pair<std::shared_ptr<void>, TokenInfo::Type> LoxFunction::call(
    AstInterpreter &interpreter, const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments)
{
    // Duplicate current environment and make it into new one for function onto which local params will be added
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(interpreter.getGlobals());

    for (int i = 0; i < declaration->params.size(); i++)
    {
        // Define the parameters in the new environment.
        environment->defineVar(declaration->params[i].getLexeme(), arguments[i].first, arguments[i].second);
    }

    interpreter.executeBlock(declaration->body, environment);

    return std::make_pair(nullptr, TokenInfo::Type::NIL);
}
