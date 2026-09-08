#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/semantic/SemanticAnalyzer.hpp"
#include "gamescript/ir/IRBuilder.hpp"
#include "gamescript/ir/IRPrinter.hpp"
#include "gamescript/optimizer/PassManager.hpp"
#include "gamescript/optimizer/ConstantFoldingPass.hpp"
#include "gamescript/optimizer/ConstantPropagationPass.hpp"
#include "gamescript/optimizer/DeadCodeEliminationPass.hpp"
#include "gamescript/optimizer/RedundantInstEliminationPass.hpp"

using namespace gamescript;

GS_TEST(IROptimizerTests, IRGenerationBasic) {
    std::string source = 
        "set speed = 5\n"
        "player move forward speed\n"
        "player turn left 90\n"
        "player attack\n";

    Lexer lexer(source, "basic.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);
    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("GAME_MOVE") != std::string::npos);
    GS_ASSERT(irDump.find("GAME_TURN") != std::string::npos);
    GS_ASSERT(irDump.find("GAME_ACTION") != std::string::npos);
}

GS_TEST(IROptimizerTests, ConstantFoldingOptimization) {
    std::string source = "set distance = 2 + 3\n";

    Lexer lexer(source, "fold.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    optimizer::ConstantFoldingPass foldPass;
    bool changed = foldPass.runOnModule(*module);
    GS_ASSERT(changed);

    std::string irDump = module->toString();
    GS_ASSERT(irDump.find("5") != std::string::npos);
    GS_ASSERT(irDump.find("constant-folded") != std::string::npos);
}

GS_TEST(IROptimizerTests, ConstantPropagationOptimization) {
    std::string source = 
        "set speed = 10\n"
        "player move forward speed\n";

    Lexer lexer(source, "prop.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    optimizer::ConstantPropagationPass propPass;
    bool changed = propPass.runOnModule(*module);
    GS_ASSERT(changed);

    std::string irDump = module->toString();
    GS_ASSERT(irDump.find("GAME_MOVE    0, 10") != std::string::npos || irDump.find("propagated constant") != std::string::npos);
}

GS_TEST(IROptimizerTests, RedundantMovementElimination) {
    std::string source = 
        "player move forward 5\n"
        "player move forward 0\n"
        "player turn left 0\n";

    Lexer lexer(source, "red.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    optimizer::RedundantInstEliminationPass redPass;
    bool changed = redPass.runOnModule(*module);
    GS_ASSERT(changed);

    std::string irDump = module->toString();
    GS_ASSERT(irDump.find("player move FORWARD") != std::string::npos);
}
