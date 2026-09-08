#include "gamescript/ir/BasicBlock.hpp"
#include <sstream>

namespace gamescript::ir {

std::string BasicBlock::toString() const {
    std::ostringstream oss;
    oss << name_ << ":\n";
    for (const auto& inst : instructions_) {
        oss << inst.toString() << "\n";
    }
    return oss.str();
}

} // namespace gamescript::ir
