#pragma once

//==============================================================================
// ConstantFoldingPass.hpp
//
// Defines the optimization pass that evaluates constant expressions at
// compile time to simplify the intermediate representation.
//==============================================================================

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

// Performs constant folding optimization on the IR module.
class ConstantFoldingPass : public Pass {

public:

    std::string getName() const override { return "ConstantFolding"; }

    // Runs constant folding on the given IR module.
    bool runOnModule(ir::IRModule& module) override;

};

} // namespace gamescript::optimizer