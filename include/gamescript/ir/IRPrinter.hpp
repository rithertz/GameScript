#pragma once

#include "gamescript/ir/IRModule.hpp"
#include <string>

namespace gamescript::ir {

class IRPrinter {
public:
    static std::string print(const IRModule& module);
};

} // namespace gamescript::ir
