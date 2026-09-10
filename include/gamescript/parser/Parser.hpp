#pragma once

//==============================================================================
// Parser.hpp
//
// Defines the parser used by the GameScript compiler to convert a sequence
// of tokens produced by the lexer into an Abstract Syntax Tree (AST).
//==============================================================================

#include "gamescript/lexer/Token.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include "gamescript/ast/Statements.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"
#include <vector>
#include <memory>
#include <string>

namespace gamescript {

// Parses GameScript tokens and builds the corresponding Abstract Syntax Tree.
class Parser {
public:
    explicit Parser(std::vector<Token> tokens, DiagnosticEngine* diagnostics = nullptr);

    // Parses the complete token sequence into a program AST.
    std::unique_ptr<Program> parseProgram();

    // Check if parser encountered errors.
    bool hasErrors() const { return diagnostics_ ? diagnostics_->hasErrors() : hasInternalErrors_; }

private:
    // Token navigation helpers
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool matchAny(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& errorMessage);

    void skipNewlines();
    void synchronize();

    // Statement parsers
    StmtPtr parseStatement();
    StmtPtr parseVarDecl();
    StmtPtr parsePlayerStatement();
    StmtPtr parseIfStatement();
    StmtPtr parseRepeatStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseFunctionDecl();
    StmtPtr parseFunctionCall();
    StmtPtr parseWhenStatement();
    StmtPtr parseExprStatement();

    std::vector<StmtPtr> parseBlock();

    // Expression & Condition parsers
    ExprPtr parseCondition();
    ExprPtr parseLogicOr();
    ExprPtr parseLogicAnd();
    ExprPtr parseLogicNot();
    ExprPtr parseRelational();
    ExprPtr parseExpression();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parsePrimary();

    std::vector<Token> tokens_;
    DiagnosticEngine* diagnostics_ = nullptr;
    size_t current_ = 0;
    bool hasInternalErrors_ = false;
};

} // namespace gamescript