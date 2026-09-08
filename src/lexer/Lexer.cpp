#include "gamescript/lexer/Lexer.hpp"
#include <cctype>
#include <stdexcept>

namespace gamescript {

const std::unordered_map<std::string, TokenType> Lexer::keywords_ = {
    {"set",       TokenType::Set},
    {"player",    TokenType::Player},
    {"move",      TokenType::Move},
    {"turn",      TokenType::Turn},
    {"attack",    TokenType::Attack},
    {"defend",    TokenType::Defend},
    {"jump",      TokenType::Jump},
    {"interact",  TokenType::Interact},
    {"retreat",   TokenType::Retreat},
    {"if",        TokenType::If},
    {"else",      TokenType::Else},
    {"repeat",    TokenType::Repeat},
    {"while",     TokenType::While},
    {"function",  TokenType::Function},
    {"call",      TokenType::Call},
    {"when",      TokenType::When},
    {"not",       TokenType::Not},
    {"and",       TokenType::And},
    {"or",        TokenType::Or},
    {"true",      TokenType::True},
    {"false",     TokenType::False},
    {"forward",   TokenType::Forward},
    {"backward",  TokenType::Backward},
    {"left",      TokenType::Left},
    {"right",     TokenType::Right},
    {"enemy",     TokenType::Enemy},
    {"nearby",    TokenType::Nearby},
    {"health",    TokenType::Health},
    {"low",       TokenType::Low},
    {"obstacle",  TokenType::Obstacle},
    {"ahead",     TokenType::Ahead}
};

Lexer::Lexer(std::string source, std::string filename, DiagnosticEngine* diagnostics)
    : source_(std::move(source)),
      filename_(std::move(filename)),
      diagnostics_(diagnostics) {
    indentStack_.push_back(0); // Base indentation level
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[cursor_];
}

char Lexer::peekNext() const {
    if (cursor_ + 1 >= source_.size()) return '\0';
    return source_[cursor_ + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[cursor_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[cursor_] != expected) return false;
    advance();
    return true;
}

bool Lexer::isAtEnd() const {
    return cursor_ >= source_.size();
}

void Lexer::skipWhitespaceExceptNewline() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Lexer::processLineIndentation() {
    atStartOfLine_ = false;

    // Scan leading whitespace and comments
    size_t currentIndent = 0;
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ') {
            currentIndent += 1;
            advance();
        } else if (c == '\t') {
            currentIndent += 4;
            advance();
        } else if (c == '\r') {
            advance();
        } else if (c == '#') {
            skipComment();
        } else if (c == '\n') {
            // Blank line, advance and reset indent calculation
            advance();
            currentIndent = 0;
        } else {
            // Reached non-whitespace, non-comment character
            break;
        }
    }

    if (isAtEnd()) {
        return;
    }

    size_t topIndent = indentStack_.back();
    SourceLocation loc{line_, column_, cursor_};
    SourceSpan span(loc, loc, filename_);

    if (currentIndent > topIndent) {
        indentStack_.push_back(currentIndent);
        pendingTokens_.push_back(Token(TokenType::Indent, "<INDENT>", span));
    } else if (currentIndent < topIndent) {
        while (indentStack_.size() > 1 && indentStack_.back() > currentIndent) {
            indentStack_.pop_back();
            pendingTokens_.push_back(Token(TokenType::Dedent, "<DEDENT>", span));
        }

        if (indentStack_.back() != currentIndent) {
            std::string msg = "Inconsistent indentation level. Expected indent level to align with a previous block.";
            if (diagnostics_) {
                diagnostics_->reportLexicalError(span, msg);
            }
            hasInternalErrors_ = true;
        }
    }
}

TokenType Lexer::lookupKeyword(const std::string& text) const {
    auto it = keywords_.find(text);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::Identifier;
}

Token Lexer::scanIdentifierOrKeyword() {
    SourceLocation startLoc{line_, column_, cursor_};
    std::string text;

    while (!isAtEnd()) {
        char c = peek();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            text += advance();
        } else {
            break;
        }
    }

    SourceLocation endLoc{line_, column_ - 1, cursor_ - 1};
    SourceSpan span(startLoc, endLoc, filename_);
    TokenType type = lookupKeyword(text);
    return Token(type, text, span);
}

Token Lexer::scanNumber() {
    SourceLocation startLoc{line_, column_, cursor_};
    std::string text;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        text += advance();
    }

    SourceLocation endLoc{line_, column_ - 1, cursor_ - 1};
    SourceSpan span(startLoc, endLoc, filename_);
    int64_t val = 0;
    try {
        val = std::stoll(text);
    } catch (...) {
        if (diagnostics_) {
            diagnostics_->reportLexicalError(span, "Integer literal '" + text + "' is out of 64-bit range.");
        }
        hasInternalErrors_ = true;
    }

    return Token(TokenType::IntegerLiteral, text, span, val);
}

