#include "gamescript/lexer/Token.hpp"
#include <sstream>

namespace gamescript {

std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile:      return "EndOfFile";
        case TokenType::Newline:        return "Newline";
        case TokenType::Indent:         return "Indent";
        case TokenType::Dedent:         return "Dedent";
        case TokenType::Set:            return "Set";
        case TokenType::Player:         return "Player";
        case TokenType::Move:           return "Move";
        case TokenType::Turn:           return "Turn";
        case TokenType::Attack:         return "Attack";
        case TokenType::Defend:         return "Defend";
        case TokenType::Jump:           return "Jump";
        case TokenType::Interact:       return "Interact";
        case TokenType::Retreat:        return "Retreat";
        case TokenType::If:             return "If";
        case TokenType::Else:           return "Else";
        case TokenType::Repeat:         return "Repeat";
        case TokenType::While:          return "While";
        case TokenType::Function:       return "Function";
        case TokenType::Call:           return "Call";
        case TokenType::When:           return "When";
        case TokenType::Not:            return "Not";
        case TokenType::And:            return "And";
        case TokenType::Or:             return "Or";
        case TokenType::True:           return "True";
        case TokenType::False:          return "False";
        case TokenType::Forward:        return "Forward";
        case TokenType::Backward:       return "Backward";
        case TokenType::Left:           return "Left";
        case TokenType::Right:          return "Right";
        case TokenType::Enemy:          return "Enemy";
        case TokenType::Nearby:         return "Nearby";
        case TokenType::Health:         return "Health";
        case TokenType::Low:            return "Low";
        case TokenType::Obstacle:       return "Obstacle";
        case TokenType::Ahead:          return "Ahead";
        case TokenType::Distance:       return "Distance";
        case TokenType::To:             return "To";
        case TokenType::Identifier:     return "Identifier";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::StringLiteral:  return "StringLiteral";
        case TokenType::Colon:          return "Colon";
        case TokenType::Comma:          return "Comma";
        case TokenType::Equal:          return "Equal";
        case TokenType::EqualEqual:     return "EqualEqual";
        case TokenType::BangEqual:      return "BangEqual";
        case TokenType::Less:           return "Less";
        case TokenType::LessEqual:      return "LessEqual";
        case TokenType::Greater:        return "Greater";
        case TokenType::GreaterEqual:   return "GreaterEqual";
        case TokenType::Plus:           return "Plus";
        case TokenType::Minus:          return "Minus";
        case TokenType::Star:           return "Star";
        case TokenType::Slash:          return "Slash";
        case TokenType::Percent:        return "Percent";
        case TokenType::LParen:         return "LParen";
        case TokenType::RParen:         return "RParen";
        case TokenType::Unknown:        return "Unknown";
    }
    return "Unknown";
}

bool Token::isKeyword() const {
    switch (type) {
        case TokenType::Set:
        case TokenType::Player:
        case TokenType::Move:
        case TokenType::Turn:
        case TokenType::Attack:
        case TokenType::Defend:
        case TokenType::Jump:
        case TokenType::Interact:
        case TokenType::Retreat:
        case TokenType::If:
        case TokenType::Else:
        case TokenType::Repeat:
        case TokenType::While:
        case TokenType::Function:
        case TokenType::Call:
        case TokenType::When:
        case TokenType::Not:
        case TokenType::And:
        case TokenType::Or:
        case TokenType::True:
        case TokenType::False:
        case TokenType::Forward:
        case TokenType::Backward:
        case TokenType::Left:
        case TokenType::Right:
        case TokenType::Enemy:
        case TokenType::Nearby:
        case TokenType::Health:
        case TokenType::Low:
        case TokenType::Obstacle:
        case TokenType::Ahead:
        case TokenType::Distance:
        case TokenType::To:
            return true;
        default:
            return false;
    }
}

bool Token::isActionKeyword() const {
    switch (type) {
        case TokenType::Attack:
        case TokenType::Defend:
        case TokenType::Jump:
        case TokenType::Interact:
        case TokenType::Retreat:
            return true;
        default:
            return false;
    }
}

bool Token::isDirectionKeyword() const {
    switch (type) {
        case TokenType::Forward:
        case TokenType::Backward:
        case TokenType::Left:
        case TokenType::Right:
            return true;
        default:
            return false;
    }
}

bool Token::isRelationalOp() const {
    switch (type) {
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
            return true;
        default:
            return false;
    }
}

bool Token::isArithmeticOp() const {
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Percent:
            return true;
        default:
            return false;
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << tokenTypeToString(type) << ", \"" << lexeme << "\", " << span.toString();
    if (type == TokenType::IntegerLiteral) {
        oss << ", val=" << intValue;
    }
    oss << ")";
    return oss.str();
}

} // namespace gamescript
