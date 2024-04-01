#ifndef LOX_CALLABLE_HPP
#define LOX_CALLABLE_HPP

#include "AstInterpreter.hpp"
#include <memory>

class LoxCallable
{
  public:
    virtual ~LoxCallable() = default;

    virtual int arity() const = 0;
    virtual std::pair<std::shared_ptr<void>, TokenInfo::Type> call(
        AstInterpreter &interpreter,
        const std::vector<std::pair<std::shared_ptr<void>, TokenInfo::Type>> &arguments) = 0;
    virtual std::string toString() const = 0; // TODO: avoid copy
};
#endif // LOX_CALLABLE_HPP
