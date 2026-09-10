#pragma once

//==============================================================================
// ConstantPropagationPass.hpp
//
// Defines the optimization pass that propagates known constant values through
// the intermediate representation to simplify subsequent instructions.
//==============================================================================

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

// Propagates constant values through the IR module.
class ConstantPropagationPass : public Pass {

public:

    std::string getName() const override { return "ConstantPropagation"; }

    // Runs constant propagation on the given IR module.
    bool runOnModule(ir::IRModule& module) override;

};

} // namespace gamescript::optimizer