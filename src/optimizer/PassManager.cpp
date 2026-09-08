#include "gamescript/optimizer/PassManager.hpp"
#include "gamescript/optimizer/ConstantFoldingPass.hpp"
#include "gamescript/optimizer/ConstantPropagationPass.hpp"
#include "gamescript/optimizer/DeadCodeEliminationPass.hpp"
#include "gamescript/optimizer/RedundantInstEliminationPass.hpp"

namespace gamescript::optimizer {

std::unique_ptr<PassManager> PassManager::createDefaultPipeline() {
    auto pm = std::make_unique<PassManager>();
    pm->addPass(std::make_unique<ConstantPropagationPass>());
    pm->addPass(std::make_unique<ConstantFoldingPass>());
    pm->addPass(std::make_unique<ConstantPropagationPass>());
    pm->addPass(std::make_unique<RedundantInstEliminationPass>());
    pm->addPass(std::make_unique<DeadCodeEliminationPass>());
    return pm;
}

} // namespace gamescript::optimizer
