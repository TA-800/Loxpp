# Lox++

This is a personal C++ implementation of the Lox interpreter ( from the book "Crafting Interpreters" [ Java ] ). It has C-like syntax and regular programming-language features like variables with dynamic types, control flow constructs like conditionals, loops and functions.

## Quick demo
Writing a simple recursive fibonacci function in Lox++ REPL mode.

https://github.com/TA-800/Loxpp/assets/31612100/3797d191-81b6-47b7-ab00-7517c0ee8091


## Features

In addition to the core functionalities, Lox++ brings the following capabilities

1. Multiline nested comments
2. String and number concatenation
3. Break statement to exit loops prematurely

## Implementation Details

### Handling the Visitor Pattern

Instead of employing templates (STL) which introduced too much complexity that I was still not familiar with yet, I used void pointers and additional variables to store and keep track of values and their types to perform typecasting as needed to access values. Perhaps not the optimal approach in hindsight.

### Modern C++ utilization

1. RAII (smart pointers)
2. Structured bindings

### Differences

1. Classes are not yet implemented. Support for it is planned but not anytime soon.
2. Only 'false' and 'nil' are considered falsy in Lox, Lox++ also considers the number 0 and empty string as falsy values.

## Usage

### Download

Go to Releases, download the executable and run it if you'd like to give Lox++ a go.

### Build

1. Ensure you have a compiler for C++17.
2. Clone the repository or download the source code.
3. Run make clean then make.
4. Run the executable in build/bin

### Running it

Path to a file as an argument will execute it in Lox++, otherwise no arguments will start REPL mode.


## TODO

1. Implement classes (sometime in the future).
2. Functions that don't create or delete ptrs should accept regular ptrs instead of unique or shared.
