#include "gamescript/driver/CompilerDriver.hpp"
#include <iostream>
#include <string>
#include <vector>

static void printUsage(const char* progName) {
    std::cout << "GameScript Natural Language Compiler (v1.0.0)\n";
    std::cout << "Usage: " << progName << " <source.gs> [options]\n\n";
    std::cout << "Pipeline Inspection Options:\n";
    std::cout << "  --tokens, -t      Dump lexical tokens\n";
    std::cout << "  --ast, -a         Dump Abstract Syntax Tree (AST)\n";
    std::cout << "  --semantic, -s    Run semantic analysis and print symbol validation\n";
    std::cout << "  --ir, -i          Dump raw GameScript 3-address IR\n";
    std::cout << "  --opt-ir, -o      Dump optimized GameScript IR\n";
    std::cout << "  --llvm, -l        Generate and dump LLVM IR\n";
    std::cout << "  --run, -r         Execute the program inside the 2D Game Runtime\n";
    std::cout << "  --all             Run all compiler phases and show intermediate outputs\n";
    std::cout << "  --output, -c <f>  Write generated LLVM IR to specified file\n";
    std::cout << "  --no-opt          Disable IR optimization passes\n";
    std::cout << "  --help, -h        Show this help message\n";
    std::cout << "  --version, -v     Show compiler version\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    gamescript::driver::CompilerOptions options;
    bool hasExplicitPhaseFlag = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            std::cout << "GameScript Compiler v1.0.0 (C++20, LLVM IR Code Generation Backend)\n";
            return 0;
        } else if (arg == "-t" || arg == "--tokens") {
            options.dumpTokens = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-a" || arg == "--ast") {
            options.dumpAST = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-s" || arg == "--semantic") {
            options.dumpSemantic = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-i" || arg == "--ir") {
            options.dumpIR = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-o" || arg == "--opt-ir") {
            options.dumpOptIR = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-l" || arg == "--llvm") {
            options.dumpLLVM = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-r" || arg == "--run") {
            options.runSimulation = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "--all") {
            options.dumpTokens = true;
            options.dumpAST = true;
            options.dumpSemantic = true;
            options.dumpIR = true;
            options.dumpOptIR = true;
            options.dumpLLVM = true;
            options.runSimulation = true;
            hasExplicitPhaseFlag = true;
        } else if (arg == "-c" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "Error: Output file path required after " << arg << "\n";
                return 1;
            }
        } else if (arg == "--no-opt") {
            options.optimize = false;
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        } else {
            options.inputFile = arg;
        }
    }

    if (options.inputFile.empty()) {
        std::cerr << "Error: No input .gs file provided.\n";
        printUsage(argv[0]);
        return 1;
    }

    // Default behavior if no specific stage flag is passed: run the simulation
    if (!hasExplicitPhaseFlag) {
        options.runSimulation = true;
    }

    gamescript::driver::CompilerDriver driver;
    auto result = driver.compileFile(options);

    return result.exitCode;
}
