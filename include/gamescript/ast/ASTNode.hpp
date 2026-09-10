#pragma once
//==============================================================================
// ASTNode.hpp
//
// Defines the base class for all Abstract Syntax Tree (AST) nodes used by the
// GameScript compiler front-end.
//==============================================================================

#include "gamescript/common/SourceLocation.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gamescript {

// Forward declarations
class ASTVisitor;

// Identifies the concrete kind of an AST node.
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

// Abstract base class for all AST nodes. Stores the node's kind and source
// span; subclasses add their own data such as children, literals, or operators.

class ASTNode {
public:
    explicit ASTNode(ASTNodeType nodeType, SourceSpan span = SourceSpan())
        : nodeType_(nodeType), span_(span) {}
     // Virtual so deleting through an ASTNode pointer destroys the concrete subclass.
    virtual ~ASTNode() = default;

    ASTNodeType getNodeType() const { return nodeType_; }
    const SourceSpan& getSpan() const { return span_; }
    void setSpan(SourceSpan span) { span_ = span; }
    // Visitor dispatch — each subclass forwards itself to the appropriate visitor method.
    virtual void accept(ASTVisitor& visitor) = 0;
    // Returns a human-readable representation of this node and its children for debugging.
    virtual std::string toString() const = 0;

private:
    ASTNodeType nodeType_;
    SourceSpan span_;
};

// Owning pointer to an AST node; unique_ptr provides single-owner memory management.
using ASTNodePtr = std::unique_ptr<ASTNode>;

} // namespace gamescript
