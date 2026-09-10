#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/ast/ASTPrinter.hpp"

using namespace gamescript;

GS_TEST(ASTTests, ASTPrettyPrinterFormat) {
    std::string source =
        "set distance = 5\n"
        "player move forward distance\n"
        "if enemy nearby:\n"
        "    player attack\n";

    Lexer lexer(source, "ast_test.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(!output.empty());
    GS_ASSERT(output.find("Program") != std::string::npos);
    GS_ASSERT(output.find("VarDecl: distance") != std::string::npos);
    GS_ASSERT(output.find("Move: FORWARD") != std::string::npos);
    GS_ASSERT(output.find("EnemyNearby") != std::string::npos);
    GS_ASSERT(output.find("Action: ATTACK") != std::string::npos);
}

GS_TEST(ASTTests, ExpressionNodes) {
    std::string source =
        "set distance = 5 + 3 * 2\n";

    Lexer lexer(source, "ast_expression.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(output.find("VarDecl: distance") != std::string::npos);
    GS_ASSERT(output.find("Value: (5 + (3 * 2))") != std::string::npos);
}

GS_TEST(ASTTests, IfElseTree) {
    std::string source =
        "if health low:\n"
        "    player defend\n"
        "else:\n"
        "    player attack\n";

    Lexer lexer(source, "ast_if_else.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(output.find("If") != std::string::npos);
    GS_ASSERT(output.find("HealthLow") != std::string::npos);
    GS_ASSERT(output.find("Action: DEFEND") != std::string::npos);
    GS_ASSERT(output.find("Action: ATTACK") != std::string::npos);
}

GS_TEST(ASTTests, LoopNodes) {
    std::string source =
        "repeat 3:\n"
        "    player move forward 1\n"
        "while health > 0:\n"
        "    player defend\n";

    Lexer lexer(source, "ast_loops.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(output.find("Repeat") != std::string::npos);
    GS_ASSERT(output.find("While") != std::string::npos);
    GS_ASSERT(output.find("Move: FORWARD") != std::string::npos);
    GS_ASSERT(output.find("Action: DEFEND") != std::string::npos);
}

GS_TEST(ASTTests, FunctionAndCallTree) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn left angle\n"
        "call patrol(5, 90)\n";

    Lexer lexer(source, "ast_function.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(output.find("Function: patrol") != std::string::npos);
    GS_ASSERT(output.find("distance") != std::string::npos);
    GS_ASSERT(output.find("angle") != std::string::npos);
    GS_ASSERT(output.find("Move: FORWARD") != std::string::npos);
    GS_ASSERT(output.find("Turn: LEFT") != std::string::npos);
    GS_ASSERT(output.find("Call: patrol") != std::string::npos);
}

GS_TEST(ASTTests, BooleanAndSensorTree) {
    std::string source =
        "if enemy nearby and not health low:\n"
        "    player attack\n";

    Lexer lexer(source, "ast_boolean.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);

    ASTPrinter printer;
    std::string output = printer.print(*program);

    GS_ASSERT(output.find("EnemyNearby") != std::string::npos);
    GS_ASSERT(output.find("HealthLow") != std::string::npos);
    GS_ASSERT(output.find("Condition: (EnemyNearby and (not HealthLow))") != std::string::npos);
    GS_ASSERT(output.find("Action: ATTACK") != std::string::npos);
}