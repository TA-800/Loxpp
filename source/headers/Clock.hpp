#include "LoxCallable.hpp"

class Clock : LoxCallable
{
  public:
    int arity() const override
    {
        return 0;
    }

    std::pair<std::shared_ptr<void>, TokenInfo::Type> call(
        AstInterpreter &interpreter,
        const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments) override
    {
        return std::make_pair(std::make_shared<double>(static_cast<double>(clock()) / CLOCKS_PER_SEC),
                              TokenInfo::Type::NUMBER);
    }

    std::string toString() const override
    {
        return "<native fn>";
    }
};
