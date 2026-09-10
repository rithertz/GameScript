#pragma once

//==============================================================================
// Lexer.hpp
//
// Defines the lexical analyzer used by the GameScript compiler to convert
// source code into a sequence of tokens.
//==============================================================================

#include "gamescript/lexer/Token.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

namespace gamescript {

// Performs lexical analysis of GameScript source code.
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
    // Reads the current character without advancing the cursor.
    char peek() const;

    // Reads the character after the current character without advancing the cursor.
    char peekNext() const;

    // Reads the current character and advances the cursor.
    char advance();

    // Matches and consumes the expected character when present.
    bool match(char expected);

    // Checks whether the lexer has reached the end of the source.
    bool isAtEnd() const;

    // Skips whitespace other than newline characters.
    void skipWhitespaceExceptNewline();

    // Skips a comment in the source code.
    void skipComment();

    // Scans the next token from the source.
    Token scanToken();

    // Scans an identifier or keyword token.
    Token scanIdentifierOrKeyword();

    // Scans a numeric literal.
    Token scanNumber();

    // Scans a string literal.
    Token scanString();

    // Processes indentation at the beginning of a source line.
    void processLineIndentation();

    // Looks up a keyword and returns its corresponding token type.
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