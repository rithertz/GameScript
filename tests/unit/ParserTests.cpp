#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/ast/Statements.hpp"
#include "gamescript/ast/ASTPrinter.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"

using namespace gamescript;

GS_TEST(ParserTests, SimpleMovementAndActionAST) {
    std::string source = 
        "player move forward 5\n"
        "player turn right 90\n"
        "player attack\n";

    Lexer lexer(source, "test.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 3);

    auto* moveStmt = dynamic_cast<MoveStmt*>(program->getStatements()[0].get());
    GS_ASSERT(moveStmt != nullptr);
    GS_ASSERT_EQ(moveStmt->getDirection(), MoveDir::Forward);

    auto* turnStmt = dynamic_cast<TurnStmt*>(program->getStatements()[1].get());
    GS_ASSERT(turnStmt != nullptr);
    GS_ASSERT_EQ(turnStmt->getDirection(), TurnDir::Right);

    auto* actionStmt = dynamic_cast<ActionStmt*>(program->getStatements()[2].get());
    GS_ASSERT(actionStmt != nullptr);
    GS_ASSERT_EQ(actionStmt->getAction(), ActionKind::Attack);
}

GS_TEST(ParserTests, ConditionalsIfElse) {
    std::string source = 
        "if enemy nearby:\n"
        "    player attack\n"
        "    player move backward 2\n"
        "else:\n"
        "    player move forward 5\n";

    Lexer lexer(source, "if_else.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 1);

    auto* ifStmt = dynamic_cast<IfStmt*>(program->getStatements()[0].get());
    GS_ASSERT(ifStmt != nullptr);
    GS_ASSERT(ifStmt->getCondition() != nullptr);
    GS_ASSERT_EQ(ifStmt->getThenBranch().size(), 2);
    GS_ASSERT_EQ(ifStmt->getElseBranch().size(), 1);
}

GS_TEST(ParserTests, RepeatAndWhileLoops) {
    std::string source = 
        "repeat 3:\n"
        "    player attack\n"
        "    player move forward 1\n"
        "while not enemy nearby:\n"
        "    player move forward 2\n";

    Lexer lexer(source, "loops.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 2);

    auto* repStmt = dynamic_cast<RepeatStmt*>(program->getStatements()[0].get());
    GS_ASSERT(repStmt != nullptr);
    GS_ASSERT_EQ(repStmt->getBody().size(), 2);

    auto* whileStmt = dynamic_cast<WhileStmt*>(program->getStatements()[1].get());
    GS_ASSERT(whileStmt != nullptr);
    GS_ASSERT_EQ(whileStmt->getBody().size(), 1);
}

GS_TEST(ParserTests, FunctionDeclarationAndCall) {
    std::string source = 
        "function attack_enemy:\n"
        "    player move forward 2\n"
        "    player attack\n"
        "call attack_enemy\n";

    Lexer lexer(source, "func.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 2);

    auto* funcDecl = dynamic_cast<FunctionDeclStmt*>(program->getStatements()[0].get());
    GS_ASSERT(funcDecl != nullptr);
    GS_ASSERT_EQ(funcDecl->getName(), "attack_enemy");
    GS_ASSERT_EQ(funcDecl->getBody().size(), 2);

    auto* funcCall = dynamic_cast<FunctionCallStmt*>(program->getStatements()[1].get());
    GS_ASSERT(funcCall != nullptr);
    GS_ASSERT_EQ(funcCall->getName(), "attack_enemy");
}

GS_TEST(ParserTests, SyntaxErrorDiagnostics) {
    std::string source = "player move forward abc\n"; // Expected integer / expression, found abc (undefined var/token or bad syntax)
    DiagnosticEngine diag(source, "err.gs");
    Lexer lexer(source, "err.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    // The parser successfully parses 'abc' as an IdentifierExpr expression!
    GS_ASSERT(program != nullptr);
    auto* moveStmt = dynamic_cast<MoveStmt*>(program->getStatements()[0].get());
    GS_ASSERT(moveStmt != nullptr);
    auto* idExpr = dynamic_cast<IdentifierExpr*>(moveStmt->getDistance());
    GS_ASSERT(idExpr != nullptr);
    GS_ASSERT_EQ(idExpr->getName(), "abc");
}
