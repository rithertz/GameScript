#include "gamescript/semantic/SemanticAnalyzer.hpp"

namespace gamescript {

SemanticAnalyzer::SemanticAnalyzer(DiagnosticEngine* diagnostics)
    : diagnostics_(diagnostics) {}

bool SemanticAnalyzer::analyze(Program& program) {
    hasInternalErrors_ = false;
    program.accept(*this);
    return !hasErrors();
}

void SemanticAnalyzer::setExprType(Expr* expr, DataType type) {
    if (expr) {
        exprTypes_[expr] = type;
        lastEvaluatedType_ = type;
    }
}

DataType SemanticAnalyzer::getExprType(Expr* expr) const {
    if (!expr) return DataType::Unknown;
    auto it = exprTypes_.find(expr);
    if (it != exprTypes_.end()) {
        return it->second;
    }
    return DataType::Unknown;
}

void SemanticAnalyzer::visit(Program& node) {
    for (const auto& stmt : node.getStatements()) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
}

void SemanticAnalyzer::visit(VarDeclStmt& node) {
    DataType initType = DataType::Integer;
    if (node.getInitializer()) {
        node.getInitializer()->accept(*this);
        initType = lastEvaluatedType_;
    }

    if (symbolTable_.resolveCurrent(node.getVarName()).has_value()) {
        std::string msg = "Redefinition of variable '" + node.getVarName() + "' in the same scope.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
    } else {
        Symbol sym;
        sym.name = node.getVarName();
        sym.kind = SymbolKind::Variable;
        sym.type = initType;
        sym.span = node.getSpan();
        sym.scopeLevel = symbolTable_.getCurrentScopeLevel();
        symbolTable_.define(sym);
    }
}

void SemanticAnalyzer::visit(MoveStmt& node) {
    if (node.getDistance()) {
        node.getDistance()->accept(*this);
        if (lastEvaluatedType_ != DataType::Integer && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "Movement distance must evaluate to an integer, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getDistance()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }
}

void SemanticAnalyzer::visit(TurnStmt& node) {
    if (node.getDegrees()) {
        node.getDegrees()->accept(*this);
        if (lastEvaluatedType_ != DataType::Integer && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "Turn degrees must evaluate to an integer, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getDegrees()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }
}

void SemanticAnalyzer::visit(ActionStmt&) {
    // Actions (attack, defend, jump, interact, retreat) are always semantically valid
}

void SemanticAnalyzer::visit(IfStmt& node) {
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        if (lastEvaluatedType_ != DataType::Boolean && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "Condition in 'if' statement must evaluate to a boolean, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getCondition()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }

    symbolTable_.enterScope();
    for (const auto& stmt : node.getThenBranch()) {
        if (stmt) stmt->accept(*this);
    }
    symbolTable_.exitScope();

    if (!node.getElseBranch().empty()) {
        symbolTable_.enterScope();
        for (const auto& stmt : node.getElseBranch()) {
            if (stmt) stmt->accept(*this);
        }
        symbolTable_.exitScope();
    }
}

void SemanticAnalyzer::visit(RepeatStmt& node) {
    if (node.getCountExpr()) {
        node.getCountExpr()->accept(*this);
        if (lastEvaluatedType_ != DataType::Integer && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "Repeat loop count must evaluate to an integer, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getCountExpr()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }

    symbolTable_.enterScope();
    for (const auto& stmt : node.getBody()) {
        if (stmt) stmt->accept(*this);
    }
    symbolTable_.exitScope();
}

void SemanticAnalyzer::visit(WhileStmt& node) {
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        if (lastEvaluatedType_ != DataType::Boolean && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "While loop condition must evaluate to a boolean, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getCondition()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }

    symbolTable_.enterScope();
    for (const auto& stmt : node.getBody()) {
        if (stmt) stmt->accept(*this);
    }
    symbolTable_.exitScope();
}

void SemanticAnalyzer::visit(FunctionDeclStmt& node) {
    if (symbolTable_.resolveCurrent(node.getName()).has_value()) {
        std::string msg = "Redefinition of function '" + node.getName() + "'.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
    } else {
        Symbol funcSym;
        funcSym.name = node.getName();
        funcSym.kind = SymbolKind::Function;
        funcSym.type = DataType::Void;
        funcSym.paramCount = node.getParams().size();
        funcSym.span = node.getSpan();

        // GameScript function parameters currently use integer values.
        // Store the parameter types in declaration order so call sites
        // can validate each argument against its corresponding parameter.
        funcSym.paramTypes.resize(node.getParams().size(), DataType::Integer);

        symbolTable_.define(funcSym);
    }

    symbolTable_.enterScope();
    for (const auto& param : node.getParams()) {
        Symbol paramSym;
        paramSym.name = param;
        paramSym.kind = SymbolKind::Parameter;
        paramSym.type = DataType::Integer; // GameScript parameters currently use integer values.
        paramSym.span = node.getSpan();
        paramSym.scopeLevel = symbolTable_.getCurrentScopeLevel();

        // Parameter names share the function's local scope. Reject duplicate
        // parameter names instead of silently ignoring a failed definition.
        if (!symbolTable_.define(paramSym)) {
            std::string msg =
                "Duplicate parameter '" + param +
                "' in function '" + node.getName() + "'.";

            if (diagnostics_) {
                diagnostics_->reportSemanticError(node.getSpan(), msg);
            }

            hasInternalErrors_ = true;
        }
    }

    for (const auto& stmt : node.getBody()) {
        if (stmt) stmt->accept(*this);
    }
    symbolTable_.exitScope();
}

void SemanticAnalyzer::visit(FunctionCallStmt& node) {
    auto sym = symbolTable_.resolve(node.getName());
    if (!sym.has_value()) {
        std::string msg = "Call to undeclared function '" + node.getName() + "'.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
        return;
    }

    if (sym->kind != SymbolKind::Function) {
        std::string msg = "'" + node.getName() + "' is a variable, not a function.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
        return;
    }

    if (node.getArgs().size() != sym->paramCount) {
        std::string msg = "Function '" + node.getName() + "' expects " + std::to_string(sym->paramCount) +
                        " arguments, but " + std::to_string(node.getArgs().size()) + " were provided.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
    }

    // Validate each argument against the corresponding parameter type.
    // Only arguments with a matching parameter are type-checked, which
    // prevents out-of-range access when the argument count is incorrect.
    const size_t argumentsToCheck = std::min(node.getArgs().size(), sym->paramTypes.size());

    for (size_t i = 0; i < argumentsToCheck; ++i) {
        const auto& arg = node.getArgs()[i];

        if (!arg) {
            continue;
        }

        arg->accept(*this);
        DataType argType = lastEvaluatedType_;
        DataType paramType = sym->paramTypes[i];

        if (argType != paramType && argType != DataType::Unknown) {
            std::string msg =
                "Argument " + std::to_string(i + 1) + " of function '" +
                node.getName() + "' expects '" + dataTypeToString(paramType) +
                "', but found '" + dataTypeToString(argType) + "'.";

            if (diagnostics_) {
                diagnostics_->reportSemanticError(arg->getSpan(), msg);
            }

            hasInternalErrors_ = true;
        }
    }
}

void SemanticAnalyzer::visit(WhenStmt& node) {
    if (node.getCondition()) {
        node.getCondition()->accept(*this);
        if (lastEvaluatedType_ != DataType::Boolean && lastEvaluatedType_ != DataType::Unknown) {
            std::string msg = "Condition in 'when' trigger must evaluate to a boolean, found '" + dataTypeToString(lastEvaluatedType_) + "'.";
            if (diagnostics_) diagnostics_->reportSemanticError(node.getCondition()->getSpan(), msg);
            hasInternalErrors_ = true;
        }
    }

    symbolTable_.enterScope();
    for (const auto& stmt : node.getBody()) {
        if (stmt) stmt->accept(*this);
    }
    symbolTable_.exitScope();
}

void SemanticAnalyzer::visit(ExprStmt& node) {
    if (node.getExpr()) {
        node.getExpr()->accept(*this);
    }
}

void SemanticAnalyzer::visit(IntegerLiteralExpr& node) {
    setExprType(&node, DataType::Integer);
}

void SemanticAnalyzer::visit(StringLiteralExpr& node) {
    setExprType(&node, DataType::String);
}

void SemanticAnalyzer::visit(BoolLiteralExpr& node) {
    setExprType(&node, DataType::Boolean);
}

void SemanticAnalyzer::visit(IdentifierExpr& node) {
    auto sym = symbolTable_.resolve(node.getName());
    if (!sym.has_value()) {
        std::string msg = "Use of undeclared identifier '" + node.getName() + "'.";
        if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), msg);
        hasInternalErrors_ = true;
        setExprType(&node, DataType::Unknown);
    } else {
        setExprType(&node, sym->type);
    }
}

void SemanticAnalyzer::visit(BinaryExpr& node) {
    DataType leftType = DataType::Unknown;
    DataType rightType = DataType::Unknown;

    if (node.getLeft()) {
        node.getLeft()->accept(*this);
        leftType = lastEvaluatedType_;
    }
    if (node.getRight()) {
        node.getRight()->accept(*this);
        rightType = lastEvaluatedType_;
    }

    switch (node.getOp()) {
        case BinaryOp::Add:
        case BinaryOp::Subtract:
        case BinaryOp::Multiply:
        case BinaryOp::Divide:
        case BinaryOp::Modulo:
            if (leftType != DataType::Integer && leftType != DataType::Unknown) {
                if (diagnostics_) diagnostics_->reportSemanticError(node.getLeft()->getSpan(), "Arithmetic operand must be integer.");
                hasInternalErrors_ = true;
            }
            if (rightType != DataType::Integer && rightType != DataType::Unknown) {
                if (diagnostics_) diagnostics_->reportSemanticError(node.getRight()->getSpan(), "Arithmetic operand must be integer.");
                hasInternalErrors_ = true;
            }
            setExprType(&node, DataType::Integer);
            break;

        case BinaryOp::Equal:
        case BinaryOp::NotEqual:
        case BinaryOp::Less:
        case BinaryOp::LessEqual:
        case BinaryOp::Greater:
        case BinaryOp::GreaterEqual:
            if (leftType != rightType && leftType != DataType::Unknown && rightType != DataType::Unknown) {
                if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), "Type mismatch in comparison: '" + dataTypeToString(leftType) + "' vs '" + dataTypeToString(rightType) + "'.");
                hasInternalErrors_ = true;
            }
            setExprType(&node, DataType::Boolean);
            break;

        case BinaryOp::And:
        case BinaryOp::Or:
            if (leftType != DataType::Boolean && leftType != DataType::Unknown) {
                if (diagnostics_) diagnostics_->reportSemanticError(node.getLeft()->getSpan(), "Logical operand must be boolean.");
                hasInternalErrors_ = true;
            }
            if (rightType != DataType::Boolean && rightType != DataType::Unknown) {
                if (diagnostics_) diagnostics_->reportSemanticError(node.getRight()->getSpan(), "Logical operand must be boolean.");
                hasInternalErrors_ = true;
            }
            setExprType(&node, DataType::Boolean);
            break;
    }
}

void SemanticAnalyzer::visit(UnaryExpr& node) {
    DataType opType = DataType::Unknown;
    if (node.getOperand()) {
        node.getOperand()->accept(*this);
        opType = lastEvaluatedType_;
    }

    if (node.getOp() == UnaryOp::Not) {
        if (opType != DataType::Boolean && opType != DataType::Unknown) {
            if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), "'not' operator requires boolean operand.");
            hasInternalErrors_ = true;
        }
        setExprType(&node, DataType::Boolean);
    } else {
        if (opType != DataType::Integer && opType != DataType::Unknown) {
            if (diagnostics_) diagnostics_->reportSemanticError(node.getSpan(), "Unary arithmetic operator requires integer operand.");
            hasInternalErrors_ = true;
        }
        setExprType(&node, DataType::Integer);
    }
}

void SemanticAnalyzer::visit(SensoryConditionExpr& node) {
    switch (node.getSensoryType()) {
        case SensoryType::EnemyNearby:
        case SensoryType::HealthLow:
        case SensoryType::ObstacleAhead:
            setExprType(&node, DataType::Boolean);
            break;
        case SensoryType::DistanceToEnemy:
        case SensoryType::Health:
            setExprType(&node, DataType::Integer);
            break;
    }
}

} // namespace gamescript
