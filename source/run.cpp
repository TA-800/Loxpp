#include "headers/Loxpp.hpp"
#include <iostream>

int main(int argc, char *argv[])
{

    // TODO: Fix the AST generation script
    // TODO: Understand string literal lifetimes (esp. from local return values)

    // Check if we are running a script or an interactive session
    if (argc > 2)
    {
        std::cout << "Usage: loxpp [script]"
                  << "\n";
        return 64;
    }
    // Script
    else if (argc == 2)
    {
        int result = Loxpp::runFile(argv[1]);
        return result;
    }
    // Interactive session
    else
    {
        Loxpp::runPrompt();
        // Will not go beyond this point because in interactive session, we are in a loop
        // and the only way to exit is to break the loop with ctrl-c or d, which will
        // terminate the program.
    }

    return 0;
}
