#pragma once

//==============================================================================
// IRModule.hpp
//
// Defines the IRFunction and IRModule structures used to organize functions
// and basic blocks in the GameScript intermediate representation.
//==============================================================================

#include "gamescript/ir/BasicBlock.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace gamescript::ir {

// Represents a function in the intermediate representation.
struct IRFunction {
    std::string name;
    std::vector<std::string> params;
    std::vector<BasicBlockPtr> blocks;

    BasicBlock* getEntryBlock() const {
        return blocks.empty() ? nullptr : blocks.front().get();
    }
};

// Represents a complete IR module containing the program's functions.
class IRModule {
public:
    explicit IRModule(std::string name = "gamescript_module") : name_(std::move(name)) {}

    const std::string& getName() const { return name_; }

    // Adds a function to the module.
    void addFunction(std::unique_ptr<IRFunction> func) {
        functions_.push_back(std::move(func));
    }

    const std::vector<std::unique_ptr<IRFunction>>& getFunctions() const {
        return functions_;
    }

    std::vector<std::unique_ptr<IRFunction>>& getFunctions() {
        return functions_;
    }

    // Returns the main function, or the first function if no main exists.
    IRFunction* getMainFunction() const {
        for (const auto& f : functions_) {
            if (f->name == "main") return f.get();
        }
        return functions_.empty() ? nullptr : functions_.front().get();
    }

    // Converts the module and its functions to a string representation.
    std::string toString() const;

private:
    std::string name_;
    std::vector<std::unique_ptr<IRFunction>> functions_;
};

} // namespace gamescript::ir