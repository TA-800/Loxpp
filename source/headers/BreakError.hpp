#ifndef BREAK_ERROR_HPP
#define BREAK_ERROR_HPP

#include <stdexcept>

class BreakError : public std::runtime_error
{

  public:
    BreakError() : std::runtime_error("Using exception to break out of a loop.")
    {
        // Should be caught by the interpreter with catch (const BreakError &e) to break out of a loop.
        // Otherwise, it will undesirably terminate the program.
    }
};

#endif // BREAK_ERROR_HPP
