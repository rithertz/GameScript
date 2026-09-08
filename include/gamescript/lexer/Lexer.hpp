#pragma once

#include "gamescript/lexer/Token.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

namespace gamescript {

class Lexer {
public:
    explicit Lexer(std::string source, std::string filename = "", DiagnosticEngine* diagnostics = nullptr);

    // Tokenize entire input stream
    std::vector<Token> tokenize();

    // Pull next token
    Token nextToken();

    // Check if lexer encountered lexical errors
    bool hasErrors() const { return diagnostics_ ? diagnostics_->hasErrors() : hasInternalErrors_; }

private:
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    void skipWhitespaceExceptNewline();
    void skipComment();

    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();

    void processLineIndentation();
    TokenType lookupKeyword(const std::string& text) const;

    std::string source_;
    std::string filename_;
    DiagnosticEngine* diagnostics_ = nullptr;
    bool hasInternalErrors_ = false;

    size_t cursor_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;

    bool atStartOfLine_ = true;
    std::vector<size_t> indentStack_;
    std::vector<Token> pendingTokens_;

    static const std::unordered_map<std::string, TokenType> keywords_;
};

} // namespace gamescript
