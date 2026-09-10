#pragma once

//==============================================================================
// SourceLocation.hpp
//
// Defines source code location and span structures used by the GameScript
// compiler to track positions and ranges within source files.
//==============================================================================

#include <string>
#include <string_view>
#include <sstream>

namespace gamescript {

// Represents a single position in the source code.
struct SourceLocation {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;

    // Compares two source locations for equality.
    bool operator==(const SourceLocation& other) const {
        return line == other.line && column == other.column && offset == other.offset;
    }

    // Returns the location in line:column format.
    std::string toString() const {
        std::ostringstream oss;
        oss << line << ":" << column;
        return oss.str();
    }
};

// Represents a range of source code between two locations.
struct SourceSpan {
    SourceLocation start;
    SourceLocation end;
    std::string filename;

    SourceSpan() = default;
    SourceSpan(SourceLocation start, SourceLocation end, std::string filename = "")
        : start(start), end(end), filename(std::move(filename)) {}

    // Creates a span representing a single source location.
    static SourceSpan single(SourceLocation loc, std::string filename = "") {
        return SourceSpan(loc, loc, std::move(filename));
    }

    // Returns the starting location in filename:line:column format when a filename is available.
    std::string toString() const {
        std::ostringstream oss;
        if (!filename.empty()) {
            oss << filename << ":";
        }
        oss << start.line << ":" << start.column;
        return oss.str();
    }
};

} // namespace gamescript