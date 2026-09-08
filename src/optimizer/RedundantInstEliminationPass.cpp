#include "gamescript/optimizer/RedundantInstEliminationPass.hpp"
#include <algorithm>

namespace gamescript::optimizer {

bool RedundantInstEliminationPass::runOnModule(ir::IRModule& module) {
    bool changed = false;

    for (auto& func : module.getFunctions()) {
        for (auto& block : func->blocks) {
            auto& insts = block->getInstructions();
            size_t initialSize = insts.size();

            insts.erase(
                std::remove_if(insts.begin(), insts.end(),
                    [](const ir::Instruction& inst) {
                        // Eliminate 0-distance movement: GAME_MOVE <dir>, 0
                        if (inst.op == ir::OpCode::GameMove &&
                            inst.src1.kind == ir::Operand::Kind::ConstantInt &&
                            inst.src1.intVal == 0) {
                            return true;
                        }
                        // Eliminate 0-degree turn: GAME_TURN <dir>, 0
                        if (inst.op == ir::OpCode::GameTurn &&
                            inst.src1.kind == ir::Operand::Kind::ConstantInt &&
                            inst.src1.intVal == 0) {
                            return true;
                        }
                        return false;
                    }),
                insts.end()
            );

            if (insts.size() < initialSize) {
                changed = true;
            }
        }
    }

    return changed;
}

} // namespace gamescript::optimizer
