cmake_minimum_required(VERSION 3.20)
project(GameScript VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Warnings and Flags
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core Compiler Library Sources
set(GAMESCRIPT_CORE_SOURCES
    src/common/DiagnosticEngine.cpp
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
    src/ast/ASTPrinter.cpp
    src/parser/Parser.cpp
    src/semantic/SymbolTable.cpp
    src/semantic/SemanticAnalyzer.cpp
    src/ir/Instruction.cpp
    src/ir/BasicBlock.cpp
    src/ir/IRBuilder.cpp
    src/ir/IRPrinter.cpp
    src/optimizer/PassManager.cpp
    src/optimizer/ConstantFoldingPass.cpp
    src/optimizer/ConstantPropagationPass.cpp
    src/optimizer/DeadCodeEliminationPass.cpp
    src/optimizer/RedundantInstEliminationPass.cpp
    src/codegen/LLVMCodegen.cpp
    src/codegen/RuntimeABI.cpp
    src/runtime/VirtualMachine.cpp
    src/runtime/GameWorld.cpp
    src/runtime/SimulationRenderer.cpp
    src/driver/CompilerDriver.cpp
)

# Optional LLVM Configuration
find_package(LLVM CONFIG QUIET)
if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})
    set(GAMESCRIPT_HAS_LLVM ON)
else()
    message(STATUS "Using integrated LLVM IR Code Generator and GameScript Bytecode VM backend.")
    set(GAMESCRIPT_HAS_LLVM OFF)
endif()

# Core static library
add_library(gamescript_core STATIC
    ${GAMESCRIPT_CORE_SOURCES}
)

target_include_directories(gamescript_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Main CLI Executable
add_executable(gamescript
    src/main.cpp
)
target_link_libraries(gamescript PRIVATE gamescript_core)

# Test Executable
add_executable(gamescript_tests
    tests/TestMain.cpp
    tests/unit/LexerTests.cpp
    tests/unit/ParserTests.cpp
    tests/unit/ASTTests.cpp
    tests/unit/SemanticTests.cpp
    tests/unit/IROptimizerTests.cpp
    tests/unit/RuntimeTests.cpp
    tests/unit/EndToEndTests.cpp
)

target_include_directories(gamescript_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_link_libraries(gamescript_tests PRIVATE gamescript_core)

enable_testing()
add_test(NAME AllTests COMMAND gamescript_tests)
