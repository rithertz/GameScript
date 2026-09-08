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
