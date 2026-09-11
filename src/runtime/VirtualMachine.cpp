#include "gamescript/runtime/VirtualMachine.hpp"
#include <iostream>

namespace gamescript::runtime {

VirtualMachine::VirtualMachine(GameWorld& world)
    : world_(world) {}

VMValue VirtualMachine::resolveOperand(const ir::Operand& op) {
    const auto& frame = callStack_.back();

    switch (op.kind) {
        case ir::Operand::Kind::ConstantInt:
            return VMValue::makeInt(op.intVal);

        case ir::Operand::Kind::ConstantBool:
            return VMValue::makeBool(op.boolVal);

        case ir::Operand::Kind::Register: {
            auto it = frame.registers.find(op.name);

            if (it != frame.registers.end()) {
                return it->second;
            }

            return VMValue::makeInt(0);
        }

        case ir::Operand::Kind::VariableName: {
            auto it = frame.variables.find(op.name);

            if (it != frame.variables.end()) {
                return it->second;
            }

            return VMValue::makeInt(0);
        }

        default:
            return VMValue{};
    }
}

void VirtualMachine::storeValue(const std::string& name, VMValue val) {
    auto& frame = callStack_.back();

    if (!name.empty() && name[0] == '%') {
        frame.registers[name] = val;
    } else {
        frame.variables[name] = val;
    }
}

bool VirtualMachine::executeFunction(
    const ir::IRModule& module,
    const ir::IRFunction& function,
    const std::vector<VMValue>& args,
    int& stepsRemaining,
    std::unordered_map<std::string, VMValue>* outputVariables
) {
    // Create a new isolated frame for this function invocation.
    callStack_.push_back({});

    // Bind function arguments to their corresponding parameters.
    for (size_t i = 0; i < function.params.size(); ++i) {
        callStack_.back().variables[function.params[i]] = args[i];
    }

    // Build block index map for this function.
    std::unordered_map<std::string, const ir::BasicBlock*> blockMap;

    for (const auto& block : function.blocks) {
        blockMap[block->getName()] = block.get();
    }

    const ir::BasicBlock* currentBlock = function.blocks.front().get();
    size_t pc = 0;

    bool success = true;
    bool returned = false;

    while (currentBlock && stepsRemaining > 0) {
        const auto& insts = currentBlock->getInstructions();

        if (pc >= insts.size()) {
            break;
        }

        --stepsRemaining;

        const auto& inst = insts[pc++];

        switch (inst.op) {
            case ir::OpCode::ConstInt:
                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(inst.src1.intVal)
                );
                break;

            case ir::OpCode::ConstBool:
                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(inst.src1.boolVal)
                );
                break;

            case ir::OpCode::Alloca:
                if (callStack_.back().variables.find(inst.dest.name)
                    == callStack_.back().variables.end()) {
                    callStack_.back().variables[inst.dest.name] =
                        VMValue::makeInt(0);
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

                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(v1.asInt() + v2.asInt())
                );
                break;
            }

