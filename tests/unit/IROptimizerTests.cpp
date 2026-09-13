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

GS_TEST(IROptimizerTests, IfElseCFGEdges) {
    std::string source =
        "if health low:\n"
        "    player defend\n"
        "else:\n"
        "    player attack\n";

    Lexer lexer(source, "if_else_cfg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    auto* mainFunction = module->getMainFunction();
    GS_ASSERT(mainFunction != nullptr);
    GS_ASSERT(mainFunction->blocks.size() == 4);

    auto* entryBlock = mainFunction->blocks[0].get();
    auto* mergeBlock = mainFunction->blocks[1].get();
    auto* thenBlock = mainFunction->blocks[2].get();
    auto* elseBlock = mainFunction->blocks[3].get();

    GS_ASSERT(entryBlock->getSuccessors().size() == 2);

    GS_ASSERT(thenBlock->getPredecessors().size() == 1);
    GS_ASSERT(elseBlock->getPredecessors().size() == 1);

    GS_ASSERT(thenBlock->getPredecessors()[0] == entryBlock->getName());
    GS_ASSERT(elseBlock->getPredecessors()[0] == entryBlock->getName());

    GS_ASSERT(thenBlock->getSuccessors().size() == 1);
    GS_ASSERT(elseBlock->getSuccessors().size() == 1);

    GS_ASSERT(thenBlock->getSuccessors()[0] == mergeBlock->getName());
    GS_ASSERT(elseBlock->getSuccessors()[0] == mergeBlock->getName());

    GS_ASSERT(mergeBlock->getPredecessors().size() == 2);
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

GS_TEST(IROptimizerTests, RepeatCFGEdges) {
    std::string source =
        "repeat 3:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "repeat_cfg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    auto* mainFunction = module->getMainFunction();
    GS_ASSERT(mainFunction != nullptr);
    GS_ASSERT(mainFunction->blocks.size() == 4);

    auto* entryBlock = mainFunction->blocks[0].get();
    auto* headBlock = mainFunction->blocks[1].get();
    auto* bodyBlock = mainFunction->blocks[2].get();
    auto* exitBlock = mainFunction->blocks[3].get();

    GS_ASSERT(entryBlock->getSuccessors().size() == 1);
    GS_ASSERT(headBlock->getPredecessors().size() == 2);

    GS_ASSERT(headBlock->getPredecessors()[0] == entryBlock->getName());
    GS_ASSERT(headBlock->getPredecessors()[1] == bodyBlock->getName());

    GS_ASSERT(headBlock->getSuccessors().size() == 2);
    GS_ASSERT(bodyBlock->getPredecessors().size() == 1);
    GS_ASSERT(exitBlock->getPredecessors().size() == 1);

    GS_ASSERT(bodyBlock->getPredecessors()[0] == headBlock->getName());
    GS_ASSERT(exitBlock->getPredecessors()[0] == headBlock->getName());

    GS_ASSERT(bodyBlock->getSuccessors().size() == 1);
    GS_ASSERT(bodyBlock->getSuccessors()[0] == headBlock->getName());
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

GS_TEST(IROptimizerTests, WhileCFGEdges) {
    std::string source =
        "set count = 3\n"
        "while count > 0:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "while_cfg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    auto* mainFunction = module->getMainFunction();
    GS_ASSERT(mainFunction != nullptr);
    GS_ASSERT(mainFunction->blocks.size() == 4);

    auto* entryBlock = mainFunction->blocks[0].get();
    auto* headBlock = mainFunction->blocks[1].get();
    auto* bodyBlock = mainFunction->blocks[2].get();
    auto* exitBlock = mainFunction->blocks[3].get();

    GS_ASSERT(entryBlock->getSuccessors().size() == 1);

    GS_ASSERT(headBlock->getPredecessors().size() == 2);
    GS_ASSERT(headBlock->getPredecessors()[0] == entryBlock->getName());
    GS_ASSERT(headBlock->getPredecessors()[1] == bodyBlock->getName());

    GS_ASSERT(headBlock->getSuccessors().size() == 2);

    GS_ASSERT(bodyBlock->getPredecessors().size() == 1);
    GS_ASSERT(bodyBlock->getPredecessors()[0] == headBlock->getName());

    GS_ASSERT(exitBlock->getPredecessors().size() == 1);
    GS_ASSERT(exitBlock->getPredecessors()[0] == headBlock->getName());

    GS_ASSERT(bodyBlock->getSuccessors().size() == 1);
    GS_ASSERT(bodyBlock->getSuccessors()[0] == headBlock->getName());
}

GS_TEST(IROptimizerTests, NestedIfWhileCFGEdges) {
    std::string source =
        "set count = 3\n"
        "while count > 0:\n"
        "    if health low:\n"
        "        player defend\n"
        "    else:\n"
        "        player attack\n";

    Lexer lexer(source, "nested_if_while_cfg.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GS_ASSERT(module != nullptr);

    auto* mainFunction = module->getMainFunction();
    GS_ASSERT(mainFunction != nullptr);

    // Expected blocks:
    // entry -> while head -> while body
    // while body -> if then / if else
    // if then / if else -> if merge
    // if merge -> while head
    GS_ASSERT(mainFunction->blocks.size() == 7);

    auto* entryBlock = mainFunction->blocks[0].get();
    auto* headBlock = mainFunction->blocks[1].get();
    auto* bodyBlock = mainFunction->blocks[2].get();
    auto* mergeBlock = mainFunction->blocks[3].get();
    auto* thenBlock = mainFunction->blocks[4].get();
    auto* elseBlock = mainFunction->blocks[5].get();
    auto* exitBlock = mainFunction->blocks[6].get();

    // Entry -> while head
    GS_ASSERT(entryBlock->getSuccessors().size() == 1);
    GS_ASSERT(entryBlock->getSuccessors()[0] == headBlock->getName());

    // While head -> body and exit
    GS_ASSERT(headBlock->getPredecessors().size() == 2);
    GS_ASSERT(headBlock->getPredecessors()[0] == entryBlock->getName());
    GS_ASSERT(headBlock->getPredecessors()[1] == mergeBlock->getName());

    GS_ASSERT(headBlock->getSuccessors().size() == 2);

    // While body -> nested if branches
    GS_ASSERT(bodyBlock->getSuccessors().size() == 2);

    GS_ASSERT(thenBlock->getPredecessors().size() == 1);
    GS_ASSERT(thenBlock->getPredecessors()[0] == bodyBlock->getName());

    GS_ASSERT(elseBlock->getPredecessors().size() == 1);
    GS_ASSERT(elseBlock->getPredecessors()[0] == bodyBlock->getName());

    // Nested if branches -> merge
    GS_ASSERT(thenBlock->getSuccessors().size() == 1);
    GS_ASSERT(thenBlock->getSuccessors()[0] == mergeBlock->getName());

    GS_ASSERT(elseBlock->getSuccessors().size() == 1);
    GS_ASSERT(elseBlock->getSuccessors()[0] == mergeBlock->getName());

    GS_ASSERT(mergeBlock->getPredecessors().size() == 2);
    GS_ASSERT(mergeBlock->getPredecessors()[0] == thenBlock->getName());
    GS_ASSERT(mergeBlock->getPredecessors()[1] == elseBlock->getName());

    // Nested if merge -> while head (loop back-edge)
    GS_ASSERT(mergeBlock->getSuccessors().size() == 1);
    GS_ASSERT(mergeBlock->getSuccessors()[0] == headBlock->getName());

    // While exit -> no successor
    GS_ASSERT(exitBlock->getPredecessors().size() == 1);
    GS_ASSERT(exitBlock->getPredecessors()[0] == headBlock->getName());
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

