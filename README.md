# Lox++

This is a C++14 implementation of the Lox interpreter ( from the book "Crafting Interpreters" [ Java ] ).

## Features

1. Multiline nested comments

2. String and number concatenation

## Notes

1. How was the Visitor pattern handled without templates?

The template system introduced too much complexity that I'm still not famililar with. Instead of using T type objects, I opted for void pointers and manually kept track of the value or object type to use typecasting on the pointer when its value was needed.

2. Specifics about the implementation?

Used modern C++ features like RAII (smart pointers) and more.