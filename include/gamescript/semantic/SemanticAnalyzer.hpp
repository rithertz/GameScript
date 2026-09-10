#pragma once

//==============================================================================
// SemanticAnalyzer.hpp
//
// Defines the semantic analyzer responsible for validating the AST and
// performing type checking and symbol resolution.
//==============================================================================

#include "gamescript/ast/ASTVisitor.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include "gamescript/ast/Statements.hpp"
#include "gamescript/semantic/SymbolTable.hpp"
#include "gamescript/common/DiagnosticEngine.hpp"
#include <unordered_map>

namespace gamescript {

// Performs semantic analysis on the GameScript AST.
class SemanticAnalyzer : public ASTVisitor {
public:
    explicit SemanticAnalyzer(DiagnosticEngine* diagnostics = nullptr);

    // Analyzes the given program and reports semantic errors.
    bool analyze(Program& program);
    bool hasErrors() const { return diagnostics_ ? diagnostics_->hasErrors() : hasInternalErrors_; }

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

    // Returns the inferred data type of an expression.
    DataType getExprType(Expr* expr) const;

private:
    // Stores the inferred data type for an expression.
    void setExprType(Expr* expr, DataType type);

    DiagnosticEngine* diagnostics_ = nullptr;
    SymbolTable symbolTable_;
    bool hasInternalErrors_ = false;

    // Type inference cache
    std::unordered_map<Expr*, DataType> exprTypes_;
    DataType lastEvaluatedType_ = DataType::Unknown;
};

} // namespace gamescript