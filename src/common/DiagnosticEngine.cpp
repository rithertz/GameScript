#include "gamescript/common/DiagnosticEngine.hpp"
#include <sstream>
#include <iomanip>

namespace gamescript {

std::string DiagnosticEngine::getLineSnippet(size_t lineNumber) const {
    if (sourceText_.empty() || lineNumber == 0) return "";
    
    std::istringstream stream(sourceText_);
    std::string line;
    size_t currentLine = 1;
    while (std::getline(stream, line)) {
        if (currentLine == lineNumber) {
            return line;
        }
        currentLine++;
    }
    return "";
}

std::string DiagnosticEngine::formatDiagnostic(const Diagnostic& diag) const {
    std::ostringstream oss;
    
    // Header: [CATEGORY] filename:line:column: error: message
    oss << "[" << diag.categoryToString() << "] ";
    if (!diag.span.filename.empty()) {
        oss << diag.span.filename << ":";
    }
    oss << diag.span.start.line << ":" << diag.span.start.column << ": "
        << diag.levelToString() << ": " << diag.message << "\n";

    // Source context snippet
    std::string lineSnippet = getLineSnippet(diag.span.start.line);
    if (!lineSnippet.empty()) {
        std::string lineNumStr = std::to_string(diag.span.start.line);
        size_t padWidth = std::max(lineNumStr.length(), size_t{4});
        
        oss << std::string(padWidth + 2, ' ') << "|\n";
        oss << std::setw(padWidth) << lineNumStr << " | " << lineSnippet << "\n";
        
        oss << std::string(padWidth + 2, ' ') << "| ";
        size_t col = diag.span.start.column > 0 ? diag.span.start.column - 1 : 0;
        oss << std::string(col, ' ') << "^";
        
        // Underline length if multicharacter span on same line
        if (diag.span.end.line == diag.span.start.line && diag.span.end.column > diag.span.start.column) {
            size_t length = diag.span.end.column - diag.span.start.column;
            oss << std::string(length, '~');
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string DiagnosticEngine::formatAll() const {
    std::ostringstream oss;
    for (const auto& diag : diagnostics_) {
        oss << formatDiagnostic(diag) << "\n";
    }
    if (errorCount_ > 0 || warningCount_ > 0) {
        oss << errorCount_ << " error(s), " << warningCount_ << " warning(s) generated.\n";
    }
    return oss.str();
}

void DiagnosticEngine::print(std::ostream& os) const {
    os << formatAll();
}

} // namespace gamescript
