#ifndef LOXPP_HPP
#define LOXPP_HPP

#include "Token.hpp"
#include <string>

class Loxpp
{
    // Keep track of errors
    static bool hadError;

  public:
    /* Run the source code
     * Used by runPrompt() and runFile() */
    static void run(const std::string &source);

    /* Run interactive session, like a shell */
    static void runPrompt();
    /* Read source code from a file and run it
     * Returns error code to main in case of error.
     * Main will exit with this error code */
    static int runFile(const std::string &path);

    static void error(const Token &token, const std::string &message);
    static void error(int line, const std::string &message);
    static void report(int line, const std::string &where, const std::string &message);
};

#endif // LOXPP_HPP
