#pragma once

#include "gamescript/ir/IRModule.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript::optimizer {

class Pass {
public:
    virtual ~Pass() = default;
    virtual std::string getName() const = 0;
    virtual bool runOnModule(ir::IRModule& module) = 0;
};

class PassManager {
public:
    void addPass(std::unique_ptr<Pass> pass) {
        passes_.push_back(std::move(pass));
    }

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

    static std::unique_ptr<PassManager> createDefaultPipeline();

private:
    std::vector<std::unique_ptr<Pass>> passes_;
};

} // namespace gamescript::optimizer
