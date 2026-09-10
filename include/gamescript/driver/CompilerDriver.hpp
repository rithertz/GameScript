#pragma once

//==============================================================================
// CompilerDriver.hpp
//
// Defines the compiler driver and supporting structures used to configure
// compilation and store the results produced by each compiler stage.
//==============================================================================

#include "gamescript/common/DiagnosticEngine.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ir/IRModule.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include <string>
#include <memory>
#include <vector>

namespace gamescript::driver {

// Stores configuration options controlling the compiler pipeline.
struct CompilerOptions {
    std::string inputFile;
    std::string outputFile;
    bool dumpTokens = false;
    bool dumpAST = false;
    bool checkSemantic = true;
    bool dumpSemantic = false;
    bool dumpIR = false;
    bool optimize = true;
    bool dumpOptIR = false;
    bool dumpLLVM = false;
    bool runSimulation = false;
    bool verbose = false;
};

// Stores the result and generated output from a compilation.
struct CompilerResult {
    bool success = false;
    int exitCode = 0;
    std::string tokensDump;
    std::string astDump;
    std::string irDump;
    std::string optIRDump;
    std::string llvmDump;
    std::string simulationOutput;
    std::string errorLog;
};

// Coordinates the GameScript compilation pipeline.
class CompilerDriver {
public:
    CompilerDriver() = default;

    // Compiles a GameScript source file using the specified options.
    CompilerResult compileFile(const CompilerOptions& options);

    // Compiles source text using the specified filename and compiler options.
    CompilerResult compileSource(const std::string& source, const std::string& filename, const CompilerOptions& options);

private:
    // Prints a banner identifying the current compiler stage.
    void printStageBanner(const std::string& stageName) const;
};

} // namespace gamescript::driver