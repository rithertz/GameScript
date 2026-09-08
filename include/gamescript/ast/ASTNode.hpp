#pragma once

#include "gamescript/common/SourceLocation.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gamescript {

// Forward declarations
class ASTVisitor;

enum class ASTNodeType {
    Program,
    // Statements
    VarDeclStmt,
    MoveStmt,
    TurnStmt,
    ActionStmt,
    IfStmt,
    RepeatStmt,
    WhileStmt,
    FunctionDeclStmt,
    FunctionCallStmt,
    WhenStmt,
    ExprStmt,
    BlockStmt,

    // Expressions
    IntegerLiteralExpr,
    StringLiteralExpr,
    BoolLiteralExpr,
    IdentifierExpr,
    BinaryExpr,
    UnaryExpr,
    SensoryConditionExpr
};

class ASTNode {
public:
    explicit ASTNode(ASTNodeType nodeType, SourceSpan span = SourceSpan())
        : nodeType_(nodeType), span_(span) {}
    virtual ~ASTNode() = default;

    ASTNodeType getNodeType() const { return nodeType_; }
    const SourceSpan& getSpan() const { return span_; }
    void setSpan(SourceSpan span) { span_ = span; }

    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::string toString() const = 0;

private:
    ASTNodeType nodeType_;
    SourceSpan span_;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

} // namespace gamescript
