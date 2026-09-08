#pragma once

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace gamescript {

enum class DiagnosticLevel {
    Error,
    Warning,
    Note
};

enum class DiagnosticCategory {
    Lexical,
    Syntax,
    Semantic,
    Codegen,
    Runtime
};

struct Diagnostic {
    DiagnosticLevel level;
    DiagnosticCategory category;
    SourceSpan span;
    std::string message;

    std::string categoryToString() const {
        switch (category) {
            case DiagnosticCategory::Lexical:  return "LEXICAL ERROR";
            case DiagnosticCategory::Syntax:   return "SYNTAX ERROR";
            case DiagnosticCategory::Semantic: return "SEMANTIC ERROR";
            case DiagnosticCategory::Codegen:  return "CODEGEN ERROR";
            case DiagnosticCategory::Runtime:  return "RUNTIME ERROR";
        }
        return "ERROR";
    }

    std::string levelToString() const {
        switch (level) {
            case DiagnosticLevel::Error:   return "error";
            case DiagnosticLevel::Warning: return "warning";
            case DiagnosticLevel::Note:    return "note";
        }
        return "info";
    }
};

class DiagnosticEngine {
public:
    explicit DiagnosticEngine(std::string sourceText = "", std::string filename = "")
        : sourceText_(std::move(sourceText)), filename_(std::move(filename)) {}

    void setSource(std::string sourceText, std::string filename = "") {
        sourceText_ = std::move(sourceText);
        filename_ = std::move(filename);
    }

    void report(DiagnosticLevel level, DiagnosticCategory category, SourceSpan span, const std::string& message) {
        if (span.filename.empty()) {
            span.filename = filename_;
        }
        diagnostics_.push_back({level, category, span, message});
        if (level == DiagnosticLevel::Error) {
            errorCount_++;
        } else if (level == DiagnosticLevel::Warning) {
            warningCount_++;
        }
    }

    void reportLexicalError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Lexical, span, message);
    }

    void reportSyntaxError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Syntax, span, message);
    }

    void reportSemanticError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Semantic, span, message);
    }

    void reportCodegenError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Codegen, span, message);
    }

    void reportRuntimeError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Runtime, span, message);
    }

    bool hasErrors() const { return errorCount_ > 0; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics_; }
    void clear() {
        diagnostics_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
    }

    std::string formatDiagnostic(const Diagnostic& diag) const;
    std::string formatAll() const;
    void print(std::ostream& os = std::cerr) const;

private:
    std::string getLineSnippet(size_t lineNumber) const;

    std::string sourceText_;
    std::string filename_;
    std::vector<Diagnostic> diagnostics_;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
};

} // namespace gamescript
