#include "gamescript/ir/IRBuilder.hpp"
#include <iostream>

namespace gamescript::ir {

IRBuilder::IRBuilder() {
    module_ = std::make_unique<IRModule>("gamescript_module");
}

Operand IRBuilder::nextRegister() {
    return Operand::makeReg(registerCounter_++);
}

std::string IRBuilder::nextLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

BasicBlock* IRBuilder::createBlock(const std::string& name) {
    auto block = std::make_unique<BasicBlock>(name);
    BasicBlock* ptr = block.get();
    if (currentFunction_) {
        currentFunction_->blocks.push_back(std::move(block));
    }
    return ptr;
}

void IRBuilder::setInsertBlock(BasicBlock* block) {
    currentBlock_ = block;
}

void IRBuilder::emit(Instruction inst) {
    if (currentBlock_) {
        currentBlock_->addInstruction(std::move(inst));
    }
}

std::unique_ptr<IRModule> IRBuilder::build(Program& program) {
    registerCounter_ = 0;
    labelCounter_ = 0;
    module_ = std::make_unique<IRModule>("gamescript_module");

    // Create main entry function
    auto mainFunc = std::make_unique<IRFunction>();
    mainFunc->name = "main";
    currentFunction_ = mainFunc.get();
    
    BasicBlock* entryBlock = createBlock("entry");
    setInsertBlock(entryBlock);

    program.accept(*this);

    // Emit final return in main
    emit(Instruction(OpCode::Return));

    module_->addFunction(std::move(mainFunc));
    return std::move(module_);
}

void IRBuilder::visit(Program& node) {
    for (const auto& stmt : node.getStatements()) {
        if (stmt) stmt->accept(*this);
    }
}

void IRBuilder::visit(VarDeclStmt& node) {
    Operand valOp = Operand::makeInt(0);
    if (node.getInitializer()) {
        node.getInitializer()->accept(*this);
        valOp = lastOperand_;
    }

    emit(Instruction(OpCode::Alloca, Operand::makeVar(node.getVarName())));
    emit(Instruction(OpCode::Store, Operand::makeVar(node.getVarName()), valOp));
}

void IRBuilder::visit(MoveStmt& node) {
    Operand distOp = Operand::makeInt(1);
    if (node.getDistance()) {
        node.getDistance()->accept(*this);
        distOp = lastOperand_;
    }

    int dirCode = 0;
    switch (node.getDirection()) {
        case MoveDir::Forward:  dirCode = 0; break;
        case MoveDir::Backward: dirCode = 1; break;
        case MoveDir::Left:     dirCode = 2; break;
        case MoveDir::Right:    dirCode = 3; break;
    }

    emit(Instruction(OpCode::GameMove, Operand::makeInt(dirCode), distOp, Operand::makeNone(), "player move " + moveDirToString(node.getDirection())));
}

void IRBuilder::visit(TurnStmt& node) {
    Operand degOp = Operand::makeInt(90);
    if (node.getDegrees()) {
        node.getDegrees()->accept(*this);
        degOp = lastOperand_;
    }

    int dirCode = (node.getDirection() == TurnDir::Left) ? 0 : 1;
    emit(Instruction(OpCode::GameTurn, Operand::makeInt(dirCode), degOp, Operand::makeNone(), "player turn " + turnDirToString(node.getDirection())));
}

void IRBuilder::visit(ActionStmt& node) {
    int actionCode = 0;
    switch (node.getAction()) {
        case ActionKind::Attack:   actionCode = 0; break;
        case ActionKind::Defend:   actionCode = 1; break;
        case ActionKind::Jump:     actionCode = 2; break;
        case ActionKind::Interact: actionCode = 3; break;
        case ActionKind::Retreat:  actionCode = 4; break;
    }

    emit(Instruction(OpCode::GameAction, Operand::makeInt(actionCode), Operand::makeNone(), Operand::makeNone(), "player " + actionKindToString(node.getAction())));
}

void IRBuilder::visit(IfStmt& node) {
    Operand condOp = Operand::makeBool(true);
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        condOp = lastOperand_;
    }

    std::string thenLabel = nextLabel("if_then_");
    std::string elseLabel = nextLabel("if_else_");
    std::string mergeLabel = nextLabel("if_merge_");

    bool hasElse = !node.getElseBranch().empty();
    std::string falseTarget = hasElse ? elseLabel : mergeLabel;

    emit(Instruction(OpCode::BranchCond, condOp, Operand::makeLabel(thenLabel), Operand::makeLabel(falseTarget)));

