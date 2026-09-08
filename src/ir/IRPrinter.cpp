#include "gamescript/ir/IRPrinter.hpp"
#include <sstream>

namespace gamescript::ir {

std::string IRPrinter::print(const IRModule& module) {
    std::ostringstream oss;
    oss << "=== GameScript IR Module: @" << module.getName() << " ===\n\n";

    for (const auto& func : module.getFunctions()) {
        oss << "define @" << func->name << "(";
        for (size_t i = 0; i < func->params.size(); ++i) {
            oss << "%" << func->params[i];
            if (i + 1 < func->params.size()) oss << ", ";
        }
        oss << ") {\n";

        for (const auto& block : func->blocks) {
            oss << block->getName() << ":\n";
            for (const auto& inst : block->getInstructions()) {
                oss << inst.toString() << "\n";
            }
        }
        oss << "}\n\n";
    }

    return oss.str();
}

std::string IRModule::toString() const {
    return IRPrinter::print(*this);
}

} // namespace gamescript::ir
