#pragma once

//==============================================================================
// PassManager.hpp
//
// Defines the optimization pass interface and pass manager used to execute
// optimization passes over the GameScript intermediate representation.
//==============================================================================

#include "gamescript/ir/IRModule.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript::optimizer {

// Base interface for an optimization pass.
class Pass {
public:
    virtual ~Pass() = default;
    virtual std::string getName() const = 0;
    virtual bool runOnModule(ir::IRModule& module) = 0;
};

// Manages and executes a sequence of optimization passes.
class PassManager {
public:
    // Adds an optimization pass to the pipeline.
    void addPass(std::unique_ptr<Pass> pass) {
        passes_.push_back(std::move(pass));
    }

    // Runs the optimization passes until no further changes are made or the
    // maximum number of iterations is reached.
    bool run(ir::IRModule& module, int maxIterations = 5) {
        bool anyChanged = false;
        for (int iter = 0; iter < maxIterations; ++iter) {
            bool iterChanged = false;
            for (auto& pass : passes_) {
                if (pass->runOnModule(module)) {
                    iterChanged = true;
                    anyChanged = true;
                }
            }
            if (!iterChanged) break; // Reached fixed point
        }
        return anyChanged;
    }

    // Creates the default optimization pass pipeline.
    static std::unique_ptr<PassManager> createDefaultPipeline();

private:
    std::vector<std::unique_ptr<Pass>> passes_;
};

} // namespace gamescript::optimizer