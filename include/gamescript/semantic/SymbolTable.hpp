#pragma once

//==============================================================================
// SymbolTable.hpp
//
// Defines symbols, scopes, and the symbol table used during semantic analysis
// to track declarations, types, and nested scopes.
//==============================================================================

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

namespace gamescript {

// Identifies the kind of symbol stored in the symbol table.
enum class SymbolKind {
    Variable,
    Function,
    Parameter
};

// Represents the data type associated with a symbol or expression.
enum class DataType {
    Integer,
    Boolean,
    String,
    Void,
    Unknown
};

// Converts a data type to its string representation.
std::string dataTypeToString(DataType type);

struct Symbol {
    std::string name;
    SymbolKind kind;
    DataType type = DataType::Unknown;
    SourceSpan span;
    int scopeLevel = 0;
    
    // Function specific
    std::vector<DataType> paramTypes;
    size_t paramCount = 0;
};

// Represents a single lexical scope and its declared symbols.
class Scope {
public:
    explicit Scope(std::shared_ptr<Scope> parent = nullptr, int level = 0)
        : parent_(std::move(parent)), level_(level) {}

    // Defines a symbol in the current scope.
    bool define(const Symbol& symbol) {
        if (symbols_.find(symbol.name) != symbols_.end()) {
            return false; // Already defined in current scope
        }
        symbols_[symbol.name] = symbol;
        return true;
    }

    // Resolves a symbol in the current scope or its parent scopes.
    std::optional<Symbol> resolve(const std::string& name) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return it->second;
        }
        if (parent_) {
            return parent_->resolve(name);
        }
        return std::nullopt;
    }

    // Resolves a symbol only within the current scope.
    std::optional<Symbol> resolveCurrent(const std::string& name) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::shared_ptr<Scope> getParent() const { return parent_; }
    int getLevel() const { return level_; }
    const std::unordered_map<std::string, Symbol>& getSymbols() const { return symbols_; }

private:
    std::shared_ptr<Scope> parent_;
    int level_ = 0;
    std::unordered_map<std::string, Symbol> symbols_;
};

// Maintains the hierarchy of scopes used during semantic analysis.
class SymbolTable {
public:
    SymbolTable();

    // Enters a new nested scope.
    void enterScope();

    // Exits the current scope and returns to its parent.
    void exitScope();

    // Defines a symbol in the current scope.
    bool define(const Symbol& symbol);

    // Resolves a symbol from the current scope and its parents.
    std::optional<Symbol> resolve(const std::string& name) const;

    // Resolves a symbol only in the current scope.
    std::optional<Symbol> resolveCurrent(const std::string& name) const;

    int getCurrentScopeLevel() const { return currentScope_->getLevel(); }

private:
    std::shared_ptr<Scope> currentScope_;
    std::shared_ptr<Scope> globalScope_;
};

} // namespace gamescript