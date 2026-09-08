#pragma once

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

class ConstantFoldingPass : public Pass {
public:
    std::string getName() const override { return "ConstantFolding"; }
    bool runOnModule(ir::IRModule& module) override;
};

} // namespace gamescript::optimizer
