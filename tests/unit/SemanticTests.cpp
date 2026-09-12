#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/semantic/SemanticAnalyzer.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"

using namespace gamescript;

GS_TEST(SemanticTests, ValidVariablesAndScopes) {
    std::string source = 
        "set speed = 5\n"
        "player move forward speed\n"
        "if enemy nearby:\n"
        "    set local_boost = 2\n"
        "    player move forward local_boost\n";

    DiagnosticEngine diag(source, "valid.gs");
    Lexer lexer(source, "valid.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    GS_ASSERT(!diag.hasErrors());
    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, DetectUndeclaredVariable) {
    std::string source = "player move forward speed\n"; // speed not declared
    DiagnosticEngine diag(source, "undeclared.gs");
    Lexer lexer(source, "undeclared.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
    GS_ASSERT_EQ(diag.getErrorCount(), 1);
}

GS_TEST(SemanticTests, DetectDuplicateDeclarationInSameScope) {
    std::string source = 
        "set count = 5\n"
        "set count = 10\n";

    DiagnosticEngine diag(source, "dup.gs");
    Lexer lexer(source, "dup.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, DetectInvalidFunctionCall) {
    std::string source = 
        "function patrol(distance):\n"
        "    player move forward distance\n"
        "call patrol\n"; // Missing argument

    DiagnosticEngine diag(source, "call_err.gs");
    Lexer lexer(source, "call_err.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, RejectNonIntegerMovementDistance) {
    std::string source =
        "set distance = true\n"
        "player move forward distance\n";

    DiagnosticEngine diag(source, "bad_move_type.gs");
    Lexer lexer(source, "bad_move_type.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, RejectNonBooleanCondition) {
    std::string source =
        "set value = 10\n"
        "if value:\n"
        "    player attack\n";

    DiagnosticEngine diag(source, "bad_condition.gs");
    Lexer lexer(source, "bad_condition.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, RejectInvalidArithmeticTypes) {
    std::string source =
        "set enabled = true\n"
        "set result = enabled + 5\n";

    DiagnosticEngine diag(source, "bad_arithmetic.gs");
    Lexer lexer(source, "bad_arithmetic.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, AcceptFunctionCallWithCorrectArgumentCount) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn right angle\n"
        "call patrol(5, 90)\n";

    DiagnosticEngine diag(source, "valid_function.gs");
    Lexer lexer(source, "valid_function.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectFunctionCallWithInvalidArgumentType) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "call strike(true)\n";

    DiagnosticEngine diag(source, "invalid_function_arg_type.gs");
    Lexer lexer(source, "invalid_function_arg_type.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, AcceptFunctionCallWithMultipleIntegerArguments) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn right angle\n"
        "call patrol(5, 90)\n";

    DiagnosticEngine diag(source, "multiple_function_args.gs");
    Lexer lexer(source, "multiple_function_args.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectDuplicateFunctionParameters) {
    std::string source =
        "function patrol(distance, distance):\n"
        "    player move forward distance\n";

    DiagnosticEngine diag(source, "duplicate_function_params.gs");
    Lexer lexer(source, "duplicate_function_params.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, RejectFunctionCallWithBooleanExpression) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "call strike(enemy nearby)\n";

    DiagnosticEngine diag(source, "boolean_function_arg.gs");
    Lexer lexer(source, "boolean_function_arg.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, DetectUndefinedFunction) {
    std::string source =
        "call missing_function\n";

    DiagnosticEngine diag(source, "missing_function.gs");
    Lexer lexer(source, "missing_function.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, DetectDuplicateFunctionDeclaration) {
    std::string source =
        "function patrol:\n"
        "    player move forward 1\n"
        "function patrol:\n"
        "    player attack\n";

    DiagnosticEngine diag(source, "duplicate_function.gs");
    Lexer lexer(source, "duplicate_function.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, DetectFunctionCallBeforeDeclaration) {
    std::string source =
        "call patrol\n"
        "function patrol:\n"
        "    player attack\n";

    DiagnosticEngine diag(source, "forward_function.gs");
    Lexer lexer(source, "forward_function.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    /*
     * The current semantic analyzer performs declaration/use
     * checking in source order, so a function must be declared
     * before it is called.
     */
    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, AcceptBooleanLogic) {
    std::string source =
        "if enemy nearby and not health low:\n"
        "    player attack\n";

    DiagnosticEngine diag(source, "boolean_logic.gs");
    Lexer lexer(source, "boolean_logic.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectInvalidComparisonTypes) {
    std::string source =
        "set enabled = true\n"
        "set result = enabled < 5\n";

    DiagnosticEngine diag(source, "bad_comparison.gs");
    Lexer lexer(source, "bad_comparison.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, AllowNestedScopeVariableAccess) {
    std::string source =
        "set speed = 5\n"
        "if enemy nearby:\n"
        "    player move forward speed\n";

    DiagnosticEngine diag(source, "nested_scope.gs");
    Lexer lexer(source, "nested_scope.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}
