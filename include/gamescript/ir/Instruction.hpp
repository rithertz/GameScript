#pragma once

//==============================================================================
// Instruction.hpp
//
// Defines the instructions, operations, and operands used to represent the
// GameScript intermediate representation (IR).
//==============================================================================

#include <string>
#include <vector>
#include <variant>
#include <cstdint>

namespace gamescript::ir {

// Identifies the operation performed by an IR instruction.
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

// Converts an IR operation code to its string representation.
std::string opCodeToString(OpCode op);

// Represents a value used as an operand in an IR instruction.
struct Operand {
    // Identifies the kind of value represented by the operand.
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

    // Creates an operand representing a register.
    static Operand makeReg(int regId) {
        Operand op;
        op.kind = Kind::Register;
        op.intVal = regId;
        op.name = "%" + std::to_string(regId);
        return op;
    }

    // Creates an operand representing an integer constant.
    static Operand makeInt(int64_t val) {
        Operand op;
        op.kind = Kind::ConstantInt;
        op.intVal = val;
        op.name = std::to_string(val);
        return op;
    }

    // Creates an operand representing a boolean constant.
    static Operand makeBool(bool val) {
        Operand op;
        op.kind = Kind::ConstantBool;
        op.boolVal = val;
        op.name = val ? "true" : "false";
        return op;
    }

    // Creates an operand representing a variable name.
    static Operand makeVar(std::string varName) {
        Operand op;
        op.kind = Kind::VariableName;
        op.name = std::move(varName);
        return op;
    }

    // Creates an operand representing a label name.
    static Operand makeLabel(std::string labelName) {
        Operand op;
        op.kind = Kind::LabelName;
        op.name = std::move(labelName);
        return op;
    }

    // Creates an empty operand.
    static Operand makeNone() {
        return Operand();
    }

    // Checks whether the operand represents a register.
    bool isRegister() const { return kind == Kind::Register; }

    // Checks whether the operand represents an integer or boolean constant.
    bool isConstant() const { return kind == Kind::ConstantInt || kind == Kind::ConstantBool; }

    // Returns the string representation of the operand.
    std::string toString() const { return name; }
};

// Represents a single instruction in the GameScript intermediate representation.
struct Instruction {
    OpCode op;
    Operand dest;
    Operand src1;
    Operand src2;
    std::string comment;

    Instruction(OpCode op, Operand dest = Operand::makeNone(), Operand src1 = Operand::makeNone(), Operand src2 = Operand::makeNone(), std::string comment = "")
        : op(op), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)), comment(std::move(comment)) {}

    // Returns a human-readable representation of the instruction.
    std::string toString() const;
};

} // namespace gamescript::ir