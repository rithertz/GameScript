#pragma once

#include "gamescript/common/DiagnosticEngine.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ir/IRModule.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include <string>
#include <memory>
#include <vector>

namespace gamescript::driver {

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

class CompilerDriver {
public:
    CompilerDriver() = default;

    CompilerResult compileFile(const CompilerOptions& options);
    CompilerResult compileSource(const std::string& source, const std::string& filename, const CompilerOptions& options);

private:
    void printStageBanner(const std::string& stageName) const;
};

} // namespace gamescript::driver
