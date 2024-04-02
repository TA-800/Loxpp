# Lox++

This is a personal C++ implementation of the Lox interpreter ( from the book "Crafting Interpreters" [ Java ] ).

## Features

In addition to the core functionalities, Lox++ brings the following capabilities

1. Multiline nested comments

2. String and number concatenation

3. Break statement to exit loops prematurely

## Implementation Details

### Handling the Visitor Pattern

Instead of employing templates (STL) which introduced too much complexity that I was still not familiar with yet, I utilized void pointers and an additional variables to keep track of value types to perform typecasting as needed to access values.

### Modern C++ utilization

1. RAII (smart pointers)

2. Structured bindings

### Differences

Only 'false' and 'nil' are considered falsy in Lox, Lox++ also considers the number 0 and empty string as falsy values.

## Usage

### Download

Go to Releases and download the executable if you'd like to give it a go.

### Build

1. Ensure you have a compiler for C++17.

2. Clone the repository or download the source code.

3. Run make clean then make.

4. Run the executable in build/bin
