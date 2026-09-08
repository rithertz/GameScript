#pragma once

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <string_view>
#include <cstdint>
#include <iostream>

namespace gamescript {

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

struct Token {
    TokenType type = TokenType::Unknown;
    std::string lexeme;
    SourceSpan span;
    int64_t intValue = 0;

    Token() = default;
    Token(TokenType type, std::string lexeme, SourceSpan span, int64_t intValue = 0)
        : type(type), lexeme(std::move(lexeme)), span(span), intValue(intValue) {}

    bool is(TokenType t) const { return type == t; }
    bool isNot(TokenType t) const { return type != t; }
    
    bool isKeyword() const;
    bool isActionKeyword() const;
    bool isDirectionKeyword() const;
    bool isRelationalOp() const;
    bool isArithmeticOp() const;

    std::string toString() const;
};

std::string_view tokenTypeToString(TokenType type);

inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    return os << tokenTypeToString(type);
}

inline std::ostream& operator<<(std::ostream& os, const Token& tok) {
    return os << tok.toString();
}

} // namespace gamescript
