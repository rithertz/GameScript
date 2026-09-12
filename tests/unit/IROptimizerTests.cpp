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

GS_TEST(IROptimizerTests, IfElseIRGeneration) {
    std::string source =
        "if health low:\n"
        "    player defend\n"
        "else:\n"
        "    player attack\n";

    Lexer lexer(source, "if_else.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("BR_COND") != std::string::npos);
    GS_ASSERT(irDump.find("GAME_ACTION") != std::string::npos);
}

GS_TEST(IROptimizerTests, RepeatLoopIRGeneration) {
    std::string source =
        "repeat 3:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "repeat.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("BR") != std::string::npos);
    GS_ASSERT(irDump.find("GAME_MOVE") != std::string::npos);
}

GS_TEST(IROptimizerTests, WhileLoopIRGeneration) {
    std::string source =
        "set count = 3\n"
        "while count > 0:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "while.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("BR_COND") != std::string::npos);
    GS_ASSERT(irDump.find("GAME_MOVE") != std::string::npos);
}

GS_TEST(IROptimizerTests, BooleanExpressionIRGeneration) {
    std::string source =
        "if enemy nearby and not health low:\n"
        "    player attack\n";

    Lexer lexer(source, "boolean.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("AND") != std::string::npos);
    GS_ASSERT(irDump.find("NOT") != std::string::npos);
    GS_ASSERT(irDump.find("BR_COND") != std::string::npos);
}

GS_TEST(IROptimizerTests, SensorIRGeneration) {
    std::string source =
        "if enemy nearby:\n"
        "    player attack\n";

    Lexer lexer(source, "sensor.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("GAME_SENSOR") != std::string::npos);
    GS_ASSERT(irDump.find("BR_COND") != std::string::npos);
}

GS_TEST(IROptimizerTests, FunctionCallPreservesSingleArgument) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "call strike(3)\n";

    Lexer lexer(source, "call_single_arg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("CALL") != std::string::npos);
    GS_ASSERT(irDump.find("strike") != std::string::npos);
    GS_ASSERT(irDump.find("3") != std::string::npos);
}

GS_TEST(IROptimizerTests, FunctionCallPreservesMultipleArguments) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn right angle\n"
        "call patrol(5, 90)\n";

    Lexer lexer(source, "call_multiple_args.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("CALL") != std::string::npos);
    GS_ASSERT(irDump.find("patrol") != std::string::npos);
    GS_ASSERT(irDump.find("5") != std::string::npos);
    GS_ASSERT(irDump.find("90") != std::string::npos);
}

GS_TEST(IROptimizerTests, FunctionCallLowersExpressionArgument) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "call strike(2 + 3)\n";

    Lexer lexer(source, "call_expression_arg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    std::string irDump = module->toString();

    GS_ASSERT(irDump.find("CALL") != std::string::npos);
    GS_ASSERT(irDump.find("strike") != std::string::npos);
    GS_ASSERT(irDump.find("ADD") != std::string::npos);
}

GS_TEST(IROptimizerTests, DeadCodeElimination) {
    std::string source =
        "42 + 10\n"
        "player move forward 5\n";

    Lexer lexer(source, "dce.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    std::string before = module->toString();

    GS_ASSERT(before.find("ADD") != std::string::npos);
    GS_ASSERT(before.find("GAME_MOVE") != std::string::npos);

    optimizer::DeadCodeEliminationPass dcePass;
    bool changed = dcePass.runOnModule(*module);

    GS_ASSERT(changed);

    std::string after = module->toString();

    GS_ASSERT(after.find("ADD") == std::string::npos);
    GS_ASSERT(after.find("GAME_MOVE") != std::string::npos);
}

