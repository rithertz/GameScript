#include "gamescript/semantic/SymbolTable.hpp"

namespace gamescript {

std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::Integer: return "Int";
        case DataType::Boolean: return "Bool";
        case DataType::String:  return "String";
        case DataType::Void:    return "Void";
        case DataType::Unknown: return "Unknown";
    }
    return "Unknown";
}

SymbolTable::SymbolTable() {
    globalScope_ = std::make_shared<Scope>(nullptr, 0);
    currentScope_ = globalScope_;
}

void SymbolTable::enterScope() {
    currentScope_ = std::make_shared<Scope>(currentScope_, currentScope_->getLevel() + 1);
}

void SymbolTable::exitScope() {
    if (currentScope_->getParent()) {
        currentScope_ = currentScope_->getParent();
    }
}

bool SymbolTable::define(const Symbol& symbol) {
    return currentScope_->define(symbol);
}

std::optional<Symbol> SymbolTable::resolve(const std::string& name) const {
    return currentScope_->resolve(name);
}

std::optional<Symbol> SymbolTable::resolveCurrent(const std::string& name) const {
    return currentScope_->resolveCurrent(name);
}

} // namespace gamescript
