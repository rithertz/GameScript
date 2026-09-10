#pragma once

//==============================================================================
// IRPrinter.hpp
//
// Defines the IRPrinter utility used to convert an IR module into a readable
// string representation.
//==============================================================================

#include "gamescript/ir/IRModule.hpp"
#include <string>

namespace gamescript::ir {

// Provides functionality for printing the intermediate representation.
class IRPrinter {
public:
    // Converts the given IR module into a string representation.
    static std::string print(const IRModule& module);
};

} // namespace gamescript::ir