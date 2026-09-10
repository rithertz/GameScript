#pragma once

//==============================================================================
// RedundantInstEliminationPass.hpp
//
// Defines the optimization pass that identifies and removes redundant
// instructions from the intermediate representation.
//==============================================================================

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

// Eliminates redundant instructions from the IR module.
class RedundantInstEliminationPass : public Pass {

public:

    std::string getName() const override { return "RedundantInstElimination"; }

    // Runs redundant instruction elimination on the given IR module.
    bool runOnModule(ir::IRModule& module) override;

};

} // namespace gamescript::optimizer