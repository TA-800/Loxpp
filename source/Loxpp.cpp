#include "headers/Loxpp.hpp"
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
    /* std::vector<std::string> lines = { */
    /*     "var a = 5;", "{", "  var a = 6;", "  print a;", "}", "print a;", */
    /* }; */

    int counter = 0;
    while (true)
    {

        std::cout << "> ";
        std::getline(std::cin, line);
        /* line = lines[counter++]; */
        /* std::cout << line << "\n"; */
        if (line.empty())
            break;

        run(line);

        // Don't kill user session if error, just reset the flag
        if (hadError)
            hadError = false;

        /* if (counter == lines.size()) */
        /*     break; */
    }
}

// Run the source code
void Loxpp::run(const std::string &source)
{
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // Parse tokens into AST statements and expressions
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    // If there was an error, don't run the interpreter and ensure memory created by scanner for tokens (literals) is
    // freed
    if (hadError)
    {
        scanner.freeTokens();
        return;
    }

    // Use AST Interpreter to interpret the tokens
    /* AstPrinter printer; printer.setPrintResult(statements); std::cout << printer.get() << "\n"; */

    interpreter.setInterpretResult(statements);

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
