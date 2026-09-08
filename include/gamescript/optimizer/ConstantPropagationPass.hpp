#pragma once

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

class ConstantPropagationPass : public Pass {
public:
    std::string getName() const override { return "ConstantPropagation"; }
    bool runOnModule(ir::IRModule& module) override;
};

} // namespace gamescript::optimizer
