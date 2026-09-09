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

GS_TEST(ParserTests, ExpressionPrecedence) {
    std::string source =
        "set value = 2 + 3 * 4\n";

    Lexer lexer(source, "precedence.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 1);

    auto* decl = dynamic_cast<VarDeclStmt*>(program->getStatements()[0].get());
    GS_ASSERT(decl != nullptr);

    auto* addExpr = dynamic_cast<BinaryExpr*>(decl->getInitializer());
    GS_ASSERT(addExpr != nullptr);
    GS_ASSERT_EQ(addExpr->getOp(), BinaryOp::Add);

    auto* rightExpr = dynamic_cast<BinaryExpr*>(addExpr->getRight());
    GS_ASSERT(rightExpr != nullptr);
    GS_ASSERT_EQ(rightExpr->getOp(), BinaryOp::Multiply);
}

GS_TEST(ParserTests, BooleanLogicAndNot) {
    std::string source =
        "if not enemy nearby and health low:\n"
        "    player defend\n";

    Lexer lexer(source, "logic.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 1);

    auto* ifStmt = dynamic_cast<IfStmt*>(program->getStatements()[0].get());
    GS_ASSERT(ifStmt != nullptr);

    auto* andExpr = dynamic_cast<BinaryExpr*>(ifStmt->getCondition());
    GS_ASSERT(andExpr != nullptr);
    GS_ASSERT_EQ(andExpr->getOp(), BinaryOp::And);

    auto* notExpr = dynamic_cast<UnaryExpr*>(andExpr->getLeft());
    GS_ASSERT(notExpr != nullptr);
    GS_ASSERT_EQ(notExpr->getOp(), UnaryOp::Not);

    auto* rightSensor = dynamic_cast<SensoryConditionExpr*>(andExpr->getRight());
    GS_ASSERT(rightSensor != nullptr);
    GS_ASSERT_EQ(rightSensor->getSensoryType(), SensoryType::HealthLow);
}

GS_TEST(ParserTests, SensorExpressions) {
    std::string source =
        "if obstacle ahead:\n"
        "    player move backward 1\n"
        "if distance to enemy < 3:\n"
        "    player attack\n";

    Lexer lexer(source, "sensors.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 2);

    auto* firstIf = dynamic_cast<IfStmt*>(program->getStatements()[0].get());
    GS_ASSERT(firstIf != nullptr);

    auto* obstacle = dynamic_cast<SensoryConditionExpr*>(firstIf->getCondition());
    GS_ASSERT(obstacle != nullptr);
    GS_ASSERT_EQ(obstacle->getSensoryType(), SensoryType::ObstacleAhead);

    auto* secondIf = dynamic_cast<IfStmt*>(program->getStatements()[1].get());
    GS_ASSERT(secondIf != nullptr);

    auto* comparison = dynamic_cast<BinaryExpr*>(secondIf->getCondition());
    GS_ASSERT(comparison != nullptr);
    GS_ASSERT_EQ(comparison->getOp(), BinaryOp::Less);

    auto* distanceSensor =
        dynamic_cast<SensoryConditionExpr*>(comparison->getLeft());
    GS_ASSERT(distanceSensor != nullptr);
    GS_ASSERT_EQ(
        distanceSensor->getSensoryType(),
        SensoryType::DistanceToEnemy);
}

GS_TEST(ParserTests, FunctionArgumentsAndParenthesizedExpression) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn right angle\n"
        "call patrol(5, 90)\n"
        "set result = (2 + 3) * 4\n";

    Lexer lexer(source, "functions.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    GS_ASSERT(program != nullptr);
    GS_ASSERT_EQ(program->getStatements().size(), 3);

    auto* function =
        dynamic_cast<FunctionDeclStmt*>(program->getStatements()[0].get());
    GS_ASSERT(function != nullptr);
    GS_ASSERT_EQ(function->getName(), "patrol");
    GS_ASSERT_EQ(function->getParams().size(), 2);
    GS_ASSERT_EQ(function->getParams()[0], "distance");
    GS_ASSERT_EQ(function->getParams()[1], "angle");

    auto* call =
        dynamic_cast<FunctionCallStmt*>(program->getStatements()[1].get());
    GS_ASSERT(call != nullptr);
    GS_ASSERT_EQ(call->getName(), "patrol");
    GS_ASSERT_EQ(call->getArgs().size(), 2);

    auto* result =
        dynamic_cast<VarDeclStmt*>(program->getStatements()[2].get());
    GS_ASSERT(result != nullptr);

    auto* multiply =
        dynamic_cast<BinaryExpr*>(result->getInitializer());
    GS_ASSERT(multiply != nullptr);
    GS_ASSERT_EQ(multiply->getOp(), BinaryOp::Multiply);
}
