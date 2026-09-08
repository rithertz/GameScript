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
