#include "gamescript/runtime/VirtualMachine.hpp"
#include <iostream>

namespace gamescript::runtime {

VirtualMachine::VirtualMachine(GameWorld& world)
    : world_(world) {}

VMValue VirtualMachine::resolveOperand(const ir::Operand& op) {
    switch (op.kind) {
        case ir::Operand::Kind::ConstantInt:
            return VMValue::makeInt(op.intVal);
        case ir::Operand::Kind::ConstantBool:
            return VMValue::makeBool(op.boolVal);
        case ir::Operand::Kind::Register: {
            auto it = registers_.find(op.name);
            if (it != registers_.end()) return it->second;
            return VMValue::makeInt(0);
        }
        case ir::Operand::Kind::VariableName: {
            auto it = variables_.find(op.name);
            if (it != variables_.end()) return it->second;
            return VMValue::makeInt(0);
        }
        default:
            return VMValue();
    }
}

void VirtualMachine::storeValue(const std::string& name, VMValue val) {
    if (!name.empty() && name[0] == '%') {
        registers_[name] = val;
    } else {
        variables_[name] = val;
    }
}

bool VirtualMachine::execute(const ir::IRModule& module, const std::string& entryFunction) {
    const ir::IRFunction* targetFunc = nullptr;
    for (const auto& func : module.getFunctions()) {
        if (func->name == entryFunction) {
            targetFunc = func.get();
            break;
        }
    }

    if (!targetFunc || targetFunc->blocks.empty()) {
        return false;
    }

    // Build block index map
    std::unordered_map<std::string, const ir::BasicBlock*> blockMap;
    for (const auto& b : targetFunc->blocks) {
        blockMap[b->getName()] = b.get();
    }

    const ir::BasicBlock* currentBlock = targetFunc->blocks.front().get();
    size_t pc = 0;
    int maxSteps = 10000; // Safeguard against infinite loops

    while (currentBlock && maxSteps-- > 0) {
        const auto& insts = currentBlock->getInstructions();
        if (pc >= insts.size()) {
            break;
        }

        const auto& inst = insts[pc++];

        switch (inst.op) {
            case ir::OpCode::ConstInt:
                storeValue(inst.dest.name, VMValue::makeInt(inst.src1.intVal));
                break;
            case ir::OpCode::ConstBool:
                storeValue(inst.dest.name, VMValue::makeBool(inst.src1.boolVal));
                break;
            case ir::OpCode::Alloca:
                // Initialize variable with 0
                if (variables_.find(inst.dest.name) == variables_.end()) {
                    variables_[inst.dest.name] = VMValue::makeInt(0);
                }
                break;
            case ir::OpCode::Store: {
                VMValue val = resolveOperand(inst.src1);
                storeValue(inst.dest.name, val);
                break;
            }
            case ir::OpCode::Load: {
                VMValue val = resolveOperand(inst.src1);
                storeValue(inst.dest.name, val);
                break;
            }
            case ir::OpCode::Add: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeInt(v1.asInt() + v2.asInt()));
                break;
            }
            case ir::OpCode::Sub: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeInt(v1.asInt() - v2.asInt()));
                break;
            }
            case ir::OpCode::Mul: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeInt(v1.asInt() * v2.asInt()));
                break;
            }
            case ir::OpCode::Div: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                int64_t divisor = v2.asInt();
                storeValue(inst.dest.name, VMValue::makeInt(divisor != 0 ? v1.asInt() / divisor : 0));
                break;
            }
            case ir::OpCode::Mod: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                int64_t divisor = v2.asInt();
                storeValue(inst.dest.name, VMValue::makeInt(divisor != 0 ? v1.asInt() % divisor : 0));
                break;
            }
            case ir::OpCode::CmpEq: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() == v2.asInt()));
                break;
            }
            case ir::OpCode::CmpNe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() != v2.asInt()));
                break;
            }
            case ir::OpCode::CmpLt: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() < v2.asInt()));
                break;
            }
            case ir::OpCode::CmpLe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() <= v2.asInt()));
                break;
            }
            case ir::OpCode::CmpGt: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() > v2.asInt()));
                break;
            }
            case ir::OpCode::CmpGe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asInt() >= v2.asInt()));
                break;
            }
            case ir::OpCode::And: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asBool() && v2.asBool()));
                break;
            }
            case ir::OpCode::Or: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);
                storeValue(inst.dest.name, VMValue::makeBool(v1.asBool() || v2.asBool()));
                break;
            }
            case ir::OpCode::Not: {
                VMValue v1 = resolveOperand(inst.src1);
                storeValue(inst.dest.name, VMValue::makeBool(!v1.asBool()));
                break;
            }
            case ir::OpCode::Branch: {
                auto it = blockMap.find(inst.dest.name);
                if (it != blockMap.end()) {
                    currentBlock = it->second;
                    pc = 0;
                }
                break;
            }
            case ir::OpCode::BranchCond: {
                VMValue cond = resolveOperand(inst.dest);
                std::string target = cond.asBool() ? inst.src1.name : inst.src2.name;
                auto it = blockMap.find(target);
                if (it != blockMap.end()) {
                    currentBlock = it->second;
                    pc = 0;
                }
                break;
            }
            case ir::OpCode::Call: {
                // Recursive / Sub-function call
                execute(module, inst.dest.name);
                break;
            }
            case ir::OpCode::Return: {
                return true;
            }
            case ir::OpCode::GameMove: {
                int dirCode = static_cast<int>(inst.dest.intVal);
                VMValue dist = resolveOperand(inst.src1);
                world_.movePlayer(dirCode, static_cast<int>(dist.asInt()));
                break;
            }
            case ir::OpCode::GameTurn: {
                int dirCode = static_cast<int>(inst.dest.intVal);
                VMValue deg = resolveOperand(inst.src1);
                world_.turnPlayer(dirCode, static_cast<int>(deg.asInt()));
                break;
            }
            case ir::OpCode::GameAction: {
                int actionCode = static_cast<int>(inst.dest.intVal);
                switch (actionCode) {
                    case 0: world_.playerAttack(); break;
                    case 1: world_.playerDefend(); break;
                    case 2: world_.playerJump(); break;
                    case 3: world_.playerInteract(); break;
                    case 4: world_.movePlayer(1, 2); break; // Retreat 2 backward
                }
                break;
            }
            case ir::OpCode::GameSensor: {
                int sensorId = static_cast<int>(inst.src1.intVal);
                VMValue res;
                switch (sensorId) {
                    case 0: res = VMValue::makeBool(world_.isEnemyNearby()); break;
                    case 1: res = VMValue::makeBool(world_.isHealthLow()); break;
                    case 2: res = VMValue::makeBool(world_.isObstacleAhead()); break;
                    case 3: res = VMValue::makeInt(world_.getDistanceToClosestEnemy()); break;
                    case 4: res = VMValue::makeInt(world_.getPlayer().hp); break;
                }
                storeValue(inst.dest.name, res);
                break;
            }
            default:
                break;
        }
    }

    return true;
}

} // namespace gamescript::runtime
