#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"

using namespace gamescript;

GS_TEST(LexerTests, BasicMovementTokens) {
    std::string source = "player move forward 5\nplayer turn left 90";
    Lexer lexer(source, "test.gs");
    auto tokens = lexer.tokenize();

    GS_ASSERT(!tokens.empty());
    GS_ASSERT_EQ(tokens[0].type, TokenType::Player);
    GS_ASSERT_EQ(tokens[1].type, TokenType::Move);
    GS_ASSERT_EQ(tokens[2].type, TokenType::Forward);
    GS_ASSERT_EQ(tokens[3].type, TokenType::IntegerLiteral);
    GS_ASSERT_EQ(tokens[3].intValue, 5);
    GS_ASSERT_EQ(tokens[4].type, TokenType::Newline);

    GS_ASSERT_EQ(tokens[5].type, TokenType::Player);
    GS_ASSERT_EQ(tokens[6].type, TokenType::Turn);
    GS_ASSERT_EQ(tokens[7].type, TokenType::Left);
    GS_ASSERT_EQ(tokens[8].type, TokenType::IntegerLiteral);
    GS_ASSERT_EQ(tokens[8].intValue, 90);
    GS_ASSERT_EQ(tokens.back().type, TokenType::EndOfFile);
}

GS_TEST(LexerTests, VariableDeclarationAndAssignment) {
    std::string source = "set speed = 10\nset attacks = 3";
    Lexer lexer(source, "vars.gs");
    auto tokens = lexer.tokenize();

    GS_ASSERT_EQ(tokens[0].type, TokenType::Set);
    GS_ASSERT_EQ(tokens[1].type, TokenType::Identifier);
    GS_ASSERT_EQ(tokens[1].lexeme, "speed");
    GS_ASSERT_EQ(tokens[2].type, TokenType::Equal);
    GS_ASSERT_EQ(tokens[3].type, TokenType::IntegerLiteral);
    GS_ASSERT_EQ(tokens[3].intValue, 10);
    GS_ASSERT_EQ(tokens[4].type, TokenType::Newline);

    GS_ASSERT_EQ(tokens[5].type, TokenType::Set);
    GS_ASSERT_EQ(tokens[6].type, TokenType::Identifier);
    GS_ASSERT_EQ(tokens[6].lexeme, "attacks");
}

GS_TEST(LexerTests, IndentationBlocks) {
    std::string source = 
        "if enemy nearby:\n"
        "    player attack\n"
        "    player move backward 2\n"
        "else:\n"
        "    player move forward 5\n";

    Lexer lexer(source, "indent.gs");
    auto tokens = lexer.tokenize();

    // Check tokens stream
    // if enemy nearby : \n INDENT player attack \n player move backward 2 \n DEDENT else : \n INDENT player move forward 5 \n DEDENT EOF
    size_t indentCount = 0;
    size_t dedentCount = 0;
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::Indent) indentCount++;
        if (tok.type == TokenType::Dedent) dedentCount++;
    }

    GS_ASSERT_EQ(indentCount, 2);
    GS_ASSERT_EQ(dedentCount, 2);
}

GS_TEST(LexerTests, CommentsAndWhitespace) {
    std::string source = 
        "# Initial comment\n"
        "set distance = 5 # inline comment\n"
        "\n"
        "# Another comment\n"
        "player move forward distance\n";

    Lexer lexer(source, "comments.gs");
    auto tokens = lexer.tokenize();

    // Verify comments are stripped and only real tokens remain
    GS_ASSERT_EQ(tokens[0].type, TokenType::Set);
    GS_ASSERT_EQ(tokens[1].type, TokenType::Identifier);
    GS_ASSERT_EQ(tokens[2].type, TokenType::Equal);
    GS_ASSERT_EQ(tokens[3].type, TokenType::IntegerLiteral);
    GS_ASSERT_EQ(tokens[4].type, TokenType::Newline);
    GS_ASSERT_EQ(tokens[5].type, TokenType::Player);
}

GS_TEST(LexerTests, RelationalAndArithmeticOperators) {
    std::string source = "== != < <= > >= + - * / % ( ) , :";
    Lexer lexer(source, "ops.gs");
    auto tokens = lexer.tokenize();

    GS_ASSERT_EQ(tokens[0].type, TokenType::EqualEqual);
    GS_ASSERT_EQ(tokens[1].type, TokenType::BangEqual);
    GS_ASSERT_EQ(tokens[2].type, TokenType::Less);
    GS_ASSERT_EQ(tokens[3].type, TokenType::LessEqual);
    GS_ASSERT_EQ(tokens[4].type, TokenType::Greater);
    GS_ASSERT_EQ(tokens[5].type, TokenType::GreaterEqual);
    GS_ASSERT_EQ(tokens[6].type, TokenType::Plus);
    GS_ASSERT_EQ(tokens[7].type, TokenType::Minus);
    GS_ASSERT_EQ(tokens[8].type, TokenType::Star);
    GS_ASSERT_EQ(tokens[9].type, TokenType::Slash);
    GS_ASSERT_EQ(tokens[10].type, TokenType::Percent);
    GS_ASSERT_EQ(tokens[11].type, TokenType::LParen);
    GS_ASSERT_EQ(tokens[12].type, TokenType::RParen);
    GS_ASSERT_EQ(tokens[13].type, TokenType::Comma);
    GS_ASSERT_EQ(tokens[14].type, TokenType::Colon);
}

GS_TEST(LexerTests, SourceSpanTracking) {
    std::string source = "player move forward 10\nplayer attack";
    Lexer lexer(source, "span.gs");
    auto tokens = lexer.tokenize();

    GS_ASSERT_EQ(tokens[0].span.start.line, 1);
    GS_ASSERT_EQ(tokens[0].span.start.column, 1);
    GS_ASSERT_EQ(tokens[0].span.end.column, 6);

    // Line 2: player attack
    // Find second player token
    Token p2 = tokens[5];
    GS_ASSERT_EQ(p2.type, TokenType::Player);
    GS_ASSERT_EQ(p2.span.start.line, 2);
    GS_ASSERT_EQ(p2.span.start.column, 1);
}

GS_TEST(LexerTests, LexicalErrorHandling) {
    std::string source = "player move @ 5\nset $ = 10";
    DiagnosticEngine diag(source, "error.gs");
    Lexer lexer(source, "error.gs", &diag);
    auto tokens = lexer.tokenize();

    GS_ASSERT(diag.hasErrors());
    GS_ASSERT_EQ(diag.getErrorCount(), 2);
}
