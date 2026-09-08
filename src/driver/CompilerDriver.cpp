#include "gamescript/driver/CompilerDriver.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/ast/ASTPrinter.hpp"
#include "gamescript/semantic/SemanticAnalyzer.hpp"
#include "gamescript/ir/IRBuilder.hpp"
#include "gamescript/ir/IRPrinter.hpp"
#include "gamescript/optimizer/PassManager.hpp"
#include "gamescript/codegen/LLVMCodegen.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include "gamescript/runtime/VirtualMachine.hpp"
#include "gamescript/runtime/SimulationRenderer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace gamescript::driver {

void CompilerDriver::printStageBanner(const std::string& stageName) const {
    std::cout << "\n========================================\n";
    std::cout << " === " << stageName << " ===\n";
    std::cout << "========================================\n";
}

CompilerResult CompilerDriver::compileFile(const CompilerOptions& options) {
    CompilerResult result;
    if (options.inputFile.empty()) {
        std::cerr << "Error: No input file specified.\n";
        result.exitCode = 1;
        return result;
    }

    std::ifstream file(options.inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input file '" << options.inputFile << "'.\n";
        result.exitCode = 1;
        return result;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return compileSource(ss.str(), options.inputFile, options);
}

CompilerResult CompilerDriver::compileSource(const std::string& source, const std::string& filename, const CompilerOptions& options) {
    CompilerResult result;
    DiagnosticEngine diag(source, filename);

    // 1. Lexical Analysis
    Lexer lexer(source, filename, &diag);
    std::vector<Token> tokens = lexer.tokenize();

    if (options.dumpTokens) {
        std::ostringstream tokStream;
        tokStream << "=== Tokens (" << tokens.size() << ") ===\n";
        for (const auto& tok : tokens) {
            tokStream << "[" << tok.span.toString() << "] "
                      << tokenTypeToString(tok.type)
                      << " '" << tok.lexeme << "'";
            if (tok.type == TokenType::IntegerLiteral) {
                tokStream << " (val=" << tok.intValue << ")";
            }
            tokStream << "\n";
        }
        result.tokensDump = tokStream.str();
        if (options.verbose || options.dumpTokens) {
            printStageBanner("Lexical Analysis Tokens");
            std::cout << result.tokensDump;
        }
    }

    if (diag.hasErrors()) {
        diag.print(std::cerr);
        result.errorLog = diag.formatAll();
        result.exitCode = 1;
        return result;
    }

    // 2. Parsing & AST Construction
    Parser parser(tokens, &diag);
    std::unique_ptr<Program> ast = parser.parseProgram();

    if (diag.hasErrors()) {
        diag.print(std::cerr);
        result.errorLog = diag.formatAll();
        result.exitCode = 1;
        return result;
    }

    if (options.dumpAST && ast) {
        ASTPrinter printer;
        result.astDump = printer.print(*ast);
        if (options.verbose || options.dumpAST) {
            printStageBanner("Abstract Syntax Tree (AST)");
            std::cout << result.astDump;
        }
    }

    // 3. Semantic Analysis
    if (options.checkSemantic && ast) {
        SemanticAnalyzer semantic(&diag);
        bool semanticOk = semantic.analyze(*ast);
        if (!semanticOk || diag.hasErrors()) {
            diag.print(std::cerr);
            result.errorLog = diag.formatAll();
            result.exitCode = 1;
            return result;
        }
        if (options.dumpSemantic) {
            printStageBanner("Semantic Analysis");
            std::cout << "Semantic analysis passed successfully. All symbols and types resolved.\n";
        }
    }

    // 4. Intermediate Representation (IR) Generation
    ir::IRBuilder irBuilder;
    std::unique_ptr<ir::IRModule> irModule;
    if (ast) {
        irModule = irBuilder.build(*ast);
    }

    if (!irModule) {
        std::cerr << "Error: Failed to generate IR Module.\n";
        result.exitCode = 1;
        return result;
    }

    result.irDump = irModule->toString();
    if (options.dumpIR) {
        printStageBanner("GameScript Intermediate Representation (IR)");
        std::cout << result.irDump;
    }

    // 5. Optimization Passes
    if (options.optimize) {
        auto passManager = optimizer::PassManager::createDefaultPipeline();
        passManager->run(*irModule);
        result.optIRDump = irModule->toString();

        if (options.dumpOptIR) {
            printStageBanner("Optimized GameScript IR");
            std::cout << result.optIRDump;
        }
    }

    // 6. LLVM IR Code Generation
    std::string llvmIR = codegen::LLVMCodegen::generateLLVMIR(*irModule);
    result.llvmDump = llvmIR;

    if (options.dumpLLVM) {
        printStageBanner("Generated LLVM IR (Host Runtime ABI Compatible)");
        std::cout << result.llvmDump;
    }

    if (!options.outputFile.empty()) {
        std::ofstream outFile(options.outputFile);
        if (outFile.is_open()) {
            outFile << llvmIR;
            std::cout << "Compiled LLVM IR written to: " << options.outputFile << "\n";
        } else {
            std::cerr << "Error: Could not write output file '" << options.outputFile << "'.\n";
        }
    }

    // 7. Execution Simulation
    if (options.runSimulation) {
        printStageBanner("Interactive Game Simulation Runtime");
        runtime::GameWorld world(12, 8);
        world.setPlayerPosition(2, 5, runtime::Direction::North);
        world.addEnemy(2, 2, 40, "Goblin");
        world.addEnemy(7, 3, 60, "Orc");
        world.addObstacle(4, 5);

        std::cout << "Initial World State:\n";
        runtime::SimulationRenderer::render(world, std::cout);

        runtime::VirtualMachine vm(world);
        bool execSuccess = vm.execute(*irModule);

        std::cout << "\nExecution Trace Log:\n";
        for (const auto& logEntry : world.getActionLog()) {
            std::cout << "  > " << logEntry << "\n";
        }

        std::cout << "\nFinal World State:\n";
        runtime::SimulationRenderer::render(world, std::cout);

        if (!execSuccess) {
            std::cerr << "Warning: VirtualMachine encountered runtime execution error.\n";
            result.exitCode = 2;
        }
    }

    result.success = true;
    result.exitCode = 0;
    return result;
}

} // namespace gamescript::driver
