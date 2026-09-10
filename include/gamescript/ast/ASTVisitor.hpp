#pragma once

//==============================================================================
// ASTVisitor.hpp
//
// Defines the visitor interface used to traverse and process nodes in the
// GameScript Abstract Syntax Tree.
//==============================================================================

namespace gamescript {

// Forward declarations of AST nodes
class Program;
class VarDeclStmt;
class MoveStmt;
class TurnStmt;
class ActionStmt;
class IfStmt;
class RepeatStmt;
class WhileStmt;
class FunctionDeclStmt;
class FunctionCallStmt;
class WhenStmt;
class ExprStmt;

class IntegerLiteralExpr;
class StringLiteralExpr;
class BoolLiteralExpr;
class IdentifierExpr;
class BinaryExpr;
class UnaryExpr;
class SensoryConditionExpr;

// Visitor interface for performing operations on different types of AST nodes.
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Visitor methods for statement nodes.
    virtual void visit(Program& node) = 0;
    virtual void visit(VarDeclStmt& node) = 0;
    virtual void visit(MoveStmt& node) = 0;
    virtual void visit(TurnStmt& node) = 0;
    virtual void visit(ActionStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(RepeatStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(FunctionDeclStmt& node) = 0;
    virtual void visit(FunctionCallStmt& node) = 0;
    virtual void visit(WhenStmt& node) = 0;
    virtual void visit(ExprStmt& node) = 0;
    
    // Visitor methods for expression nodes.
    virtual void visit(IntegerLiteralExpr& node) = 0;
    virtual void visit(StringLiteralExpr& node) = 0;
    virtual void visit(BoolLiteralExpr& node) = 0;
    virtual void visit(IdentifierExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(SensoryConditionExpr& node) = 0;
};

} // namespace gamescript
