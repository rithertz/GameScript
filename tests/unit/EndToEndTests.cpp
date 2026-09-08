#include "TestHarness.hpp"
#include "gamescript/driver/CompilerDriver.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/semantic/SemanticAnalyzer.hpp"
#include "gamescript/ir/IRBuilder.hpp"
#include "gamescript/optimizer/PassManager.hpp"
#include "gamescript/codegen/LLVMCodegen.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include "gamescript/runtime/VirtualMachine.hpp"

using namespace gamescript;

GS_TEST(EndToEndTests, FullPipelineMovementAndCombat) {
    std::string source = 
        "# Combat Patrol Script\n"
        "set speed = 2\n"
        "player move forward speed\n"
        "player turn right 90\n"
        "if enemy nearby:\n"
        "    player attack\n"
        "    player move backward 1\n"
        "else:\n"
        "    player move forward 3\n";

    driver::CompilerDriver driver;
    driver::CompilerOptions opts;
    opts.dumpTokens = true;
    opts.dumpAST = true;
    opts.checkSemantic = true;
    opts.dumpIR = true;
    opts.optimize = true;
    opts.dumpOptIR = true;
    opts.dumpLLVM = true;
    opts.runSimulation = true;

    auto result = driver.compileSource(source, "e2e_combat.gs", opts);
    GS_ASSERT_EQ(result.exitCode, 0);
    GS_ASSERT(result.success);
    GS_ASSERT(!result.tokensDump.empty());
    GS_ASSERT(!result.astDump.empty());
    GS_ASSERT(!result.irDump.empty());
    GS_ASSERT(!result.optIRDump.empty());
    GS_ASSERT(!result.llvmDump.empty());

    // Verify LLVM IR contains expected declarations and instructions
    GS_ASSERT(result.llvmDump.find("@gs_runtime_move") != std::string::npos);
    GS_ASSERT(result.llvmDump.find("@gs_runtime_turn") != std::string::npos);
    GS_ASSERT(result.llvmDump.find("@gs_runtime_action") != std::string::npos);
    GS_ASSERT(result.llvmDump.find("@gs_runtime_sensor") != std::string::npos);
}

GS_TEST(EndToEndTests, NegativeSyntaxErrorRecovery) {
    std::string badSource = "player move forward @\n";
    driver::CompilerDriver driver;
    driver::CompilerOptions opts;
    auto result = driver.compileSource(badSource, "syntax_err.gs", opts);

    GS_ASSERT_NE(result.exitCode, 0);
    GS_ASSERT(!result.success);
    GS_ASSERT(!result.errorLog.empty());
}

GS_TEST(EndToEndTests, NegativeUndeclaredVariable) {
    std::string badSource = "player move forward missing_variable\n";
    driver::CompilerDriver driver;
    driver::CompilerOptions opts;
    auto result = driver.compileSource(badSource, "undeclared.gs", opts);

    GS_ASSERT_NE(result.exitCode, 0);
    GS_ASSERT(!result.success);
    GS_ASSERT(result.errorLog.find("undeclared") != std::string::npos ||
              result.errorLog.find("missing_variable") != std::string::npos);
}

GS_TEST(EndToEndTests, OptimizationPipelineE2E) {
    std::string optSource = 
        "set a = 10 + 5\n"
        "set b = a * 2\n"
        "player move forward b\n"
        "player move forward 0\n"
        "player turn left 0\n";

    driver::CompilerDriver driver;
    driver::CompilerOptions opts;
    opts.dumpIR = true;
    opts.optimize = true;
    opts.dumpOptIR = true;

    auto result = driver.compileSource(optSource, "opt_e2e.gs", opts);
    GS_ASSERT_EQ(result.exitCode, 0);
    GS_ASSERT(result.success);

    // Constant folded 10 + 5 = 15, then b = 15 * 2 = 30, propagated to move forward 30
    GS_ASSERT(result.optIRDump.find("30") != std::string::npos);
}

GS_TEST(EndToEndTests, LoopAndFunctionPipeline) {
    std::string source = 
        "function strike:\n"
        "    player attack\n"
        "    player move backward 1\n"
        "\n"
        "repeat 3:\n"
        "    call strike\n";

    driver::CompilerDriver driver;
    driver::CompilerOptions opts;
    opts.dumpAST = true;
    opts.dumpIR = true;
    opts.dumpLLVM = true;
    opts.runSimulation = true;

    auto result = driver.compileSource(source, "func_loop.gs", opts);
    GS_ASSERT_EQ(result.exitCode, 0);
    GS_ASSERT(result.success);
}
