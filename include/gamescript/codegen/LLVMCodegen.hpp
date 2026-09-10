#pragma once

//==============================================================================
// LLVMCodegen.hpp
//
// Defines the LLVM code generator used to convert the GameScript intermediate
// representation (IR) into LLVM IR.
//==============================================================================

#include "gamescript/ir/IRModule.hpp"
#include <string>

namespace gamescript::codegen {

// Generates LLVM IR from a GameScript IR module.
class LLVMCodegen {
public:
    // Converts the given IR module into LLVM IR.
    static std::string generateLLVMIR(const ir::IRModule& module);
};

} // namespace gamescript::codegen
