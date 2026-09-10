#pragma once

//==============================================================================
// VirtualMachine.hpp
//
// Defines the runtime virtual machine responsible for executing the
// GameScript intermediate representation within a game world.
//==============================================================================

#include "gamescript/ir/IRModule.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace gamescript::runtime {

// Represents a value stored and manipulated by the virtual machine.
struct VMValue {
    enum class Type { Integer, Boolean, None } type = Type::None;
    int64_t intVal = 0;
    bool boolVal = false;

    static VMValue makeInt(int64_t v) {
        VMValue val;
        val.type = Type::Integer;
        val.intVal = v;
        return val;
    }

    static VMValue makeBool(bool v) {
        VMValue val;
        val.type = Type::Boolean;
        val.boolVal = v;
        return val;
    }

    bool asBool() const {
        if (type == Type::Boolean) return boolVal;
        return intVal != 0;
    }

    int64_t asInt() const {
        if (type == Type::Integer) return intVal;
        return boolVal ? 1 : 0;
    }
};

// Executes IR instructions against a runtime game world.
class VirtualMachine {
public:
    explicit VirtualMachine(GameWorld& world);

    // Executes the specified IR module starting from the entry function.
    bool execute(const ir::IRModule& module, const std::string& entryFunction = "main");

    const std::unordered_map<std::string, VMValue>& getVariables() const { return variables_; }

private:
    // Resolves an IR operand to a runtime value.
    VMValue resolveOperand(const ir::Operand& op);

    // Stores a runtime value under the specified variable name.
    void storeValue(const std::string& name, VMValue val);

    GameWorld& world_;
    std::unordered_map<std::string, VMValue> registers_;
    std::unordered_map<std::string, VMValue> variables_;
};

} // namespace gamescript::runtime