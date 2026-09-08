# GameScript

**GameScript** is a small domain-specific programming language (DSL) for describing game behavior using simple, human-readable commands. The project implements a compiler pipeline in **C++20**, including lexical analysis, parsing, AST construction, semantic analysis, intermediate representation (IR) generation, optimization, LLVM IR generation, and a custom 2D game simulation runtime.

---

## Table of Contents

- [Overview](#overview)
- [Main Features](#main-features)
- [Compiler Architecture](#compiler-architecture)
  - [1. Lexical Analysis](#1-lexical-analysis)
  - [2. Parsing](#2-parsing)
  - [3. Abstract Syntax Tree](#3-abstract-syntax-tree)
  - [4. Semantic Analysis](#4-semantic-analysis)
  - [5. Intermediate Representation](#5-intermediate-representation)
  - [6. Optimization](#6-optimization)
  - [LLVM IR Generation](#llvm-ir-generation)
  - [Game Runtime and Virtual Machine](#game-runtime-and-virtual-machine)
- [Command-Line Interface](#command-line-interface)
- [Example Programs](#example-programs)
- [Example Compilation Flow](#example-compilation-flow)
- [Building on Windows 11](#building-on-windows-11)
  - [Requirements](#requirements)
  - [Install the Toolchain](#install-the-toolchain)
- [Build Instructions](#build-instructions)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Technical Highlights](#technical-highlights)
- [Current Limitations](#current-limitations)
- [Future Improvements](#future-improvements)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)
- [Project Status](#project-status)

---


## Overview

A GameScript program can describe actions such as movement, turning, combat, conditions, loops, and subroutines.

For example:

```gamescript
set patrol_steps = 4
set turn_angle = 90

player move forward patrol_steps
player turn right turn_angle

player move forward patrol_steps
player turn right turn_angle

player defend
````

Instead of directly executing these commands, the compiler processes them through multiple stages before the program is executed by the GameScript runtime.

The main idea of the project is:

```text
GameScript Source
       |
       v
    Lexer
       |
       v
    Parser
       |
       v
      AST
       |
       v
Semantic Analysis
       |
       v
 GameScript IR
       |
       v
  Optimization
       |
       +-------------------+
       |                   |
       v                   v
   LLVM IR             Virtual Machine
                           |
                           v
                      Game Simulation
```

---

## Main Features

### Language Features

* Variable declarations using `set`
* Integer and boolean expressions
* Arithmetic operators
* Relational and logical operators
* Player movement
* Player rotation
* Game actions such as:

  * `attack`
  * `defend`
  * `jump`
  * `interact`
  * `retreat`
* Conditional statements using `if` and `else`
* `repeat` loops
* `while` loops
* Functions and subroutine calls
* Sensor-based conditions such as:

  * `enemy nearby`
  * `health low`
  * `obstacle ahead`
  * `distance to enemy`
  * `health`

### Compiler Features

* Indentation-aware lexical analysis
* Source location tracking
* Recursive-descent parser
* Abstract Syntax Tree (AST)
* Scoped symbol table
* Semantic analysis
* Three-address-style intermediate representation
* Basic blocks and control-flow representation
* Multiple optimization passes
* LLVM IR generation
* Custom virtual machine for runtime execution
* Deterministic 2D grid simulation
* Unit and end-to-end tests

---

# Compiler Architecture

GameScript is divided into several compiler stages. Each stage has a separate responsibility.

## 1. Lexical Analysis

The lexer converts the source program into a stream of tokens.

For example:

```gamescript
player move forward patrol_steps
```

is converted into tokens similar to:

```text
Player
Move
Forward
Identifier("patrol_steps")
Newline
```

The lexer also handles indentation and emits `INDENT` and `DEDENT` tokens for block-based constructs.

Source locations are attached to tokens so that syntax and semantic errors can be reported with their source position.

Implementation:

```text
include/gamescript/lexer/
src/lexer/
```

---

## 2. Parsing

The parser consumes the token stream and checks whether the program follows the GameScript grammar.

GameScript uses a **recursive-descent parser**.

For example:

```gamescript
player move forward patrol_steps
```

is represented as a structured statement containing:

```text
Move
├── Direction: FORWARD
└── Distance: patrol_steps
```

The parser builds the Abstract Syntax Tree used by later compiler stages.

Implementation:

```text
include/gamescript/parser/
src/parser/
```

---

## 3. Abstract Syntax Tree

The AST represents the structure of the GameScript program without unnecessary source-level details.

For example:

```text
Program
├── VarDecl: patrol_steps
│   └── Value: 4
├── VarDecl: turn_angle
│   └── Value: 90
├── Move: FORWARD
│   └── Distance: patrol_steps
├── Turn: RIGHT
│   └── Degrees: turn_angle
└── Action: DEFEND
```

AST nodes are defined for expressions and statements, and an AST visitor is used to traverse them.

Implementation:

```text
include/gamescript/ast/
src/ast/
```

---

## 4. Semantic Analysis

Semantic analysis checks whether a syntactically valid program also makes sense.

The semantic analyzer uses a scoped symbol table to perform checks such as:

* Detecting undeclared identifiers
* Preventing duplicate declarations within the same scope
* Checking function call argument counts
* Validating conditions and expressions

For example:

```gamescript
set speed = 4
player move forward speed
```

is valid because `speed` has been declared before it is used.

Implementation:

```text
include/gamescript/semantic/
src/semantic/
```

---

## 5. Intermediate Representation

The compiler lowers the AST into a custom **GameScript Intermediate Representation (IR)**.

The IR uses instructions such as:

```text
ALLOCA
STORE
LOAD
GAME_MOVE
GAME_TURN
GAME_ACTION
GAME_SENSOR
BRANCH
BRANCH_COND
RET
```

For example:

```gamescript
player move forward patrol_steps
```

can become:

```text
LOAD         %0, patrol_steps
GAME_MOVE    0, %0
```

The IR separates the language front end from later optimization and execution stages.

Implementation:

```text
include/gamescript/ir/
src/ir/
```

---

## 6. Optimization

GameScript includes a modular optimization pipeline implemented through a `PassManager`.

Current optimization passes include:

### Constant Folding

Evaluates constant expressions at compile time.

Example:

```text
2 + 3
```

can become:

```text
5
```

### Constant Propagation

Replaces known variable values with constants where possible.

Example:

```text
set speed = 10
player move forward speed
```

can be simplified to:

```text
GAME_MOVE 0, 10
```

### Dead Code Elimination

Removes instructions whose results are not needed.

### Redundant Instruction Elimination

Removes operations that have no effect on the game state, such as:

```text
GAME_MOVE 0, 0
GAME_TURN 1, 0
```

Implementation:

```text
include/gamescript/optimizer/
src/optimizer/
```

---

# LLVM IR Generation

GameScript also includes an LLVM IR backend.

The backend converts GameScript IR into typed LLVM IR and exposes game operations through a small runtime ABI.

For example, a GameScript movement instruction can be represented as:

```llvm
call void @gs_runtime_move(i32 0, i64 4)
```

The current implementation can generate LLVM IR using:

```powershell
.\build\gamescript.exe .\examples\02_combat_tactics.gs --llvm
```

The generated LLVM module contains declarations for runtime operations such as movement, turning, actions, and sensors.

Implementation:

```text
include/gamescript/codegen/
src/codegen/
```

---

# Game Runtime and Virtual Machine

GameScript includes a custom runtime that executes compiled GameScript instructions in a small 2D grid-based world.

The runtime keeps track of:

* Player position
* Player direction
* Player health
* Defensive state
* Enemies
* Obstacles
* Game actions
* Execution history

The world is represented using a text-based grid.

Example:

```text
+-------------------------+
| . . . . . . . . . . . . |
| . . . . . . . . . . . . |
| . . E . . . . . . . . . |
| . . . . . . . E . . . . |
| . . . . . . . . . . . . |
| . . P^. # . . . . . . . |
| . . . . . . . . . . X . |
| . . . . . . . . . . . . |
+-------------------------+
```

Legend:

```text
P^ / P> / Pv / P<  Player and facing direction
E                   Enemy
#                   Obstacle
X                   Goal / target
.                   Empty cell
```

The virtual machine interprets GameScript instructions and updates the game world accordingly.

Implementation:

```text
include/gamescript/runtime/
src/runtime/
```

---

# Command-Line Interface

The main executable is:

```text
gamescript.exe
```

General usage:

```text
gamescript.exe <source.gs> [options]
```

Display help:

```powershell
.\build\gamescript.exe --help
```

### Available Options

```text
--tokens, -t      Dump lexical tokens
--ast, -a         Dump Abstract Syntax Tree
--semantic, -s    Run semantic analysis
--ir, -i          Dump raw GameScript IR
--opt-ir, -o      Dump optimized GameScript IR
--llvm, -l        Generate and dump LLVM IR
--run, -r         Execute the program in the game runtime
--all             Run all compiler phases
--output, -c <f>  Write generated LLVM IR to a file
--no-opt          Disable IR optimization
--help, -h        Show help
```

---

# Example Programs

The `examples/` directory contains sample GameScript programs:

```text
examples/
├── 01_movement_patrol.gs
├── 02_combat_tactics.gs
├── 03_loops_and_counters.gs
├── 04_functions_and_subroutines.gs
└── 05_optimizer_demo.gs
```

### Movement Example

Run the movement example in the game runtime:

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --run
```

### View Tokens

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --tokens
```

### View AST

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --ast
```

### Run Semantic Analysis

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --semantic
```

### View Raw IR

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --ir
```

### View Optimized IR

```powershell
.\build\gamescript.exe .\examples\05_optimizer_demo.gs --opt-ir
```

### Generate LLVM IR

```powershell
.\build\gamescript.exe .\examples\02_combat_tactics.gs --llvm
```

### Run All Compiler Stages

```powershell
.\build\gamescript.exe .\examples\01_movement_patrol.gs --all
```

---

# Example Compilation Flow

Consider:

```gamescript
set patrol_steps = 4
set turn_angle = 90

player move forward patrol_steps
player turn right turn_angle
player defend
```

The lexer first identifies keywords, identifiers, literals, and operators.

The parser converts the token stream into an AST similar to:

```text
Program
├── VarDecl: patrol_steps
│   └── Value: 4
├── VarDecl: turn_angle
│   └── Value: 90
├── Move: FORWARD
│   └── Distance: patrol_steps
├── Turn: RIGHT
│   └── Degrees: turn_angle
└── Action: DEFEND
```

The IR builder then produces instructions such as:

```text
ALLOCA       patrol_steps
STORE        patrol_steps, 4
LOAD         %0, patrol_steps
GAME_MOVE    0, %0
LOAD         %1, turn_angle
GAME_TURN    1, %1
GAME_ACTION  1
RET
```

Optimization can replace known variable values:

```text
GAME_MOVE    0, 4
GAME_TURN    1, 90
GAME_ACTION  1
```

The optimized instructions can then be executed by the GameScript virtual machine.

---

# Building on Windows 11

## Requirements

For the current Windows development setup:

* Windows 11
* Git
* MSYS2
* UCRT64 GCC/G++
* MinGW Make
* CMake 3.20 or newer

The project uses **C++20**.

LLVM does not need to be installed separately for the current project build configuration.

## Install the Toolchain

Using an MSYS2 UCRT64 terminal:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-make
```

Make sure the following tools are available from PowerShell:

```powershell
g++ --version
mingw32-make --version
cmake --version
git --version
```

---

# Build Instructions

From the project root:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

Build the project:

```powershell
cmake --build build
```

After a successful build, the main executables are generated inside `build/`:

```text
build/
├── gamescript.exe
└── gamescript_tests.exe
```

The `build/` directory contains generated files and should not be committed to Git.

---

# Testing

The project contains tests for the main compiler components.

Test categories include:

* Lexer tests
* Parser tests
* AST tests
* Semantic analysis tests
* IR and optimizer tests
* Runtime tests
* End-to-end tests

Run the test executable directly:

```powershell
.\build\gamescript_tests.exe
```

Or use CTest:

```powershell
ctest --test-dir build --output-on-failure
```

A successful test run currently reports:

```text
100% tests passed
```

---

# Project Structure

```text
GameScript/
│
├── docs/
│   ├── COMPILER_PIPELINE.md
│   ├── GRAMMAR.md
│   ├── LANGUAGE_SPEC.md
│   ├── LLVM_INTEGRATION.md
│   ├── OPTIMIZATIONS.md
│   └── RUNTIME_SIMULATION.md
│
├── examples/
│   ├── 01_movement_patrol.gs
│   ├── 02_combat_tactics.gs
│   ├── 03_loops_and_counters.gs
│   ├── 04_functions_and_subroutines.gs
│   └── 05_optimizer_demo.gs
│
├── include/
│   └── gamescript/
│       ├── ast/
│       ├── codegen/
│       ├── common/
│       ├── driver/
│       ├── ir/
│       ├── lexer/
│       ├── optimizer/
│       ├── parser/
│       ├── runtime/
│       └── semantic/
│
├── src/
│   ├── ast/
│   ├── codegen/
│   ├── common/
│   ├── driver/
│   ├── ir/
│   ├── lexer/
│   ├── optimizer/
│   ├── parser/
│   ├── runtime/
│   ├── semantic/
│   └── main.cpp
│
├── tests/
│   ├── unit/
│   ├── TestHarness.hpp
│   └── TestMain.cpp
│
├── CMakeLists.txt
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

---

# Technical Highlights

This project demonstrates several important compiler and software engineering concepts:

### Compiler Design

* Lexical analysis
* Recursive-descent parsing
* Abstract Syntax Trees
* Semantic analysis
* Symbol tables
* Intermediate representation
* Basic blocks
* Control-flow representation
* Compiler optimizations
* Code generation
* Runtime execution

### Software Engineering

* Modular C++20 codebase
* Separation between compiler stages
* CMake-based build system
* Automated testing
* Example programs for feature testing
* Documentation for the language and compiler pipeline

### Optimization

Implemented optimization passes include:

* Constant folding
* Constant propagation
* Dead code elimination
* Redundant instruction elimination

### Multiple Execution Paths

The compiler supports both:

```text
GameScript IR
      |
      v
Virtual Machine
      |
      v
2D Game Simulation
```

and:

```text
GameScript IR
      |
      v
LLVM Code Generation
      |
      v
LLVM IR
```

This separation makes it possible to experiment with different execution backends without changing the language front end.

---

# Current Limitations

GameScript is an educational compiler project and is intentionally smaller than a production programming language.

Current limitations include:

* The language has a limited set of statements and expressions.
* The game runtime is a simplified grid-based simulation.
* The runtime is designed primarily for demonstrating compiled game instructions.
* LLVM IR generation is implemented, but the project does not currently provide a complete one-command pipeline from `.gs` source to a standalone native game executable.
* The simulation currently uses a text-based renderer.

---

# Future Improvements

Possible future work includes:

* Adding more language constructs
* Improving syntax and semantic error reporting
* Adding more compiler optimization passes
* Improving the LLVM backend
* Expanding the runtime and game world
* Adding more entities and game interactions
* Improving the simulation visualization
* Expanding automated test coverage
* Adding a cleaner command-line workflow
* Adding continuous integration for automated builds and tests

---

# Documentation

More detailed technical information is available in the `docs/` directory:

| File                    | Description                               |
| ----------------------- | ----------------------------------------- |
| `COMPILER_PIPELINE.md`  | Compiler architecture and phase breakdown |
| `GRAMMAR.md`            | Formal GameScript grammar                 |
| `LANGUAGE_SPEC.md`      | Language syntax and supported constructs  |
| `LLVM_INTEGRATION.md`   | LLVM IR generation and runtime ABI        |
| `OPTIMIZATIONS.md`      | Optimization passes                       |
| `RUNTIME_SIMULATION.md` | 2D game runtime behavior                  |

---

# Contributing

This project is being developed as a university Compiler Design project.

Please see `CONTRIBUTING.md` for guidelines regarding:

* Compiler phase separation
* C++ coding practices
* Memory management
* Diagnostics
* Testing
* Adding new language features

When adding a language feature, the typical process is:

```text
Grammar
   ↓
Lexer
   ↓
Parser / AST
   ↓
Semantic Analysis
   ↓
IR
   ↓
Optimizer
   ↓
Runtime / LLVM
   ↓
Tests
```

---

# License

This project is distributed under the license specified in `LICENSE`.

---

# Project Status

**Status: Working and under active development**

The current version successfully demonstrates:

```text
GameScript Source
        ↓
Lexical Analysis
        ↓
Parsing / AST
        ↓
Semantic Analysis
        ↓
GameScript IR
        ↓
Optimization
        ↓
LLVM IR Generation
        ↓
Virtual Machine Execution
        ↓
2D Game Simulation
```

The project provides a working base for further improvements to the language, compiler optimizations, LLVM backend, testing, and runtime.
