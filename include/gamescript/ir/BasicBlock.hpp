#pragma once

//==============================================================================
// BasicBlock.hpp
//
// Defines a basic block in the GameScript intermediate representation (IR),
// containing a sequence of instructions and its control-flow connections.
//==============================================================================

#include "gamescript/ir/Instruction.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript::ir {

// Represents a basic block containing instructions and control-flow information.
class BasicBlock {
public:
    explicit BasicBlock(std::string name) : name_(std::move(name)) {}

    const std::string& getName() const { return name_; }
    void addInstruction(Instruction inst) { instructions_.push_back(std::move(inst)); }
    
    std::vector<Instruction>& getInstructions() { return instructions_; }
    const std::vector<Instruction>& getInstructions() const { return instructions_; }

    void addPredecessor(std::string pred) { predecessors_.push_back(std::move(pred)); }
    void addSuccessor(std::string succ) { successors_.push_back(std::move(succ)); }

    const std::vector<std::string>& getPredecessors() const { return predecessors_; }
    const std::vector<std::string>& getSuccessors() const { return successors_; }

    // Returns a human-readable representation of the basic block.
    std::string toString() const;

private:
    std::string name_;
    std::vector<Instruction> instructions_;
    std::vector<std::string> predecessors_;
    std::vector<std::string> successors_;
};

// Owning pointer to a basic block.
using BasicBlockPtr = std::unique_ptr<BasicBlock>;

} // namespace gamescript::ir