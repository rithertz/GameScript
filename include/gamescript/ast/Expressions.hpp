#pragma once

//==============================================================================
// Expressions.hpp
//
// Defines the expression nodes and operators used to represent expressions
// in the GameScript Abstract Syntax Tree.
//==============================================================================

#include <cstdint>
#include "gamescript/ast/ASTNode.hpp"
#include <string>
#include <memory>

namespace gamescript {

// Represents the supported binary operators in GameScript.
enum class BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or
};

// Converts a binary operator to its string representation.
std::string binaryOpToString(BinaryOp op);

// Represents the supported unary operators in GameScript.
enum class UnaryOp {
    Plus,
    Negate,
    Not
};

// Converts a unary operator to its string representation.
std::string unaryOpToString(UnaryOp op);

// Represents sensory conditions and values available to GameScript expressions.
enum class SensoryType {
    EnemyNearby,
    HealthLow,
    ObstacleAhead,
    DistanceToEnemy,
    Health
};

// Converts a sensory type to its string representation.
std::string sensoryTypeToString(SensoryType type);

// Stream operators for displaying operators and sensory types.
inline std::ostream& operator<<(std::ostream& os, BinaryOp op) {
    return os << binaryOpToString(op);
}

inline std::ostream& operator<<(std::ostream& os, UnaryOp op) {
    return os << unaryOpToString(op);
}

inline std::ostream& operator<<(std::ostream& os, SensoryType type) {
    return os << sensoryTypeToString(type);
}

// Base class for all expression nodes in the AST.
class Expr : public ASTNode {
public:
    explicit Expr(ASTNodeType type, SourceSpan span = SourceSpan())
        : ASTNode(type, span) {}
};

// Owning pointer to an expression node.
using ExprPtr = std::unique_ptr<Expr>;

// Represents an integer literal expression.
class IntegerLiteralExpr : public Expr {
public:
    IntegerLiteralExpr(int64_t value, SourceSpan span)
        : Expr(ASTNodeType::IntegerLiteralExpr, span), value_(value) {}

    int64_t getValue() const { return value_; }
    void setValue(int64_t val) { value_ = val; }

    // Visitor dispatch for integer literal expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    int64_t value_;
};

// Represents a string literal expression.
class StringLiteralExpr : public Expr {
public:
    StringLiteralExpr(std::string value, SourceSpan span)
        : Expr(ASTNodeType::StringLiteralExpr, span), value_(std::move(value)) {}

    const std::string& getValue() const { return value_; }

    // Visitor dispatch for string literal expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    std::string value_;
};

// Represents a boolean literal expression.
class BoolLiteralExpr : public Expr {
public:
    BoolLiteralExpr(bool value, SourceSpan span)
        : Expr(ASTNodeType::BoolLiteralExpr, span), value_(value) {}

    bool getValue() const { return value_; }
    void setValue(bool val) { value_ = val; }

    // Visitor dispatch for boolean literal expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    bool value_;
};

// Represents an identifier expression, such as a variable or function name.
class IdentifierExpr : public Expr {
public:
    IdentifierExpr(std::string name, SourceSpan span)
        : Expr(ASTNodeType::IdentifierExpr, span), name_(std::move(name)) {}

    const std::string& getName() const { return name_; }

    // Visitor dispatch for identifier expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    std::string name_;
};

// Represents a binary expression with a left operand, operator, and right operand.
class BinaryExpr : public Expr {
public:
    BinaryExpr(BinaryOp op, ExprPtr left, ExprPtr right, SourceSpan span)
        : Expr(ASTNodeType::BinaryExpr, span),
          op_(op),
          left_(std::move(left)),
          right_(std::move(right)) {}

    BinaryOp getOp() const { return op_; }
    Expr* getLeft() const { return left_.get(); }
    Expr* getRight() const { return right_.get(); }
    ExprPtr& getLeftPtr() { return left_; }
    ExprPtr& getRightPtr() { return right_; }

    // Visitor dispatch for binary expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    BinaryOp op_;
    ExprPtr left_;
    ExprPtr right_;
};

// Represents a unary expression with an operator and operand.
class UnaryExpr : public Expr {
public:
    UnaryExpr(UnaryOp op, ExprPtr operand, SourceSpan span)
        : Expr(ASTNodeType::UnaryExpr, span),
          op_(op),
          operand_(std::move(operand)) {}

    UnaryOp getOp() const { return op_; }
    Expr* getOperand() const { return operand_.get(); }
    ExprPtr& getOperandPtr() { return operand_; }

    // Visitor dispatch for unary expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    UnaryOp op_;
    ExprPtr operand_;
};

// Represents an expression that evaluates a game-related sensory condition.
class SensoryConditionExpr : public Expr {
public:
    SensoryConditionExpr(SensoryType sensoryType, SourceSpan span)
        : Expr(ASTNodeType::SensoryConditionExpr, span),
          sensoryType_(sensoryType) {}

    SensoryType getSensoryType() const { return sensoryType_; }

    // Visitor dispatch for sensory condition expressions.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the expression.
    std::string toString() const override;

private:
    SensoryType sensoryType_;
};

} // namespace gamescript