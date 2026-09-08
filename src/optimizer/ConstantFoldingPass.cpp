#include "gamescript/optimizer/ConstantFoldingPass.hpp"

namespace gamescript::optimizer {

bool ConstantFoldingPass::runOnModule(ir::IRModule& module) {
    bool changed = false;

    for (auto& func : module.getFunctions()) {
        for (auto& block : func->blocks) {
            for (auto& inst : block->getInstructions()) {
                if (inst.src1.kind == ir::Operand::Kind::ConstantInt &&
                    inst.src2.kind == ir::Operand::Kind::ConstantInt) {
                    
                    int64_t a = inst.src1.intVal;
                    int64_t b = inst.src2.intVal;
                    bool folded = false;
                    int64_t resInt = 0;
                    bool resBool = false;
                    bool isBool = false;

                    switch (inst.op) {
                        case ir::OpCode::Add:
                            resInt = a + b;
                            folded = true;
                            break;
                        case ir::OpCode::Sub:
                            resInt = a - b;
                            folded = true;
                            break;
                        case ir::OpCode::Mul:
                            resInt = a * b;
                            folded = true;
                            break;
                        case ir::OpCode::Div:
                            if (b != 0) {
                                resInt = a / b;
                                folded = true;
                            }
                            break;
                        case ir::OpCode::Mod:
                            if (b != 0) {
                                resInt = a % b;
                                folded = true;
                            }
                            break;
                        case ir::OpCode::CmpEq:
                            resBool = (a == b);
                            isBool = true;
                            folded = true;
                            break;
                        case ir::OpCode::CmpNe:
                            resBool = (a != b);
                            isBool = true;
                            folded = true;
                            break;
                        case ir::OpCode::CmpLt:
                            resBool = (a < b);
                            isBool = true;
                            folded = true;
                            break;
                        case ir::OpCode::CmpLe:
                            resBool = (a <= b);
                            isBool = true;
                            folded = true;
                            break;
                        case ir::OpCode::CmpGt:
                            resBool = (a > b);
                            isBool = true;
                            folded = true;
                            break;
                        case ir::OpCode::CmpGe:
                            resBool = (a >= b);
                            isBool = true;
                            folded = true;
                            break;
                        default:
                            break;
                    }

                    if (folded) {
                        if (isBool) {
                            inst.op = ir::OpCode::ConstBool;
                            inst.src1 = ir::Operand::makeBool(resBool);
                            inst.src2 = ir::Operand::makeNone();
                        } else {
                            inst.op = ir::OpCode::ConstInt;
                            inst.src1 = ir::Operand::makeInt(resInt);
                            inst.src2 = ir::Operand::makeNone();
                        }
                        inst.comment = "constant-folded";
                        changed = true;
                    }
                }
            }
        }
    }

    return changed;
}

} // namespace gamescript::optimizer