    // Then Block
    BasicBlock* thenBlock = createBlock(thenLabel);
    setInsertBlock(thenBlock);
    for (const auto& s : node.getThenBranch()) {
        if (s) s->accept(*this);
    }
    emit(Instruction(OpCode::Branch, Operand::makeLabel(mergeLabel)));

    // Else Block
    if (hasElse) {
        BasicBlock* elseBlock = createBlock(elseLabel);
        setInsertBlock(elseBlock);
        for (const auto& s : node.getElseBranch()) {
            if (s) s->accept(*this);
        }
        emit(Instruction(OpCode::Branch, Operand::makeLabel(mergeLabel)));
    }

    // Merge Block
    BasicBlock* mergeBlock = createBlock(mergeLabel);
    setInsertBlock(mergeBlock);
}

void IRBuilder::visit(RepeatStmt& node) {
    Operand countOp = Operand::makeInt(1);
    if (node.getCountExpr()) {
        node.getCountExpr()->accept(*this);
        countOp = lastOperand_;
    }

    std::string loopVar = "__loop_ctr_" + std::to_string(labelCounter_++);
    emit(Instruction(OpCode::Alloca, Operand::makeVar(loopVar)));
    emit(Instruction(OpCode::Store, Operand::makeVar(loopVar), countOp, Operand::makeNone(), "loop counter init"));

    std::string headLabel = nextLabel("repeat_head_");
    std::string bodyLabel = nextLabel("repeat_body_");
    std::string exitLabel = nextLabel("repeat_exit_");

    emit(Instruction(OpCode::Branch, Operand::makeLabel(headLabel)));

    // Loop Header
    BasicBlock* headBlock = createBlock(headLabel);
    setInsertBlock(headBlock);
    Operand curVal = nextRegister();
    emit(Instruction(OpCode::Load, curVal, Operand::makeVar(loopVar)));
    Operand cmpReg = nextRegister();
    emit(Instruction(OpCode::CmpGt, cmpReg, curVal, Operand::makeInt(0)));
    emit(Instruction(OpCode::BranchCond, cmpReg, Operand::makeLabel(bodyLabel), Operand::makeLabel(exitLabel)));

    // Loop Body
    BasicBlock* bodyBlock = createBlock(bodyLabel);
    setInsertBlock(bodyBlock);
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    // Decrement counter: load, sub 1, store
    Operand valToDec = nextRegister();
    emit(Instruction(OpCode::Load, valToDec, Operand::makeVar(loopVar)));
    Operand decVal = nextRegister();
    emit(Instruction(OpCode::Sub, decVal, valToDec, Operand::makeInt(1)));
    emit(Instruction(OpCode::Store, Operand::makeVar(loopVar), decVal));
    emit(Instruction(OpCode::Branch, Operand::makeLabel(headLabel)));

    // Exit Block
    BasicBlock* exitBlock = createBlock(exitLabel);
    setInsertBlock(exitBlock);
}

void IRBuilder::visit(WhileStmt& node) {
    std::string headLabel = nextLabel("while_head_");
    std::string bodyLabel = nextLabel("while_body_");
    std::string exitLabel = nextLabel("while_exit_");

    emit(Instruction(OpCode::Branch, Operand::makeLabel(headLabel)));

    // Loop Header: evaluate condition
    BasicBlock* headBlock = createBlock(headLabel);
    setInsertBlock(headBlock);
    Operand condOp = Operand::makeBool(true);
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        condOp = lastOperand_;
    }
    emit(Instruction(OpCode::BranchCond, condOp, Operand::makeLabel(bodyLabel), Operand::makeLabel(exitLabel)));

    // Loop Body
    BasicBlock* bodyBlock = createBlock(bodyLabel);
    setInsertBlock(bodyBlock);
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    emit(Instruction(OpCode::Branch, Operand::makeLabel(headLabel)));

    // Exit Block
    BasicBlock* exitBlock = createBlock(exitLabel);
    setInsertBlock(exitBlock);
}

void IRBuilder::visit(FunctionDeclStmt& node) {
    IRFunction* prevFunc = currentFunction_;
    BasicBlock* prevBlock = currentBlock_;

    auto func = std::make_unique<IRFunction>();
    func->name = node.getName();
    func->params = node.getParams();
    currentFunction_ = func.get();

    BasicBlock* entry = createBlock(node.getName() + "_entry");
    setInsertBlock(entry);

    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    emit(Instruction(OpCode::Return));

    module_->addFunction(std::move(func));

    currentFunction_ = prevFunc;
    currentBlock_ = prevBlock;
}

