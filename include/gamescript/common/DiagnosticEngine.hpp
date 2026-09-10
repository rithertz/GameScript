#pragma once

//==============================================================================
// Diagnostic.hpp
//
// Defines diagnostic types and the DiagnosticEngine used to report, store,
// format, and display compiler errors, warnings, and notes.
//==============================================================================

#include "gamescript/common/SourceLocation.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace gamescript {

// Represents the severity level of a diagnostic message.
enum class DiagnosticLevel {
    Error,
    Warning,
    Note
};

// Represents the compiler stage where a diagnostic originated.
enum class DiagnosticCategory {
    Lexical,
    Syntax,
    Semantic,
    Codegen,
    Runtime
};

// Stores information about a single compiler diagnostic.
struct Diagnostic {
    DiagnosticLevel level;
    DiagnosticCategory category;
    SourceSpan span;
    std::string message;

    // Converts the diagnostic category to a human-readable string.
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

    // Converts the diagnostic level to a human-readable string.
    std::string levelToString() const {
        switch (level) {
            case DiagnosticLevel::Error:   return "error";
            case DiagnosticLevel::Warning: return "warning";
            case DiagnosticLevel::Note:    return "note";
        }
        return "info";
    }
};

// Manages compiler diagnostics and tracks errors and warnings produced during compilation.
class DiagnosticEngine {
public:
    explicit DiagnosticEngine(std::string sourceText = "", std::string filename = "")
        : sourceText_(std::move(sourceText)), filename_(std::move(filename)) {}

    // Sets the source text and filename associated with subsequent diagnostics.
    void setSource(std::string sourceText, std::string filename = "") {
        sourceText_ = std::move(sourceText);
        filename_ = std::move(filename);
    }

    // Records a diagnostic and updates the corresponding error or warning count.
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

    // Reports a lexical error.
    void reportLexicalError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Lexical, span, message);
    }

    // Reports a syntax error.
    void reportSyntaxError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Syntax, span, message);
    }

    // Reports a semantic error.
    void reportSemanticError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Semantic, span, message);
    }

    // Reports a code generation error.
    void reportCodegenError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Codegen, span, message);
    }

    // Reports a runtime error.
    void reportRuntimeError(SourceSpan span, const std::string& message) {
        report(DiagnosticLevel::Error, DiagnosticCategory::Runtime, span, message);
    }

    bool hasErrors() const { return errorCount_ > 0; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics_; }

    // Clears all stored diagnostics and resets the error and warning counts.
    void clear() {
        diagnostics_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
    }

    // Formats a single diagnostic for display.
    std::string formatDiagnostic(const Diagnostic& diag) const;

    // Formats all stored diagnostics for display.
    std::string formatAll() const;

    // Prints all stored diagnostics to the specified output stream.
    void print(std::ostream& os = std::cerr) const;

private:
    // Extracts the source line associated with a diagnostic.
    std::string getLineSnippet(size_t lineNumber) const;

    std::string sourceText_;
    std::string filename_;
    std::vector<Diagnostic> diagnostics_;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
};

} // namespace gamescript