#pragma once

#include <string>
#include <vector>
#include <variant>
#include <cstdint>

namespace gamescript::ir {

enum class OpCode {
    // Constants & Memory
    ConstInt,
    ConstBool,
    Alloca,
    Load,
    Store,

    // Arithmetic & Logic
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    CmpEq,
    CmpNe,
    CmpLt,
    CmpLe,
    CmpGt,
    CmpGe,
    And,
    Or,
    Not,

    // Control Flow
    Label,
    Branch,
    BranchCond,
    Call,
    Return,

    // Game Runtime Instructions
    GameMove,
    GameTurn,
    GameAction,
    GameSensor
};

std::string opCodeToString(OpCode op);

struct Operand {
    enum class Kind {
        Register,
        ConstantInt,
        ConstantBool,
        VariableName,
        LabelName,
        None
    };

    Kind kind = Kind::None;
    int64_t intVal = 0;
    bool boolVal = false;
    std::string name;

    static Operand makeReg(int regId) {
        Operand op;
        op.kind = Kind::Register;
        op.intVal = regId;
        op.name = "%" + std::to_string(regId);
        return op;
    }

    static Operand makeInt(int64_t val) {
        Operand op;
        op.kind = Kind::ConstantInt;
        op.intVal = val;
        op.name = std::to_string(val);
        return op;
    }

    static Operand makeBool(bool val) {
        Operand op;
        op.kind = Kind::ConstantBool;
        op.boolVal = val;
        op.name = val ? "true" : "false";
        return op;
    }

    static Operand makeVar(std::string varName) {
        Operand op;
        op.kind = Kind::VariableName;
        op.name = std::move(varName);
        return op;
    }

    static Operand makeLabel(std::string labelName) {
        Operand op;
        op.kind = Kind::LabelName;
        op.name = std::move(labelName);
        return op;
    }

    static Operand makeNone() {
        return Operand();
    }

    bool isRegister() const { return kind == Kind::Register; }
    bool isConstant() const { return kind == Kind::ConstantInt || kind == Kind::ConstantBool; }
    std::string toString() const { return name; }
};

struct Instruction {
    OpCode op;
    Operand dest;
    Operand src1;
    Operand src2;
    std::string comment;

    Instruction(OpCode op, Operand dest = Operand::makeNone(), Operand src1 = Operand::makeNone(), Operand src2 = Operand::makeNone(), std::string comment = "")
        : op(op), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)), comment(std::move(comment)) {}

    std::string toString() const;
};

} // namespace gamescript::ir
