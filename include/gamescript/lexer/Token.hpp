#pragma once

//==============================================================================
// Token.hpp
//
// Defines token types and the Token structure used by the GameScript lexer
// to represent keywords, literals, operators, and other lexical elements.
//==============================================================================

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <string_view>
#include <cstdint>
#include <iostream>

namespace gamescript {

// Identifies the different types of tokens recognized by the lexer.
enum class TokenType {
    // Control & Structure
    EndOfFile,
    Newline,
    Indent,
    Dedent,

    // Core Keywords
    Set,
    Player,
    Move,
    Turn,
    Attack,
    Defend,
    Jump,
    Interact,
    Retreat,
    If,
    Else,
    Repeat,
    While,
    Function,
    Call,
    When,
    Not,
    And,
    Or,
    True,
    False,

    // Direction & Sensory Keywords
    Forward,
    Backward,
    Left,
    Right,
    Enemy,
    Nearby,
    Health,
    Low,
    Obstacle,
    Ahead,
    Distance,
    To,

    // Identifiers & Literals
    Identifier,
    IntegerLiteral,
    StringLiteral,

    // Symbols & Operators
    Colon,
    Comma,
    Equal,
    EqualEqual,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    LParen,
    RParen,

    // Error / Unknown token
    Unknown
};

// Stores the type, text, source location, and optional integer value of a token.
struct Token {
    TokenType type = TokenType::Unknown;
    std::string lexeme;
    SourceSpan span;
    int64_t intValue = 0;

    Token() = default;
    Token(TokenType type, std::string lexeme, SourceSpan span, int64_t intValue = 0)
        : type(type), lexeme(std::move(lexeme)), span(span), intValue(intValue) {}

    // Checks whether the token has the specified type.
    bool is(TokenType t) const { return type == t; }

    // Checks whether the token does not have the specified type.
    bool isNot(TokenType t) const { return type != t; }

    // Checks whether the token represents a keyword.
    bool isKeyword() const;

    // Checks whether the token represents an action keyword.
    bool isActionKeyword() const;

    // Checks whether the token represents a direction keyword.
    bool isDirectionKeyword() const;

    // Checks whether the token represents a relational operator.
    bool isRelationalOp() const;

    // Checks whether the token represents an arithmetic operator.
    bool isArithmeticOp() const;

    // Returns a human-readable representation of the token.
    std::string toString() const;
};

// Converts a token type to its string representation.
std::string_view tokenTypeToString(TokenType type);

inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    return os << tokenTypeToString(type);
}

inline std::ostream& operator<<(std::ostream& os, const Token& tok) {
    return os << tok.toString();
}

} // namespace gamescript