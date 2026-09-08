#include "gamescript/ast/ASTPrinter.hpp"

namespace gamescript {

// Helper string conversions
std::string binaryOpToString(BinaryOp op) {
    switch (op) {
        case BinaryOp::Add:          return "+";
        case BinaryOp::Subtract:     return "-";
        case BinaryOp::Multiply:     return "*";
        case BinaryOp::Divide:       return "/";
        case BinaryOp::Modulo:       return "%";
        case BinaryOp::Equal:        return "==";
        case BinaryOp::NotEqual:     return "!=";
        case BinaryOp::Less:         return "<";
        case BinaryOp::LessEqual:    return "<=";
        case BinaryOp::Greater:      return ">";
        case BinaryOp::GreaterEqual: return ">=";
        case BinaryOp::And:          return "and";
        case BinaryOp::Or:           return "or";
    }
    return "?";
}

std::string unaryOpToString(UnaryOp op) {
    switch (op) {
        case UnaryOp::Plus:   return "+";
        case UnaryOp::Negate: return "-";
        case UnaryOp::Not:    return "not ";
    }
    return "?";
}

std::string sensoryTypeToString(SensoryType type) {
    switch (type) {
        case SensoryType::EnemyNearby:     return "EnemyNearby";
        case SensoryType::HealthLow:       return "HealthLow";
        case SensoryType::ObstacleAhead:   return "ObstacleAhead";
        case SensoryType::DistanceToEnemy: return "DistanceToEnemy";
        case SensoryType::Health:          return "Health";
    }
    return "?";
}

std::string moveDirToString(MoveDir dir) {
    switch (dir) {
        case MoveDir::Forward:  return "FORWARD";
        case MoveDir::Backward: return "BACKWARD";
        case MoveDir::Left:     return "LEFT";
        case MoveDir::Right:    return "RIGHT";
    }
    return "?";
}

std::string turnDirToString(TurnDir dir) {
    switch (dir) {
        case TurnDir::Left:  return "LEFT";
        case TurnDir::Right: return "RIGHT";
    }
    return "?";
}

std::string actionKindToString(ActionKind kind) {
    switch (kind) {
        case ActionKind::Attack:   return "ATTACK";
        case ActionKind::Defend:   return "DEFEND";
        case ActionKind::Jump:     return "JUMP";
        case ActionKind::Interact: return "INTERACT";
        case ActionKind::Retreat:  return "RETREAT";
    }
    return "?";
}

// ASTNode accept / toString implementations
void IntegerLiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string IntegerLiteralExpr::toString() const { return std::to_string(value_); }

void StringLiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string StringLiteralExpr::toString() const { return "\"" + value_ + "\""; }

void BoolLiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string BoolLiteralExpr::toString() const { return value_ ? "true" : "false"; }

void IdentifierExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string IdentifierExpr::toString() const { return name_; }

void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string BinaryExpr::toString() const {
    return "(" + (left_ ? left_->toString() : "null") + " " + binaryOpToString(op_) + " " + (right_ ? right_->toString() : "null") + ")";
}

void UnaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string UnaryExpr::toString() const {
    return "(" + unaryOpToString(op_) + (operand_ ? operand_->toString() : "null") + ")";
}

void SensoryConditionExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string SensoryConditionExpr::toString() const { return sensoryTypeToString(sensoryType_); }

void VarDeclStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string VarDeclStmt::toString() const {
    return "set " + varName_ + " = " + (initializer_ ? initializer_->toString() : "null");
}

void MoveStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string MoveStmt::toString() const {
    return "player move " + moveDirToString(direction_) + " " + (distance_ ? distance_->toString() : "0");
}

void TurnStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string TurnStmt::toString() const {
    return "player turn " + turnDirToString(direction_) + " " + (degrees_ ? degrees_->toString() : "0");
}

void ActionStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string ActionStmt::toString() const {
    return "player " + actionKindToString(action_);
}

void IfStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string IfStmt::toString() const {
    return "if " + (condition_ ? condition_->toString() : "null");
}

void RepeatStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string RepeatStmt::toString() const {
    return "repeat " + (countExpr_ ? countExpr_->toString() : "0");
}

void WhileStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string WhileStmt::toString() const {
    return "while " + (condition_ ? condition_->toString() : "null");
}

void FunctionDeclStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string FunctionDeclStmt::toString() const {
    return "function " + name_;
}

void FunctionCallStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string FunctionCallStmt::toString() const {
    return "call " + name_;
}

void WhenStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string WhenStmt::toString() const {
    return "when " + (condition_ ? condition_->toString() : "null");
}

void ExprStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string ExprStmt::toString() const {
    return expr_ ? expr_->toString() : "null";
}

void Program::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::string Program::toString() const {
    return "Program(" + std::to_string(statements_.size()) + " stmts)";
}

// ASTPrinter implementation
void ASTPrinter::indent() { indentLevel_++; }
void ASTPrinter::dedent() { if (indentLevel_ > 0) indentLevel_--; }
std::string ASTPrinter::getIndentString() const {
    std::string s;
    for (int i = 0; i < indentLevel_; ++i) {
        s += " │  ";
    }
    return s;
}

std::string ASTPrinter::print(ASTNode& node) {
    oss_.str("");
    oss_.clear();
    indentLevel_ = 0;
    node.accept(*this);
    return oss_.str();
}

void ASTPrinter::visit(Program& node) {
    oss_ << "Program\n";
    indent();
    for (const auto& stmt : node.getStatements()) {
        if (stmt) stmt->accept(*this);
    }
    dedent();
}

void ASTPrinter::visit(VarDeclStmt& node) {
    oss_ << getIndentString() << "├── VarDecl: " << node.getVarName() << "\n";
    if (node.getInitializer()) {
        indent();
        oss_ << getIndentString() << "└── Value: ";
        node.getInitializer()->accept(*this);
        oss_ << "\n";
        dedent();
    }
}

void ASTPrinter::visit(MoveStmt& node) {
    oss_ << getIndentString() << "├── Move: " << moveDirToString(node.getDirection()) << "\n";
    if (node.getDistance()) {
        indent();
        oss_ << getIndentString() << "└── Distance: ";
        node.getDistance()->accept(*this);
        oss_ << "\n";
        dedent();
    }
}

void ASTPrinter::visit(TurnStmt& node) {
    oss_ << getIndentString() << "├── Turn: " << turnDirToString(node.getDirection()) << "\n";
    if (node.getDegrees()) {
        indent();
        oss_ << getIndentString() << "└── Degrees: ";
        node.getDegrees()->accept(*this);
        oss_ << "\n";
        dedent();
    }
}

void ASTPrinter::visit(ActionStmt& node) {
    oss_ << getIndentString() << "├── Action: " << actionKindToString(node.getAction()) << "\n";
}

void ASTPrinter::visit(IfStmt& node) {
    oss_ << getIndentString() << "├── If\n";
    indent();
    oss_ << getIndentString() << "├── Condition: ";
    if (node.getCondition()) node.getCondition()->accept(*this);
    oss_ << "\n";
    
    oss_ << getIndentString() << "├── Then:\n";
    indent();
    for (const auto& s : node.getThenBranch()) {
        if (s) s->accept(*this);
    }
    dedent();

    if (!node.getElseBranch().empty()) {
        oss_ << getIndentString() << "└── Else:\n";
        indent();
        for (const auto& s : node.getElseBranch()) {
            if (s) s->accept(*this);
        }
        dedent();
    }
    dedent();
}

void ASTPrinter::visit(RepeatStmt& node) {
    oss_ << getIndentString() << "├── Repeat\n";
    indent();
    oss_ << getIndentString() << "├── Count: ";
    if (node.getCountExpr()) node.getCountExpr()->accept(*this);
    oss_ << "\n";
    oss_ << getIndentString() << "└── Body:\n";
    indent();
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    dedent();
    dedent();
}

void ASTPrinter::visit(WhileStmt& node) {
    oss_ << getIndentString() << "├── While\n";
    indent();
    oss_ << getIndentString() << "├── Condition: ";
    if (node.getCondition()) node.getCondition()->accept(*this);
    oss_ << "\n";
    oss_ << getIndentString() << "└── Body:\n";
    indent();
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    dedent();
    dedent();
}

void ASTPrinter::visit(FunctionDeclStmt& node) {
    oss_ << getIndentString() << "├── Function: " << node.getName() << "\n";
    indent();
    oss_ << getIndentString() << "└── Body:\n";
    indent();
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    dedent();
    dedent();
}

void ASTPrinter::visit(FunctionCallStmt& node) {
    oss_ << getIndentString() << "├── Call: " << node.getName() << "\n";
}

void ASTPrinter::visit(WhenStmt& node) {
    oss_ << getIndentString() << "├── When\n";
    indent();
    oss_ << getIndentString() << "├── Trigger: ";
    if (node.getCondition()) node.getCondition()->accept(*this);
    oss_ << "\n";
    oss_ << getIndentString() << "└── Action:\n";
    indent();
    for (const auto& s : node.getBody()) {
        if (s) s->accept(*this);
    }
    dedent();
    dedent();
}

void ASTPrinter::visit(ExprStmt& node) {
    oss_ << getIndentString() << "├── Expr: ";
    if (node.getExpr()) node.getExpr()->accept(*this);
    oss_ << "\n";
}

void ASTPrinter::visit(IntegerLiteralExpr& node) {
    oss_ << node.getValue();
}

void ASTPrinter::visit(StringLiteralExpr& node) {
    oss_ << "\"" << node.getValue() << "\"";
}

void ASTPrinter::visit(BoolLiteralExpr& node) {
    oss_ << (node.getValue() ? "true" : "false");
}

void ASTPrinter::visit(IdentifierExpr& node) {
    oss_ << node.getName();
}

void ASTPrinter::visit(BinaryExpr& node) {
    oss_ << "(";
    if (node.getLeft()) node.getLeft()->accept(*this);
    oss_ << " " << binaryOpToString(node.getOp()) << " ";
    if (node.getRight()) node.getRight()->accept(*this);
    oss_ << ")";
}

void ASTPrinter::visit(UnaryExpr& node) {
    oss_ << "(" << unaryOpToString(node.getOp());
    if (node.getOperand()) node.getOperand()->accept(*this);
    oss_ << ")";
}

void ASTPrinter::visit(SensoryConditionExpr& node) {
    oss_ << sensoryTypeToString(node.getSensoryType());
}

} // namespace gamescript
