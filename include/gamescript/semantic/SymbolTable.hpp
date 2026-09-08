#pragma once

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

namespace gamescript {

enum class SymbolKind {
    Variable,
    Function,
    Parameter
};

enum class DataType {
    Integer,
    Boolean,
    String,
    Void,
    Unknown
};

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

class Scope {
public:
    explicit Scope(std::shared_ptr<Scope> parent = nullptr, int level = 0)
        : parent_(std::move(parent)), level_(level) {}

    bool define(const Symbol& symbol) {
        if (symbols_.find(symbol.name) != symbols_.end()) {
            return false; // Already defined in current scope
        }
        symbols_[symbol.name] = symbol;
        return true;
    }

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

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool define(const Symbol& symbol);
    std::optional<Symbol> resolve(const std::string& name) const;
    std::optional<Symbol> resolveCurrent(const std::string& name) const;

    int getCurrentScopeLevel() const { return currentScope_->getLevel(); }

private:
    std::shared_ptr<Scope> currentScope_;
    std::shared_ptr<Scope> globalScope_;
};

} // namespace gamescript
