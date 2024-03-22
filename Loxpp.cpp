
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "headers/Scanner.hpp"
#include "headers/Token.hpp"

void run(const std::string &source);
void runPrompt();
void runFile(const std::string &path);
void report(int line, const std::string &where, const std::string &message);

// Global variable to track if an error has occurred
bool hadError = false;

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
        runFile(argv[1]);
    }
    // Interactive session
    else
    {
        runPrompt();
    }
}

void runFile(const std::string &path)
{
    // Reference: https://stackoverflow.com/a/50317432
    // Iterate through the file in binary mode
    std::ifstream file(path, std::ios::binary);

    // Create iterator to read from param 1 (file) to param 2 (end).
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();

    // Create string from bytes vector (specify begin and end of vector) and pass to run
    run(std::string(bytes.begin(), bytes.end()));

    // If error
    if (hadError)
    {
        // 65 -> indicates error
        exit(65);
    }
}

void runPrompt()
{

    std::string line;

    while (true)
    {

        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty())
            break;

        run(line);

        // Don't kill user session if error, just reset the flag
        if (hadError)
            hadError = false;
    }
}

void run(const std::string &source)
{
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // Just print for now
    for (const Token &token : tokens)
    {
        std::cout << token.toString() << "\n";
    }
}

void error(int line, const std::string &message)
{
    report(line, "", message);
}

void report(int line, const std::string &where, const std::string &message)
{
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}
