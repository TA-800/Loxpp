#include "headers/Loxpp.hpp"
#include "headers/AstPrinter.hpp"
#include "headers/Parser.hpp"
#include "headers/Scanner.hpp"
#include "headers/Token.hpp"
#include <fstream>
#include <iostream>
#include <vector>

// Everything is static in this class
bool Loxpp::hadError = false;
bool Loxpp::hadRuntimeError = false;
AstInterpreter Loxpp::interpreter;

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
        return 65;
    if (hadRuntimeError)
        return 70;

    // 0 -> indicates success
    return 0;
}

// Interactive session
void Loxpp::runPrompt()
{
    std::string line;
    /* std::vector<std::string> lines{ "!(false)",        "\"ab\" + \"bc\"",     "1 + 2 * 3",           "\"Score: \" +
     * 4", "\"Score: \" * 4", "\"Score: \" + 4 * 5", "4 * 5 + \"Score: \"", "10 + 20 + (10 / 2) + (5 * 2) + 100 * 5"};
     */

    /* int counter = 0; */
    while (true)
    {

        std::cout << "> ";
        std::getline(std::cin, line);
        /* line = lines[counter]; // Debugging purposes */
        if (line.empty())
            break;

        run(line);

        // Don't kill user session if error, just reset the flag
        if (hadError)
            hadError = false;

        /* counter++; */
        /* if (counter == lines.size()) // Debugging purposes */
        /*     break; */
    }
}

// Run the source code
void Loxpp::run(const std::string &source)
{
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // Parse tokens into expressions
    Parser parser(tokens);
    std::unique_ptr<Expr> expression = parser.parse();

    // If there was an error, don't run the interpreter
    if (hadError)
        return;

    // Use AST Interpreter to interpret the tokens
    AstPrinter printer;
    printer.setPrintResult(expression);
    std::cout << printer.get() << "\n";
    interpreter.evaluate(expression);

    // Free memory (to avoid memory leaks because we are using new with void *)
    scanner.freeTokens();
}

// Error handling
void Loxpp::runtimeError(const RuntimeError &error)
{
    std::cerr << "[line " << error.token.getLine() << "] " << error.token.getLexeme() << " : " << error.what() << "\n";
    hadRuntimeError = true;
}
void Loxpp::error(const Token &token, const std::string &message)
{
    if (token.getType() == TokenInfo::Type::END_OF_FILE)
    {
        report(token.getLine(), " at end", message);
    }
    else
    {
        report(token.getLine(), " at '" + token.getLexeme() + "'", message);
    }
}

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
