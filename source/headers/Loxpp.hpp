#ifndef LOXPP_HPP
#define LOXPP_HPP

#include <string>

class Loxpp
{
    // Keep track of errors
    static bool hadError;

  public:
    void runPrompt();
    int run(const std::string &source);
    int runFile(const std::string &path);
    static void error(int line, const std::string &message);
    static void report(int line, const std::string &where, const std::string &message);
};

#endif // LOXPP_HPP
