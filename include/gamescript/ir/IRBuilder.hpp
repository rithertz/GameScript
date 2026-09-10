#pragma once

//==============================================================================
// IRBuilder.hpp
//
// Defines the IRBuilder visitor used to translate the GameScript Abstract
// Syntax Tree (AST) into the intermediate representation (IR).
//==============================================================================

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

// Builds the GameScript intermediate representation by visiting AST nodes.
class IRBuilder : public ASTVisitor {
public:
    IRBuilder();

    // Builds an IR module from the given GameScript program.
    std::unique_ptr<IRModule> build(Program& program);

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
    // Generates the next virtual register used by the IR.
    Operand nextRegister();

    // Generates the next unique label using the specified prefix.
    std::string nextLabel(const std::string& prefix = "L");

    // Creates and returns a new basic block.
    BasicBlock* createBlock(const std::string& name);

    // Sets the current basic block where new instructions are inserted.
    void setInsertBlock(BasicBlock* block);

    // Adds an instruction to the current basic block.
    void emit(Instruction inst);

    std::unique_ptr<IRModule> module_;
    IRFunction* currentFunction_ = nullptr;
    BasicBlock* currentBlock_ = nullptr;

    int registerCounter_ = 0;
    int labelCounter_ = 0;
    Operand lastOperand_ = Operand::makeNone();
};

} // namespace gamescript::ir