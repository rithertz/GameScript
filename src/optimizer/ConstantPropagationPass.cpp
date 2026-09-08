#include "gamescript/optimizer/ConstantPropagationPass.hpp"
#include <unordered_map>
#include <unordered_set>

namespace gamescript::optimizer {

bool ConstantPropagationPass::runOnModule(ir::IRModule& module) {
    bool changed = false;

    for (auto& func : module.getFunctions()) {
        // Count stores per variable to detect mutable/loop variables
        std::unordered_map<std::string, int> storeCounts;
        for (const auto& block : func->blocks) {
            for (const auto& inst : block->getInstructions()) {
                if (inst.op == ir::OpCode::Store) {
                    storeCounts[inst.dest.name]++;
                }
            }
        }

        std::unordered_map<std::string, ir::Operand> knownConstants;

        for (auto& block : func->blocks) {
            for (auto& inst : block->getInstructions()) {
                // Track constant values stored into immutable variables
                if (inst.op == ir::OpCode::Store && inst.src1.isConstant()) {
                    if (storeCounts[inst.dest.name] == 1) {
                        knownConstants[inst.dest.name] = inst.src1;
                    }
                } else if (inst.op == ir::OpCode::ConstInt || inst.op == ir::OpCode::ConstBool) {
                    knownConstants[inst.dest.name] = inst.src1;
                }

                // If loading from a single-assignment variable with a known constant value
                if (inst.op == ir::OpCode::Load) {
                    auto it = knownConstants.find(inst.src1.name);
                    if (it != knownConstants.end()) {
                        knownConstants[inst.dest.name] = it->second;
                        inst.op = it->second.kind == ir::Operand::Kind::ConstantBool ? ir::OpCode::ConstBool : ir::OpCode::ConstInt;
                        inst.src1 = it->second;
                        inst.src2 = ir::Operand::makeNone();
                        inst.comment = "propagated constant";
                        changed = true;
                    }
                }

                // Replace uses of registers with known constants
                if (inst.src1.isRegister()) {
                    auto it = knownConstants.find(inst.src1.name);
                    if (it != knownConstants.end()) {
                        inst.src1 = it->second;
                        changed = true;
                    }
                }
                if (inst.src2.isRegister()) {
                    auto it = knownConstants.find(inst.src2.name);
                    if (it != knownConstants.end()) {
                        inst.src2 = it->second;
                        changed = true;
                    }
                }
            }
        }
    }

    return changed;
}

} // namespace gamescript::optimizer
