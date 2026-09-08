#pragma once

#include <cstdint>
#include "gamescript/ast/ASTNode.hpp"
#include <string>
#include <memory>

namespace gamescript {

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

std::string binaryOpToString(BinaryOp op);

enum class UnaryOp {
    Plus,
    Negate,
    Not
};

std::string unaryOpToString(UnaryOp op);

enum class SensoryType {
    EnemyNearby,
    HealthLow,
    ObstacleAhead,
    DistanceToEnemy,
    Health
};

std::string sensoryTypeToString(SensoryType type);

inline std::ostream& operator<<(std::ostream& os, BinaryOp op) { return os << binaryOpToString(op); }
inline std::ostream& operator<<(std::ostream& os, UnaryOp op) { return os << unaryOpToString(op); }
inline std::ostream& operator<<(std::ostream& os, SensoryType type) { return os << sensoryTypeToString(type); }

class Expr : public ASTNode {
public:
    explicit Expr(ASTNodeType type, SourceSpan span = SourceSpan())
        : ASTNode(type, span) {}
};

using ExprPtr = std::unique_ptr<Expr>;

class IntegerLiteralExpr : public Expr {
public:
    IntegerLiteralExpr(int64_t value, SourceSpan span)
        : Expr(ASTNodeType::IntegerLiteralExpr, span), value_(value) {}

    int64_t getValue() const { return value_; }
    void setValue(int64_t val) { value_ = val; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    int64_t value_;
};

class StringLiteralExpr : public Expr {
public:
    StringLiteralExpr(std::string value, SourceSpan span)
        : Expr(ASTNodeType::StringLiteralExpr, span), value_(std::move(value)) {}

    const std::string& getValue() const { return value_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string value_;
};

class BoolLiteralExpr : public Expr {
public:
    BoolLiteralExpr(bool value, SourceSpan span)
        : Expr(ASTNodeType::BoolLiteralExpr, span), value_(value) {}

    bool getValue() const { return value_; }
    void setValue(bool val) { value_ = val; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    bool value_;
};

class IdentifierExpr : public Expr {
public:
    IdentifierExpr(std::string name, SourceSpan span)
        : Expr(ASTNodeType::IdentifierExpr, span), name_(std::move(name)) {}

    const std::string& getName() const { return name_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string name_;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(BinaryOp op, ExprPtr left, ExprPtr right, SourceSpan span)
        : Expr(ASTNodeType::BinaryExpr, span), op_(op), left_(std::move(left)), right_(std::move(right)) {}

    BinaryOp getOp() const { return op_; }
    Expr* getLeft() const { return left_.get(); }
    Expr* getRight() const { return right_.get(); }
    ExprPtr& getLeftPtr() { return left_; }
    ExprPtr& getRightPtr() { return right_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    BinaryOp op_;
    ExprPtr left_;
    ExprPtr right_;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(UnaryOp op, ExprPtr operand, SourceSpan span)
        : Expr(ASTNodeType::UnaryExpr, span), op_(op), operand_(std::move(operand)) {}

    UnaryOp getOp() const { return op_; }
    Expr* getOperand() const { return operand_.get(); }
    ExprPtr& getOperandPtr() { return operand_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    UnaryOp op_;
    ExprPtr operand_;
};

class SensoryConditionExpr : public Expr {
public:
    SensoryConditionExpr(SensoryType sensoryType, SourceSpan span)
        : Expr(ASTNodeType::SensoryConditionExpr, span), sensoryType_(sensoryType) {}

    SensoryType getSensoryType() const { return sensoryType_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    SensoryType sensoryType_;
};

} // namespace gamescript
