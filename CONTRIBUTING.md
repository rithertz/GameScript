# Contributing to GameScript

Thank you for your interest in contributing to GameScript!

## Code Architecture & Standards
1. **Language Standard**: Modern C++20 (`-std=c++20`).
2. **Compiler Modularity**: Maintain strict phase isolation (Lexer -> Parser -> AST -> Semantic -> IR -> Optimizer -> Codegen -> Runtime). Do not leak runtime concepts into the parser, or parser tokens into the IR.
3. **Memory Safety**: Use RAII, `std::unique_ptr`, `std::shared_ptr`, and `std::string_view` where appropriate. No raw owning pointers.
4. **Diagnostic Integrity**: All syntax and semantic errors must be channeled through `DiagnosticEngine` with accurate source locations and line snippets.

## Building and Testing
```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
# or run the test runner directly:
./gamescript_tests
```

## Adding New Language Features
1. Update lexical tokens in `include/gamescript/lexer/Token.hpp` and `src/lexer/Lexer.cpp`.
2. Add grammar rules in `docs/GRAMMAR.md` and implement parser methods in `src/parser/Parser.cpp`.
3. Add AST nodes in `include/gamescript/ast/`.
4. Implement semantic checking in `src/semantic/SemanticAnalyzer.cpp`.
5. Lower to IR in `src/ir/IRBuilder.cpp`.
6. Add LLVM emission in `src/codegen/LLVMCodegen.cpp` and VM execution in `src/runtime/VirtualMachine.cpp`.
7. Add positive and negative unit tests in `tests/unit/`.
