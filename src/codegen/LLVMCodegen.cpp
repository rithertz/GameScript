#include "gamescript/codegen/LLVMCodegen.hpp"
#include <sstream>
#include <unordered_map>

namespace gamescript::codegen {

static std::string formatOperand(const ir::Operand& op) {
    switch (op.kind) {
        case ir::Operand::Kind::ConstantInt:
            return std::to_string(op.intVal);
        case ir::Operand::Kind::ConstantBool:
            return op.boolVal ? "1" : "0";
        case ir::Operand::Kind::Register:
            return op.name;
        case ir::Operand::Kind::VariableName:
            return "%var_" + op.name;
        default:
            return "0";
    }
}

std::string LLVMCodegen::generateLLVMIR(const ir::IRModule& module) {
    std::ostringstream oss;

    oss << "; ModuleID = '" << module.getName() << "'\n";
    oss << "source_filename = \"" << module.getName() << ".gs\"\n";
    oss << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
    oss << "target triple = \"x86_64-pc-windows-msvc\"\n\n";

    // Runtime declarations
    oss << "; --- GameScript Host Runtime ABI Declarations ---\n";
    oss << "declare void @gs_runtime_move(i32 noundef %dir, i64 noundef %steps)\n";
    oss << "declare void @gs_runtime_turn(i32 noundef %dir, i64 noundef %degrees)\n";
    oss << "declare void @gs_runtime_action(i32 noundef %actionCode)\n";
    oss << "declare i64 @gs_runtime_sensor(i32 noundef %sensorId)\n";
    oss << "declare void @gs_runtime_print_int(i64 noundef %val)\n\n";

    // Function definitions
    for (const auto& func : module.getFunctions()) {
        oss << "define void @" << func->name << "(";
        for (size_t i = 0; i < func->params.size(); ++i) {
            oss << "i64 %" << func->params[i];
            if (i + 1 < func->params.size()) oss << ", ";
        }
        oss << ") {\n";

        for (const auto& block : func->blocks) {
            oss << block->getName() << ":\n";

            for (const auto& inst : block->getInstructions()) {
                switch (inst.op) {
                    case ir::OpCode::Alloca:
                        oss << "  %var_" << inst.dest.name << " = alloca i64, align 8\n";
                        break;
                    case ir::OpCode::Store:
                        oss << "  store i64 " << formatOperand(inst.src1) << ", ptr %var_" << inst.dest.name << ", align 8\n";
                        break;
                    case ir::OpCode::Load:
                        oss << "  " << inst.dest.name << " = load i64, ptr %var_" << inst.src1.name << ", align 8\n";
                        break;
                    case ir::OpCode::ConstInt:
                        oss << "  " << inst.dest.name << " = add i64 0, " << inst.src1.intVal << "\n";
                        break;
                    case ir::OpCode::ConstBool:
                        oss << "  " << inst.dest.name << " = add i1 0, " << (inst.src1.boolVal ? 1 : 0) << "\n";
                        break;
                    case ir::OpCode::Add:
                        oss << "  " << inst.dest.name << " = add i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Sub:
                        oss << "  " << inst.dest.name << " = sub i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Mul:
                        oss << "  " << inst.dest.name << " = mul i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Div:
                        oss << "  " << inst.dest.name << " = sdiv i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Mod:
                        oss << "  " << inst.dest.name << " = srem i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpEq:
                        oss << "  " << inst.dest.name << " = icmp eq i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpNe:
                        oss << "  " << inst.dest.name << " = icmp ne i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpLt:
                        oss << "  " << inst.dest.name << " = icmp slt i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpLe:
                        oss << "  " << inst.dest.name << " = icmp sle i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpGt:
                        oss << "  " << inst.dest.name << " = icmp sgt i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::CmpGe:
                        oss << "  " << inst.dest.name << " = icmp sge i64 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::And:
                        oss << "  " << inst.dest.name << " = and i1 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Or:
                        oss << "  " << inst.dest.name << " = or i1 " << formatOperand(inst.src1) << ", " << formatOperand(inst.src2) << "\n";
                        break;
                    case ir::OpCode::Not:
                        oss << "  " << inst.dest.name << " = xor i1 " << formatOperand(inst.src1) << ", true\n";
                        break;
                    case ir::OpCode::Branch:
                        oss << "  br label %" << inst.dest.name << "\n";
                        break;
                    case ir::OpCode::BranchCond:
                        oss << "  br i1 " << formatOperand(inst.dest) << ", label %" << inst.src1.name << ", label %" << inst.src2.name << "\n";
                        break;
                    case ir::OpCode::Call:
                        oss << "  call void @" << inst.dest.name << "()\n";
                        break;
                    case ir::OpCode::Return:
                        oss << "  ret void\n";
                        break;
                    case ir::OpCode::GameMove:
                        oss << "  call void @gs_runtime_move(i32 " << inst.dest.intVal << ", i64 " << formatOperand(inst.src1) << ")\n";
                        break;
                    case ir::OpCode::GameTurn:
                        oss << "  call void @gs_runtime_turn(i32 " << inst.dest.intVal << ", i64 " << formatOperand(inst.src1) << ")\n";
                        break;
                    case ir::OpCode::GameAction:
                        oss << "  call void @gs_runtime_action(i32 " << inst.dest.intVal << ")\n";
                        break;
                    case ir::OpCode::GameSensor:
                        oss << "  " << inst.dest.name << " = call i64 @gs_runtime_sensor(i32 " << inst.src1.intVal << ")\n";
                        break;
                    default:
                        break;
                }
            }
        }
        oss << "}\n\n";
    }

    return oss.str();
}

} // namespace gamescript::codegen
