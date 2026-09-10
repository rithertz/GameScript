#pragma once

//==============================================================================
// DeadCodeEliminationPass.hpp
//
// Defines the optimization pass that removes instructions and code that do
// not contribute to the program's observable behavior.
//==============================================================================

#include "gamescript/optimizer/PassManager.hpp"

namespace gamescript::optimizer {

// Removes unused or unreachable code from the IR module.
class DeadCodeEliminationPass : public Pass {

public:

    std::string getName() const override { return "DeadCodeElimination"; }

    // Runs dead code elimination on the given IR module.
    bool runOnModule(ir::IRModule& module) override;

};

} // namespace gamescript::optimizer