            case ir::OpCode::Sub: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(v1.asInt() - v2.asInt())
                );
                break;
            }

            case ir::OpCode::Mul: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(v1.asInt() * v2.asInt())
                );
                break;
            }

            case ir::OpCode::Div: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                int64_t divisor = v2.asInt();

                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(
                        divisor != 0 ? v1.asInt() / divisor : 0
                    )
                );
                break;
            }

            case ir::OpCode::Mod: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                int64_t divisor = v2.asInt();

                storeValue(
                    inst.dest.name,
                    VMValue::makeInt(
                        divisor != 0 ? v1.asInt() % divisor : 0
                    )
                );
                break;
            }

            case ir::OpCode::CmpEq: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() == v2.asInt())
                );
                break;
            }

            case ir::OpCode::CmpNe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() != v2.asInt())
                );
                break;
            }

            case ir::OpCode::CmpLt: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() < v2.asInt())
                );
                break;
            }

            case ir::OpCode::CmpLe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() <= v2.asInt())
                );
                break;
            }

            case ir::OpCode::CmpGt: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() > v2.asInt())
                );
                break;
            }

            case ir::OpCode::CmpGe: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asInt() >= v2.asInt())
                );
                break;
            }

            case ir::OpCode::And: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asBool() && v2.asBool())
                );
                break;
            }

            case ir::OpCode::Or: {
                VMValue v1 = resolveOperand(inst.src1);
                VMValue v2 = resolveOperand(inst.src2);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(v1.asBool() || v2.asBool())
                );
                break;
            }

            case ir::OpCode::Not: {
                VMValue v1 = resolveOperand(inst.src1);

                storeValue(
                    inst.dest.name,
                    VMValue::makeBool(!v1.asBool())
                );
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

                std::string target =
                    cond.asBool() ? inst.src1.name : inst.src2.name;

                auto it = blockMap.find(target);

                if (it != blockMap.end()) {
                    currentBlock = it->second;
                    pc = 0;
                }

                break;
            }

            case ir::OpCode::Call: {
                // Resolve all arguments in the caller's frame before
                // entering the callee.
                std::vector<VMValue> args;

                for (const auto& arg : inst.args) {
                    args.push_back(resolveOperand(arg));
                }

                // Find the function being called.
                const ir::IRFunction* callee = nullptr;

                for (const auto& func : module.getFunctions()) {
                    if (func->name == inst.dest.name) {
                        callee = func.get();
                        break;
                    }
                }

                // The semantic analyzer should already have validated the
                // argument count, but the VM checks it defensively as well.
                if (!callee || args.size() != callee->params.size()) {
                    callStack_.pop_back();
                    return false;
                }

                // Execute the callee using a new call frame.
                if (!executeFunction(
                        module,
                        *callee,
                        args,
                        stepsRemaining
                    )) {
                    callStack_.pop_back();
                    return false;
                }

                break;
            }

            case ir::OpCode::Return:
                returned = true;
                break;

            case ir::OpCode::GameMove: {
                int dirCode = static_cast<int>(inst.dest.intVal);
                VMValue dist = resolveOperand(inst.src1);

                world_.movePlayer(
                    dirCode,
                    static_cast<int>(dist.asInt())
                );

                break;
            }

            case ir::OpCode::GameTurn: {
                int dirCode = static_cast<int>(inst.dest.intVal);
                VMValue deg = resolveOperand(inst.src1);

                world_.turnPlayer(
                    dirCode,
                    static_cast<int>(deg.asInt())
                );

                break;
            }

            case ir::OpCode::GameAction: {
                int actionCode = static_cast<int>(inst.dest.intVal);

                switch (actionCode) {
                    case 0:
                        world_.playerAttack();
                        break;

                    case 1:
                        world_.playerDefend();
                        break;

                    case 2:
                        world_.playerJump();
                        break;

                    case 3:
                        world_.playerInteract();
                        break;

                    case 4:
                        world_.movePlayer(1, 2);
                        break;
                }

                break;
            }

            case ir::OpCode::GameSensor: {
                int sensorId = static_cast<int>(inst.src1.intVal);
                VMValue res;

                switch (sensorId) {
                    case 0:
                        res = VMValue::makeBool(
                            world_.isEnemyNearby()
                        );
                        break;

                    case 1:
                        res = VMValue::makeBool(
                            world_.isHealthLow()
                        );
                        break;

                    case 2:
                        res = VMValue::makeBool(
                            world_.isObstacleAhead()
                        );
                        break;

                    case 3:
                        res = VMValue::makeInt(
                            world_.getDistanceToClosestEnemy()
                        );
                        break;

                    case 4:
                        res = VMValue::makeInt(
                            world_.getPlayer().hp
                        );
                        break;
                }

                storeValue(inst.dest.name, res);
                break;
            }

            default:
                break;
        }

        if (returned) {
            break;
        }
    }

    if (stepsRemaining <= 0) {
        success = false;
    }

    // Preserve the requested frame state before removing it.
    if (outputVariables != nullptr) {
        *outputVariables = callStack_.back().variables;
    }

    // Remove this function's frame before returning to its caller.
    callStack_.pop_back();

    return success;
}

bool VirtualMachine::execute(
    const ir::IRModule& module,
    const std::string& entryFunction
) {
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

    // Start a fresh execution with an empty entry-argument list.
    callStack_.clear();
    variables_.clear();

    int stepsRemaining = 10000;

    bool success = executeFunction(
        module,
        *targetFunc,
        {},
        stepsRemaining,
        &variables_
    );

    return success;
}

} // namespace gamescript::runtime