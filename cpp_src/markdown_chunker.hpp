#ifndef MARKDOWN_CHUNKER_HPP
#define MARKDOWN_CHUNKER_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>

struct Breakpoint {
    std::size_t offset;   // Character offset in the original text
    int priority;         // Priority (lower number = higher priority)
    std::string type;     // Type of breakpoint (e.g., "h1", "code_block", etc.)
};

class MarkdownChunker {
public:
    MarkdownChunker();

    // Load a new markdown string and compute breakpoints
    void setText(const std::string& text);
    // Create chunks based on the loaded text and breakpoints
    // Returns a vector of chunk strings
    std::vector<std::string> createChunks(int maxTokens) const;

public:
    // Print all breakpoints in a formatted table
    void printBreakpoints(std::ostream& os = std::cout) const;

private:
    std::string text_;
    std::vector<Breakpoint> breakpoints_;

    // Internal helper to split text based on breakpoints and maxTokens
    static void createChunks(
        const std::string& text,
        const std::vector<Breakpoint>& breakpoints,
        int maxTokens,
        std::function<void(const std::string&, std::size_t, std::size_t)> printChunk
    );
};

#endif // MARKDOWN_CHUNKER_HPP
