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

GS_TEST(SemanticTests, AcceptFunctionCallBeforeDeclaration) {
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

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
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

GS_TEST(SemanticTests, AcceptFunctionCallingLaterDeclaredFunction) {
    std::string source =
        "function attack:\n"
        "    call defend\n"
        "function defend:\n"
        "    player attack\n";

    DiagnosticEngine diag(source, "mutual_forward_function.gs");
    Lexer lexer(source, "mutual_forward_function.gs", &diag);
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

// ============================================================================
// Enhanced function semantic tests
// ============================================================================

GS_TEST(SemanticTests, FunctionWithZeroParameters) {
    std::string source =
        "function patrol:\n"
        "    player move forward 5\n"
        "    player attack\n"
        "call patrol\n";

    DiagnosticEngine diag(source, "zero_params.gs");
    Lexer lexer(source, "zero_params.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectParameterAccessOutsideFunction) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "player move forward distance\n";  // distance not declared in global scope

    DiagnosticEngine diag(source, "param_outside.gs");
    Lexer lexer(source, "param_outside.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, FunctionCallWithExpressionArgument) {
    std::string source =
        "function move_steps(distance):\n"
        "    player move forward distance\n"
        "set base = 3\n"
        "call move_steps(base + 2)\n";

    DiagnosticEngine diag(source, "expr_arg.gs");
    Lexer lexer(source, "expr_arg.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectFunctionCallWithTooManyArguments) {
    std::string source =
        "function strike(distance):\n"
        "    player move forward distance\n"
        "call strike(5, 90)\n";  // Expects 1 argument, got 2

    DiagnosticEngine diag(source, "too_many_args.gs");
    Lexer lexer(source, "too_many_args.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, AcceptRecursiveFunctionCall) {
    std::string source =
        "function countdown(count):\n"
        "    if count > 0:\n"
        "        player attack\n"
        "        call countdown(count - 1)\n";

    DiagnosticEngine diag(source, "recursion.gs");
    Lexer lexer(source, "recursion.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    // The semantic analyzer should accept recursive calls because functions
    // are collected in pass 1, making them available in pass 2
    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, ParameterShadowsOuterVariable) {
    std::string source =
        "set distance = 10\n"
        "function move_steps(distance):\n"
        "    player move forward distance\n"
        "call move_steps(5)\n"
        "player move forward distance\n";  // Outer distance should still be 10

    DiagnosticEngine diag(source, "param_shadow.gs");
    Lexer lexer(source, "param_shadow.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    // Should be valid: parameter shadows outer variable in function scope
    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectFunctionAndVariableSameName) {
    std::string source =
        "set attack = 10\n"
        "function attack:\n"
        "    player jump\n";

    DiagnosticEngine diag(source, "func_var_same_name.gs");
    Lexer lexer(source, "func_var_same_name.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    // Language design: function names and variable names share the same namespace
    // They cannot have the same name in the same scope
    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, ParameterUsedInFunctionBody) {
    std::string source =
        "function triple_move(distance):\n"
        "    player move forward distance\n"
        "    player move forward distance\n"
        "    player move forward distance\n"
        "call triple_move(2)\n";

    DiagnosticEngine diag(source, "param_reuse.gs");
    Lexer lexer(source, "param_reuse.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, NestedFunctionCalls) {
    std::string source =
        "function inner(x):\n"
        "    player move forward x\n"
        "function outer(y):\n"
        "    call inner(y + 1)\n"
        "call outer(5)\n";

    DiagnosticEngine diag(source, "nested_calls.gs");
    Lexer lexer(source, "nested_calls.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, FunctionCallWithMultipleExpressionArguments) {
    std::string source =
        "function patrol(distance, angle):\n"
        "    player move forward distance\n"
        "    player turn right angle\n"
        "set d = 3\n"
        "set a = 45\n"
        "call patrol(d * 2, a + 45)\n";

    DiagnosticEngine diag(source, "multi_expr_args.gs");
    Lexer lexer(source, "multi_expr_args.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectFunctionCallWithMissingAllArguments) {
    std::string source =
        "function patrol(distance, angle, speed):\n"
        "    player move forward distance\n"
        "call patrol()\n";  // Missing all 3 arguments

    DiagnosticEngine diag(source, "missing_all_args.gs");
    Lexer lexer(source, "missing_all_args.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}

GS_TEST(SemanticTests, FunctionCallingMultipleFunctions) {
    std::string source =
        "function move_forward(d):\n"
        "    player move forward d\n"
        "function turn_right(a):\n"
        "    player turn right a\n"
        "function combined(d, a):\n"
        "    call move_forward(d)\n"
        "    call turn_right(a)\n"
        "call combined(5, 90)\n";

    DiagnosticEngine diag(source, "multi_func_calls.gs");
    Lexer lexer(source, "multi_func_calls.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, KeywordLikeFunctionNames) {
    std::string source =
        "function attack:\n"
        "    player move forward 1\n"
        "function defend:\n"
        "    player jump\n"
        "call attack\n"
        "call defend\n";

    DiagnosticEngine diag(source, "keyword_func_names.gs");
    Lexer lexer(source, "keyword_func_names.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    // Parser now allows action keywords as function names
    GS_ASSERT(ok);
    GS_ASSERT(!diag.hasErrors());
}

GS_TEST(SemanticTests, RejectVariableAndFunctionWithKeywordSameName) {
    std::string source =
        "set forward = 10\n"
        "function forward:\n"
        "    player move forward forward\n"
        "call forward\n";

    DiagnosticEngine diag(source, "keyword_var_and_func.gs");
    Lexer lexer(source, "keyword_var_and_func.gs", &diag);
    Parser parser(lexer.tokenize(), &diag);
    auto program = parser.parseProgram();

    SemanticAnalyzer semantic(&diag);
    bool ok = semantic.analyze(*program);

    // Parser allows keyword names; semantic analyzer should reject duplicate names
    // Functions and variables share the same namespace
    GS_ASSERT(!ok);
    GS_ASSERT(diag.hasErrors());
}