Token Lexer::scanString() {
    SourceLocation startLoc{line_, column_, cursor_};
    advance(); // Consume opening quote '"'
    std::string text;

    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            SourceLocation endLoc{line_, column_, cursor_};
            SourceSpan span(startLoc, endLoc, filename_);
            if (diagnostics_) {
                diagnostics_->reportLexicalError(span, "Unterminated string literal (newline in string).");
            }
            hasInternalErrors_ = true;
            return Token(TokenType::StringLiteral, text, span);
        }
        if (peek() == '\\') {
            advance(); // escape char
            if (!isAtEnd()) {
                char esc = advance();
                switch (esc) {
                    case 'n': text += '\n'; break;
                    case 't': text += '\t'; break;
                    case 'r': text += '\r'; break;
                    case '"': text += '"'; break;
                    case '\\': text += '\\'; break;
                    default: text += esc; break;
                }
            }
        } else {
            text += advance();
        }
    }

    if (isAtEnd()) {
        SourceLocation endLoc{line_, column_, cursor_};
        SourceSpan span(startLoc, endLoc, filename_);
        if (diagnostics_) {
            diagnostics_->reportLexicalError(span, "Unterminated string literal at end of file.");
        }
        hasInternalErrors_ = true;
        return Token(TokenType::StringLiteral, text, span);
    }

    advance(); // Consume closing quote '"'
    SourceLocation endLoc{line_, column_ - 1, cursor_ - 1};
    SourceSpan span(startLoc, endLoc, filename_);
    return Token(TokenType::StringLiteral, text, span);
}

Token Lexer::scanToken() {
    if (atStartOfLine_) {
        processLineIndentation();
        if (!pendingTokens_.empty()) {
            Token tok = pendingTokens_.front();
            pendingTokens_.erase(pendingTokens_.begin());
            return tok;
        }
    }

    skipWhitespaceExceptNewline();

    if (isAtEnd()) {
        // Emit remaining dedents before EOF
        if (indentStack_.size() > 1) {
            indentStack_.pop_back();
            SourceLocation loc{line_, column_, cursor_};
            return Token(TokenType::Dedent, "<DEDENT>", SourceSpan::single(loc, filename_));
        }
        SourceLocation loc{line_, column_, cursor_};
        return Token(TokenType::EndOfFile, "", SourceSpan::single(loc, filename_));
    }

    char c = peek();

    // Comments
    if (c == '#') {
        skipComment();
        return scanToken();
    }

    // Newlines
    if (c == '\n') {
        SourceLocation startLoc{line_, column_, cursor_};
        advance();
        atStartOfLine_ = true;
        SourceLocation endLoc{line_, column_ - 1, cursor_ - 1};
        return Token(TokenType::Newline, "\\n", SourceSpan(startLoc, endLoc, filename_));
    }

    // Identifiers & Keywords
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return scanIdentifierOrKeyword();
    }

    // Number literals
    if (std::isdigit(static_cast<unsigned char>(c))) {
        return scanNumber();
    }

    // String literals
    if (c == '"') {
        return scanString();
    }

    // Operators and delimiters
    SourceLocation startLoc{line_, column_, cursor_};
    char op = advance();
    SourceLocation endLoc{line_, column_ - 1, cursor_ - 1};
    SourceSpan singleSpan(startLoc, endLoc, filename_);

    switch (op) {
        case ':': return Token(TokenType::Colon, ":", singleSpan);
        case ',': return Token(TokenType::Comma, ",", singleSpan);
        case '(': return Token(TokenType::LParen, "(", singleSpan);
        case ')': return Token(TokenType::RParen, ")", singleSpan);
        case '+': return Token(TokenType::Plus, "+", singleSpan);
        case '-': return Token(TokenType::Minus, "-", singleSpan);
        case '*': return Token(TokenType::Star, "*", singleSpan);
        case '/': return Token(TokenType::Slash, "/", singleSpan);
        case '%': return Token(TokenType::Percent, "%", singleSpan);
        case '=':
            if (match('=')) {
                SourceLocation doubleEndLoc{line_, column_ - 1, cursor_ - 1};
                return Token(TokenType::EqualEqual, "==", SourceSpan(startLoc, doubleEndLoc, filename_));
            }
            return Token(TokenType::Equal, "=", singleSpan);
        case '!':
            if (match('=')) {
                SourceLocation doubleEndLoc{line_, column_ - 1, cursor_ - 1};
                return Token(TokenType::BangEqual, "!=", SourceSpan(startLoc, doubleEndLoc, filename_));
            }
            break;
        case '<':
            if (match('=')) {
                SourceLocation doubleEndLoc{line_, column_ - 1, cursor_ - 1};
                return Token(TokenType::LessEqual, "<=", SourceSpan(startLoc, doubleEndLoc, filename_));
            }
            return Token(TokenType::Less, "<", singleSpan);
        case '>':
            if (match('=')) {
                SourceLocation doubleEndLoc{line_, column_ - 1, cursor_ - 1};
                return Token(TokenType::GreaterEqual, ">=", SourceSpan(startLoc, doubleEndLoc, filename_));
            }
            return Token(TokenType::Greater, ">", singleSpan);
        default:
            break;
    }

    // Unrecognized character
    std::string unrec(1, op);
    if (diagnostics_) {
        diagnostics_->reportLexicalError(singleSpan, "Unrecognized character '" + unrec + "'.");
    }
    hasInternalErrors_ = true;
    return Token(TokenType::Unknown, unrec, singleSpan);
}

Token Lexer::nextToken() {
    if (!pendingTokens_.empty()) {
        Token tok = pendingTokens_.front();
        pendingTokens_.erase(pendingTokens_.begin());
        return tok;
    }
    return scanToken();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.push_back(tok);
        if (tok.is(TokenType::EndOfFile)) {
            break;
        }
    }
    return tokens;
}

} // namespace gamescript
