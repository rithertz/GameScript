#pragma once

#include "gamescript/ast/ASTVisitor.hpp"
#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include "gamescript/ast/Statements.hpp"
#include "gamescript/ir/IRModule.hpp"
#include "gamescript/ir/Instruction.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace gamescript::ir {

class IRBuilder : public ASTVisitor {
public:
    IRBuilder();

    std::unique_ptr<IRModule> build(Program& program);

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
    Operand nextRegister();
    std::string nextLabel(const std::string& prefix = "L");
    BasicBlock* createBlock(const std::string& name);
    void setInsertBlock(BasicBlock* block);
    void emit(Instruction inst);

    std::unique_ptr<IRModule> module_;
    IRFunction* currentFunction_ = nullptr;
    BasicBlock* currentBlock_ = nullptr;

    int registerCounter_ = 0;
    int labelCounter_ = 0;
    Operand lastOperand_ = Operand::makeNone();
};

} // namespace gamescript::ir
