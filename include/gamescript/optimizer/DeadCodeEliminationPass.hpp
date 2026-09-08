#pragma once

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

class DeadCodeEliminationPass : public Pass {
public:
    std::string getName() const override { return "DeadCodeElimination"; }
    bool runOnModule(ir::IRModule& module) override;
};

} // namespace gamescript::optimizer
