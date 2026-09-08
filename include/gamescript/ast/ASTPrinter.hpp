#pragma once

#include "gamescript/ast/ASTVisitor.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include "gamescript/ast/Statements.hpp"
#include <string>
#include <sstream>

namespace gamescript {

class ASTPrinter : public ASTVisitor {
public:
    std::string print(ASTNode& node);

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

    void visit(IntegerLiteralExpr& node) override;
    void visit(StringLiteralExpr& node) override;
    void visit(BoolLiteralExpr& node) override;
    void visit(IdentifierExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(SensoryConditionExpr& node) override;

private:
    void indent();
    void dedent();
    std::string getIndentString() const;

    int indentLevel_ = 0;
    std::ostringstream oss_;
};

} // namespace gamescript
