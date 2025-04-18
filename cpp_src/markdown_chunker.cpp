#include "markdown_chunker.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <iomanip>
#include <deque>
#include <limits>
#include <functional>

#define PENALTY 15


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
            bps.push_back({i + 1, 10, "newline"});
        }
    }
}

static void findSpace(const std::string& text, std::vector<Breakpoint>& bps) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == ' ') {
            bps.push_back({i + 1, 12, "space"});
        }
    }
}

static void findSentenceEnds(const std::string& text, std::vector<Breakpoint>& bps) {
    std::size_t len = text.size();
    for (std::size_t i = 0; i + 1 < len; ++i) {
        if ((text[i] == '.' || text[i] == '?' || text[i] == ';' || text[i] == '!')
            && text[i + 1] == ' ') {
            bps.push_back({i + 2, 10, "sentence_end"});
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

    bps.push_back({0, 0, "start_of_text"});

    findHeadings(text, bps);
    findCodeBlockBoundary(text, bps);
    findParagraphBreaks(text, bps);
    findNewlines(text, bps);
    findSentenceEnds(text, bps);
    findSpace(text, bps);

    bps.push_back({text.size(), 0, "end_of_text"});

    std::sort(bps.begin(), bps.end(), compareBps);
    std::vector<Breakpoint> uniqueBps;
    for (const auto& bp : bps) {
        if (uniqueBps.empty() || bp.offset != uniqueBps.back().offset) {
            uniqueBps.push_back(bp);
        }
    }
    return uniqueBps;
}

#include <deque>
#include <limits>

void MarkdownChunker::createChunks(
    const std::string& text,
    const std::vector<Breakpoint>& breakpoints,
    int maxTokens,
    std::function<void(const std::string&, std::size_t, std::size_t)> printChunk
) {
    if (text.empty() || breakpoints.empty()) return;
    std::size_t N = breakpoints.size();

    // dp[i]: min total cost to reach breakpoint i
    // prev[i]: best predecessor index for i
    std::vector<int> dp(N, std::numeric_limits<int>::max());
    std::vector<int> prev(N, -1);

    // Monotonic deque of candidate indices j.
    // Invariant: dp[dq.front()] is minimal among all j in window.
    std::deque<std::size_t> dq;

    // Base case: start at breakpoints[0]
    dp[0] = 0;
    dq.push_back(0);

    // Build dp[1..N-1]
    for (std::size_t i = 1; i < N; ++i) {
        // 1) Evict any j that would exceed maxTokens if we chunk [j..i]
        while (!dq.empty()) {
            std::size_t j = dq.front();
            int needed = estimateTokens(
                breakpoints[i].offset - breakpoints[j].offset
            );
            if (needed <= maxTokens) break;
            dq.pop_front();
        }

        if (dq.empty()) {
            // fallback: break at i-1
            std::size_t j = i - 1;
            dp[i]     = dp[j] + breakpoints[i].priority + PENALTY;
            prev[i]   = static_cast<int>(j);
        
            // re‑prime the deque so future i's can use this forced break
            dq.clear();
            dq.push_back(i);
            continue;
        }

        // 2) Transition from the best j = dq.front()
        std::size_t bestJ = dq.front();
        dp[i] = dp[bestJ] + breakpoints[i].priority;
        prev[i] = static_cast<int>(bestJ);

        // 3) Insert i into dq, popping any worse-or-equal dp’s from the back
        while (!dq.empty() && dp[i] <= dp[dq.back()]) {
            dq.pop_back();
        }
        dq.push_back(i);
    }

    // Reconstruct the optimal path of breakpoints
    std::vector<std::size_t> path;
    for (int cur = static_cast<int>(N) - 1; cur >= 0; cur = prev[cur]) {
        path.push_back(cur);
        if (cur == 0) break;
    }
    std::reverse(path.begin(), path.end());

    // Emit chunks
    for (std::size_t k = 0; k + 1 < path.size(); ++k) {
        std::size_t startOff = breakpoints[path[k]].offset;
        std::size_t endOff   = breakpoints[path[k+1]].offset;
        printChunk(
            text.substr(startOff, endOff - startOff),
            startOff,
            endOff - startOff
        );
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
