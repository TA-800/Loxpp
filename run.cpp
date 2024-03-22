#include "headers/Loxpp.hpp"

int main(int argc, char *argv[])
{

    // Check if we are running a script or an interactive session
    if (argc > 1)
    {
        std::cout << "Usage: loxpp [script]"
                  << "\n";
        exit(64);
    }
    // Script
    else if (argc == 1)
    {
        Loxpp::runFile(argv[1]);
    }
    // Interactive session
    else
    {
        Loxpp::runPrompt();
    }

    return 0;
}
