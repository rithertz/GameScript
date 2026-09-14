#include "gamescript/ir/BasicBlock.hpp"
#include <sstream>

namespace gamescript::ir {

std::string BasicBlock::toString() const {
    std::ostringstream oss;
    oss << name_ << ":\n";

    oss << "  Predecessors: [";
    for (std::size_t i = 0; i < predecessors_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << predecessors_[i];
    }
    oss << "]\n";

    oss << "  Successors: [";
    for (std::size_t i = 0; i < successors_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << successors_[i];
    }
    oss << "]\n";

    for (const auto& inst : instructions_) {
        oss << inst.toString() << "\n";
    }
    return oss.str();
}

} // namespace gamescript::ir
