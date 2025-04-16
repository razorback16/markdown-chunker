#include "markdown_chunker.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <iomanip>


static bool isStartOfLine(const std::string& text, std::size_t pos) {
    return pos == 0 || text[pos - 1] == '\n';
}

static void findHeadings(const std::string& text, std::vector<Breakpoint>& bps) {
    std::size_t len = text.size();
    for (std::size_t i = 0; i < len; ++i) {
        if (text[i] == '#' && isStartOfLine(text, i)) {
            int level = 0;
            std::size_t p = i;
            while (p < len && text[p] == '#') {
                ++level; ++p;
            }
            if (level >= 1 && level <= 6 && p < len && (text[p] == ' ' || text[p] == '\t')) {
                bps.push_back({i, level, "h" + std::to_string(level)});
            }
        }
    }
}

static void findCodeBlockBoundary(const std::string& text, std::vector<Breakpoint>& bps) {
    bool inCode = false;
    std::size_t len = text.size();
    for (std::size_t i = 0; i + 2 < len; ++i) {
        char c = text[i];
        if ((c == '`' || c == '~') && text[i+1] == c && text[i+2] == c) {
            std::size_t fenceStart = i;
            std::size_t p = i + 3;
            while (p < len && text[p] != '\n') ++p;
            if (!inCode) {
                bps.push_back({fenceStart, 7, "code_block"});
                inCode = true;
            } else {
                if (p < len) {
                    bps.push_back({p + 1, 7, "code_block"});
                }
                inCode = false;
            }
            i = p;
        }
    }
}

static void findParagraphBreaks(const std::string& text, std::vector<Breakpoint>& bps) {
    std::size_t len = text.size();
    for (std::size_t i = 0; i + 1 < len; ++i) {
        if (text[i] == '\n') {
            std::size_t p = i + 1;
            while (p < len && (text[p] == ' ' || text[p] == '\t')) ++p;
            if (p < len && text[p] == '\n') {
                bps.push_back({p + 1, 8, "para_break"});
            }
        }
    }
}

static void findNewlines(const std::string& text, std::vector<Breakpoint>& bps) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\n') {
            bps.push_back({i + 1, 9, "newline"});
        }
    }
}

static int estimateTokens(std::size_t len) {
    return static_cast<int>(len / 4) + 1;
}

static bool compareBps(const Breakpoint& a, const Breakpoint& b) {
    if (a.offset != b.offset) return a.offset < b.offset;
    return a.priority < b.priority;
}

static std::vector<Breakpoint> findAllBreakpoints(const std::string& text) {
    std::vector<Breakpoint> bps;
    findHeadings(text, bps);
    findCodeBlockBoundary(text, bps);
    findParagraphBreaks(text, bps);
    findNewlines(text, bps);
    std::sort(bps.begin(), bps.end(), compareBps);
    std::vector<Breakpoint> uniqueBps;
    for (const auto& bp : bps) {
        if (uniqueBps.empty() || bp.offset != uniqueBps.back().offset) {
            uniqueBps.push_back(bp);
        }
    }
    return uniqueBps;
}

void MarkdownChunker::createChunks(
    const std::string& text,
    const std::vector<Breakpoint>& breakpoints,
    int maxTokens,
    std::function<void(const std::string&, std::size_t, std::size_t)> printChunk
) {
    if (text.empty() || breakpoints.empty()) return;
    std::size_t textLen = text.size();
    std::size_t chunkStart = 0;
    std::size_t lastValid = 0;
    while (chunkStart < textLen) {
        std::size_t bestBreak = textLen;
        bool found = false;
        for (const auto& bp : breakpoints) {
            if (bp.offset <= chunkStart) continue;
            std::size_t segLen = bp.offset - chunkStart;
            int tokens = estimateTokens(segLen);
            if (tokens <= maxTokens) {
                bestBreak = bp.offset;
                found = true;
                lastValid = bp.offset;
            } else if (found) {
                break;
            } else {
                bestBreak = bp.offset;
                found = true;
            }
        }
        if (!found) {
            if (lastValid > chunkStart) {
                bestBreak = lastValid;
            } else {
                bestBreak = std::min(chunkStart + std::size_t(100), textLen);
            }
        }
        std::size_t size = bestBreak - chunkStart;
        printChunk(text.substr(chunkStart, size), chunkStart, size);
        chunkStart = bestBreak;
    }
}

// -- Member function implementations --

MarkdownChunker::MarkdownChunker()
    : text_(), breakpoints_() {}

void MarkdownChunker::setText(const std::string& text) {
    text_ = text;
    breakpoints_ = findAllBreakpoints(text_);
}

std::vector<std::string> MarkdownChunker::createChunks(int maxTokens) const {
    std::vector<std::string> chunks;
    createChunks(text_, breakpoints_, maxTokens,
        [&chunks](const std::string& chunkText, std::size_t, std::size_t) {
            chunks.push_back(chunkText);
        }
    );
    return chunks;
}

void MarkdownChunker::printBreakpoints(std::ostream& os) const {
    os << "Markdown text length: " << text_.size() << " characters\n";
    os << "Found " << breakpoints_.size() << " breakpoints:\n";
    os << "Offset     | Priority   | Type\n";
    os << "-----------------------------------------------\n";
    for (const auto& bp : breakpoints_) {
        os << std::left
           << std::setw(10) << bp.offset << " | "
           << std::setw(10) << bp.priority << " | "
           << bp.type << "\n";
    }
}
