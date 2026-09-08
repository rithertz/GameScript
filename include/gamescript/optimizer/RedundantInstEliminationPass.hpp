#pragma once

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

class RedundantInstEliminationPass : public Pass {
public:
    std::string getName() const override { return "RedundantInstElimination"; }
    bool runOnModule(ir::IRModule& module) override;
};

} // namespace gamescript::optimizer
