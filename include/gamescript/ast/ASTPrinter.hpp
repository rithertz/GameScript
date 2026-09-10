#pragma once
//==============================================================================
// ASTPrinter.hpp
//
// Defines the ASTPrinter visitor used to generate a human-readable
// representation of the GameScript Abstract Syntax Tree.
//==============================================================================

#include "gamescript/ast/ASTVisitor.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include "gamescript/ast/Statements.hpp"
#include <string>
#include <sstream>

namespace gamescript {
    
// Prints AST nodes in a structured, indented format for debugging and inspection.
class ASTPrinter : public ASTVisitor {
public:
    // Generates a string representation of the given AST node.
    std::string print(ASTNode& node);

    // Visitor methods for statement nodes.
    void visit(Program& node) override;
    void visit(VarDeclStmt& node) override;
    void visit(MoveStmt& node) override;
    void visit(TurnStmt& node) override;
    void visit(ActionStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(RepeatStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(FunctionDeclStmt& node) override;
    void visit(FunctionCallStmt& node) override;
    void visit(WhenStmt& node) override;
    void visit(ExprStmt& node) override;

    // Visitor methods for expression nodes.
    void visit(IntegerLiteralExpr& node) override;
    void visit(StringLiteralExpr& node) override;
    void visit(BoolLiteralExpr& node) override;
    void visit(IdentifierExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(SensoryConditionExpr& node) override;

private:
    // Increases the indentation level for nested AST nodes.
    void indent();

    // Decreases the indentation level after processing nested AST nodes.
    void dedent();

    // Returns the whitespace string corresponding to the current indentation level.
    std::string getIndentString() const;

    int indentLevel_ = 0;
    std::ostringstream oss_;
};

} // namespace gamescript
