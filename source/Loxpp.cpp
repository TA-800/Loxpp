#include "headers/Loxpp.hpp"
#include "headers/Scanner.hpp"
#include "headers/Token.hpp"
#include <fstream>
#include <iostream>
#include <vector>

// Everything is static in this class
bool Loxpp::hadError = false;

int Loxpp::runFile(const std::string &path)
{
    // Reference: https://stackoverflow.com/a/50317432
    // Iterate through the file in binary mode
    std::ifstream file(path, std::ios::binary);

    // Create iterator to read from param 1 (file) to param 2 (end).
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();

    // Get source code from input read
    // Create string from bytes
    // Run the source code
    run(std::string(bytes.begin(), bytes.end()));

    // If error (can be set by run() if error occurs)
    if (hadError)
    {
        // 65 -> indicates error
        return 65;
    }

    // 0 -> indicates success
    return 0;
}

// Interactive session
void Loxpp::runPrompt()
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

// Run the source code
void Loxpp::run(const std::string &source)
{
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // Just print for now
    for (const Token &token : tokens)
    {
        std::cout << token.toString() << "\n";
    }

    // Free memory (to avoid memory leaks because we are using new with void *)
    scanner.freeTokens();
}

// Error handling
void Loxpp::error(int line, const std::string &message)
{
    // For now, where is empty
    report(line, "", message);
}

// Will set hadError to true
void Loxpp::report(int line, const std::string &where, const std::string &message)
{
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}