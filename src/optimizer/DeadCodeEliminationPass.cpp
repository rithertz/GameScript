#include "gamescript/optimizer/DeadCodeEliminationPass.hpp"
#include <unordered_set>
#include <algorithm>

namespace gamescript::optimizer {

bool DeadCodeEliminationPass::runOnModule(ir::IRModule& module) {
    bool changed = false;

    for (auto& func : module.getFunctions()) {
        if (func->blocks.empty()) continue;

        // 1. Unreachable basic block elimination
        std::unordered_set<std::string> reachable;
        std::vector<std::string> worklist;

        reachable.insert(func->blocks.front()->getName());
        worklist.push_back(func->blocks.front()->getName());

        std::unordered_map<std::string, ir::BasicBlock*> blockMap;
        for (auto& b : func->blocks) {
            blockMap[b->getName()] = b.get();
        }

        while (!worklist.empty()) {
            std::string current = worklist.back();
            worklist.pop_back();

            auto it = blockMap.find(current);
            if (it == blockMap.end() || !it->second) continue;

            for (const auto& inst : it->second->getInstructions()) {
                if (inst.op == ir::OpCode::Branch) {
                    if (reachable.insert(inst.dest.name).second) {
                        worklist.push_back(inst.dest.name);
                    }
                } else if (inst.op == ir::OpCode::BranchCond) {
                    if (reachable.insert(inst.src1.name).second) {
                        worklist.push_back(inst.src1.name);
                    }
                    if (reachable.insert(inst.src2.name).second) {
                        worklist.push_back(inst.src2.name);
                    }
                }
            }
        }

        size_t initialBlockCount = func->blocks.size();
        func->blocks.erase(
            std::remove_if(func->blocks.begin(), func->blocks.end(),
                [&reachable](const std::unique_ptr<ir::BasicBlock>& b) {
                    return reachable.find(b->getName()) == reachable.end();
                }),
            func->blocks.end()
        );

        if (func->blocks.size() < initialBlockCount) {
            changed = true;
        }

        // 2. Unused register definition elimination
        // Collect all used registers
        std::unordered_set<std::string> usedRegisters;
        for (const auto& block : func->blocks) {
            for (const auto& inst : block->getInstructions()) {
                if (inst.src1.isRegister()) usedRegisters.insert(inst.src1.name);
                if (inst.src2.isRegister()) usedRegisters.insert(inst.src2.name);
                if (inst.op == ir::OpCode::BranchCond && inst.dest.isRegister()) {
                    usedRegisters.insert(inst.dest.name);
                }
            }
        }

        for (auto& block : func->blocks) {
            auto& insts = block->getInstructions();
            size_t initialInstCount = insts.size();

            insts.erase(
                std::remove_if(insts.begin(), insts.end(),
                    [&usedRegisters](const ir::Instruction& inst) {
                        // Check if instruction produces an unused register and has no side effects
                        if (inst.dest.isRegister() && usedRegisters.find(inst.dest.name) == usedRegisters.end()) {
                            switch (inst.op) {
                                case ir::OpCode::ConstInt:
                                case ir::OpCode::ConstBool:
                                case ir::OpCode::Add:
                                case ir::OpCode::Sub:
                                case ir::OpCode::Mul:
                                case ir::OpCode::Div:
                                case ir::OpCode::Mod:
                                case ir::OpCode::CmpEq:
                                case ir::OpCode::CmpNe:
                                case ir::OpCode::CmpLt:
                                case ir::OpCode::CmpLe:
                                case ir::OpCode::CmpGt:
                                case ir::OpCode::CmpGe:
                                case ir::OpCode::And:
                                case ir::OpCode::Or:
                                case ir::OpCode::Not:
                                    return true; // Safe to eliminate
                                default:
                                    break;
                            }
                        }
                        return false;
                    }),
                insts.end()
            );

            if (insts.size() < initialInstCount) {
                changed = true;
            }
        }
    }

    return changed;
}

} // namespace gamescript::optimizer
