#include "gamescript/ir/Instruction.hpp"
#include <sstream>
#include <iomanip>

namespace gamescript::ir {

std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::ConstInt:    return "CONST_INT";
        case OpCode::ConstBool:   return "CONST_BOOL";
        case OpCode::Alloca:      return "ALLOCA";
        case OpCode::Load:        return "LOAD";
        case OpCode::Store:       return "STORE";
        case OpCode::Add:         return "ADD";
        case OpCode::Sub:         return "SUB";
        case OpCode::Mul:         return "MUL";
        case OpCode::Div:         return "DIV";
        case OpCode::Mod:         return "MOD";
        case OpCode::CmpEq:       return "CMP_EQ";
        case OpCode::CmpNe:       return "CMP_NE";
        case OpCode::CmpLt:       return "CMP_LT";
        case OpCode::CmpLe:       return "CMP_LE";
        case OpCode::CmpGt:       return "CMP_GT";
        case OpCode::CmpGe:       return "CMP_GE";
        case OpCode::And:         return "AND";
        case OpCode::Or:          return "OR";
        case OpCode::Not:         return "NOT";
        case OpCode::Label:       return "LABEL";
        case OpCode::Branch:      return "BR";
        case OpCode::BranchCond:  return "BR_COND";
        case OpCode::Call:        return "CALL";
        case OpCode::Return:      return "RET";
        case OpCode::GameMove:    return "GAME_MOVE";
        case OpCode::GameTurn:    return "GAME_TURN";
        case OpCode::GameAction:  return "GAME_ACTION";
        case OpCode::GameSensor:  return "GAME_SENSOR";
    }
    return "UNKNOWN";
}

std::string Instruction::toString() const {
    std::ostringstream oss;
    if (op == OpCode::Label) {
        oss << dest.toString() << ":";
        return oss.str();
    }

    oss << "  ";
    oss << std::left << std::setw(12) << opCodeToString(op);

    if (dest.kind != Operand::Kind::None) {
        oss << " " << dest.toString();
    }
    if (src1.kind != Operand::Kind::None) {
        oss << ", " << src1.toString();
    }
    if (src2.kind != Operand::Kind::None) {
        oss << ", " << src2.toString();
    }

    if (!comment.empty()) {
        oss << "  # " << comment;
    }
    return oss.str();
}

} // namespace gamescript::ir
