#pragma once

#include <string>
#include <string_view>
#include <sstream>

namespace gamescript {

struct SourceLocation {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;

    bool operator==(const SourceLocation& other) const {
        return line == other.line && column == other.column && offset == other.offset;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << line << ":" << column;
        return oss.str();
    }
};

struct SourceSpan {
    SourceLocation start;
    SourceLocation end;
    std::string filename;

    SourceSpan() = default;
    SourceSpan(SourceLocation start, SourceLocation end, std::string filename = "")
        : start(start), end(end), filename(std::move(filename)) {}

    static SourceSpan single(SourceLocation loc, std::string filename = "") {
        return SourceSpan(loc, loc, std::move(filename));
    }

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
