# GameScript Testing

## Overview

GameScript uses multiple levels of testing to validate the compiler and runtime.

The testing strategy is divided into:

1. Unit tests
2. Integration tests
3. End-to-end compiler tests
4. Example GameScript programs
5. Negative/error cases

## Unit Tests

Unit tests validate individual compiler components.

| Component | Test File | Purpose |
|---|---|---|
| Lexer | `tests/unit/LexerTests.cpp` | Tokenization, indentation, literals, operators, diagnostics |
| Parser | `tests/unit/ParserTests.cpp` | AST construction and syntax handling |
| AST | `tests/unit/ASTTests.cpp` | AST representation and printing |
| Semantic Analysis | `tests/unit/SemanticTests.cpp` | Symbols, scopes, and type checking |
| IR / Optimizer | `tests/unit/IROptimizerTests.cpp` | IR generation and optimization passes |
| Runtime | `tests/unit/RuntimeTests.cpp` | GameWorld and VM behaviour |
| End-to-End | `tests/unit/EndToEndTests.cpp` | Compiler pipeline integration |

## Integration Testing

Integration tests validate complete language features across multiple compiler stages.

Planned coverage includes:

- Control flow
- Functions and function calls
- Function arguments
- Sensors
- Runtime decisions
- Optimization
- Error propagation

## Example Programs

The `examples/` directory contains human-readable GameScript programs that demonstrate language features and compiler behaviour.

Examples are intended to act as:

- Language demonstrations
- Manual integration scenarios
- Regression programs
- Portfolio examples

## Negative Testing

Negative tests verify that invalid GameScript programs are rejected with useful diagnostics.

Planned coverage includes:

- Lexical errors
- Syntax errors
- Undeclared identifiers
- Duplicate declarations
- Invalid function calls
- Type errors
- Invalid indentation

## Testing Goals

The long-term goal is to ensure that important GameScript features are validated across the complete pipeline:

GameScript source

→ Lexer

→ Parser

→ AST

→ Semantic Analysis

→ IR

→ Optimization

→ VM / Runtime

A feature should not be considered complete until its behaviour is covered at the appropriate testing levels.