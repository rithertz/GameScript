#pragma once

#include "gamescript/ir/Instruction.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript::ir {

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

    std::string toString() const;

private:
    std::string name_;
    std::vector<Instruction> instructions_;
    std::vector<std::string> predecessors_;
    std::vector<std::string> successors_;
};

using BasicBlockPtr = std::unique_ptr<BasicBlock>;

} // namespace gamescript::ir