void IRBuilder::visit(FunctionCallStmt& node) {
    emit(Instruction(OpCode::Call, Operand::makeLabel(node.getName())));
}

void IRBuilder::visit(WhenStmt& node) {
    // Lower when block as a triggered conditional check
    Operand condOp = Operand::makeBool(true);
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        condOp = lastOperand_;
    }

    std::string whenBodyLabel = nextLabel("when_body_");
    std::string whenExitLabel = nextLabel("when_exit_");

    emit(Instruction(OpCode::BranchCond, condOp, Operand::makeLabel(whenBodyLabel), Operand::makeLabel(whenExitLabel)));

    BasicBlock* bodyBlock = createBlock(whenBodyLabel);
    setInsertBlock(bodyBlock);
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    emit(Instruction(OpCode::Branch, Operand::makeLabel(whenExitLabel)));

    BasicBlock* exitBlock = createBlock(whenExitLabel);
    setInsertBlock(exitBlock);
}

void IRBuilder::visit(ExprStmt& node) {
    if (node.getExpr()) node.getExpr()->accept(*this);
}

void IRBuilder::visit(IntegerLiteralExpr& node) {
    lastOperand_ = Operand::makeInt(node.getValue());
}

void IRBuilder::visit(StringLiteralExpr&) {
    lastOperand_ = Operand::makeNone();
}

void IRBuilder::visit(BoolLiteralExpr& node) {
    lastOperand_ = Operand::makeBool(node.getValue());
}

void IRBuilder::visit(IdentifierExpr& node) {
    Operand reg = nextRegister();
    emit(Instruction(OpCode::Load, reg, Operand::makeVar(node.getName())));
    lastOperand_ = reg;
}

void IRBuilder::visit(BinaryExpr& node) {
    Operand leftOp = Operand::makeInt(0);
    Operand rightOp = Operand::makeInt(0);

    if (node.getLeft()) {
        node.getLeft()->accept(*this);
        leftOp = lastOperand_;
    }
    if (node.getRight()) {
        node.getRight()->accept(*this);
        rightOp = lastOperand_;
    }

    Operand destReg = nextRegister();
    OpCode op = OpCode::Add;
    switch (node.getOp()) {
        case BinaryOp::Add:          op = OpCode::Add; break;
        case BinaryOp::Subtract:     op = OpCode::Sub; break;
        case BinaryOp::Multiply:     op = OpCode::Mul; break;
        case BinaryOp::Divide:       op = OpCode::Div; break;
        case BinaryOp::Modulo:       op = OpCode::Mod; break;
        case BinaryOp::Equal:        op = OpCode::CmpEq; break;
        case BinaryOp::NotEqual:     op = OpCode::CmpNe; break;
        case BinaryOp::Less:         op = OpCode::CmpLt; break;
        case BinaryOp::LessEqual:    op = OpCode::CmpLe; break;
        case BinaryOp::Greater:      op = OpCode::CmpGt; break;
        case BinaryOp::GreaterEqual: op = OpCode::CmpGe; break;
        case BinaryOp::And:          op = OpCode::And; break;
        case BinaryOp::Or:           op = OpCode::Or; break;
    }

    emit(Instruction(op, destReg, leftOp, rightOp));
    lastOperand_ = destReg;
}

void IRBuilder::visit(UnaryExpr& node) {
    Operand op = Operand::makeInt(0);
    if (node.getOperand()) {
        node.getOperand()->accept(*this);
        op = lastOperand_;
    }

    Operand destReg = nextRegister();
    if (node.getOp() == UnaryOp::Not) {
        emit(Instruction(OpCode::Not, destReg, op));
    } else if (node.getOp() == UnaryOp::Negate) {
        emit(Instruction(OpCode::Sub, destReg, Operand::makeInt(0), op));
    } else {
        destReg = op;
    }
    lastOperand_ = destReg;
}

void IRBuilder::visit(SensoryConditionExpr& node) {
    Operand destReg = nextRegister();
    int sensorId = 0;
    switch (node.getSensoryType()) {
        case SensoryType::EnemyNearby:     sensorId = 0; break;
        case SensoryType::HealthLow:       sensorId = 1; break;
        case SensoryType::ObstacleAhead:   sensorId = 2; break;
        case SensoryType::DistanceToEnemy: sensorId = 3; break;
        case SensoryType::Health:          sensorId = 4; break;
    }

    emit(Instruction(OpCode::GameSensor, destReg, Operand::makeInt(sensorId), Operand::makeNone(), sensoryTypeToString(node.getSensoryType())));
    lastOperand_ = destReg;
}

} // namespace gamescript::ir
