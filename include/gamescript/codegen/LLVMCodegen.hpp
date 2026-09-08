#pragma once

#include "gamescript/ir/IRModule.hpp"
#include <string>

namespace gamescript::codegen {

class LLVMCodegen {
public:
    static std::string generateLLVMIR(const ir::IRModule& module);
};

} // namespace gamescript::codegen
