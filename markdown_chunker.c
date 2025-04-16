#include "markdown_chunker.h"

// Initialize a breakpoint array
BreakpointArray* init_breakpoint_array() {
    BreakpointArray* array = (BreakpointArray*)malloc(sizeof(BreakpointArray));
    if (!array) return NULL;
    
    array->capacity = 100;  // Initial capacity
    array->count = 0;
    array->points = (Breakpoint*)malloc(array->capacity * sizeof(Breakpoint));
    
    if (!array->points) {
        free(array);
        return NULL;
    }
    
    return array;
}

// Free a breakpoint array
void free_breakpoint_array(BreakpointArray* array) {
    if (array) {
        if (array->points) {
            free(array->points);
        }
        free(array);
    }
}

// Add a breakpoint to the array
void add_breakpoint(BreakpointArray* array, long offset, int priority, const char* type) {
    if (!array) return;
    
    // Resize if needed
    if (array->count >= array->capacity) {
        array->capacity *= 2;
        Breakpoint* new_points = (Breakpoint*)realloc(array->points, array->capacity * sizeof(Breakpoint));
        if (!new_points) return;
        array->points = new_points;
    }
    
    // Add the new breakpoint
    array->points[array->count].offset = offset;
    array->points[array->count].priority = priority;
    strncpy(array->points[array->count].type, type, sizeof(array->points[array->count].type) - 1);
    array->points[array->count].type[sizeof(array->points[array->count].type) - 1] = '\0';  // Ensure null termination
    array->count++;
}

// Helper function to check if a position is at the start of a line
static bool is_start_of_line(const char* text, long pos) {
    return pos == 0 || text[pos-1] == '\n';
}

// Helper function to check for heading patterns
static void find_headings(const char* text, size_t text_len, BreakpointArray* array) {
    for (size_t i = 0; i < text_len; i++) {
        // Check for headings (# to ######)
        if (text[i] == '#' && is_start_of_line(text, i)) {
            // Count consecutive # characters to determine heading level
            int level = 0;
            size_t pos = i;
            while (pos < text_len && text[pos] == '#') {
                level++;
                pos++;
            }
            
            // Validate it's a proper heading (needs space after ###)
            if (level >= 1 && level <= 6 && pos < text_len && (text[pos] == ' ' || text[pos] == '\t')) {
                // For headings, add breakpoint BEFORE the heading (which is at the start of line)
                // Get position of previous newline or start of text
                long break_pos = i;
                if (break_pos > 0) {
                    break_pos = i; // We're at the start of line already
                }
                char type[20];
                sprintf(type, "h%d", level);
                add_breakpoint(array, break_pos, level, type);
            }
        }
    }
}

static void find_code_block_boundary(const char* text, size_t text_len, BreakpointArray* array) {
    bool in_code = false;
    for (size_t i = 0; i < text_len - 2; i++) {
        // Check for code fence start or end (``` or ~~~)
        if ((text[i] == '`' || text[i] == '~') &&
            text[i+1] == text[i] && text[i+2] == text[i]) {
            size_t fence_start = i;
            // Skip to end of fence line for any language hint
            size_t pos = i + 3;
            while (pos < text_len && text[pos] != '\n') {
                pos++;
            }
            if (!in_code) {
                // Code block start, add breakpoint at start of fence
                add_breakpoint(array, (long)fence_start, 7, "code_block");
                in_code = true;
            } else {
                // Code block end, add breakpoint after fence newline
                if (pos < text_len) {
                    add_breakpoint(array, (long)(pos + 1), 7, "code_block");
                }
                in_code = false;
            }
            // Advance past this fence line
            i = pos;
        }
    }
}

// Find paragraph breaks (double newlines)
static void find_paragraph_breaks(const char* text, size_t text_len, BreakpointArray* array) {
    for (size_t i = 0; i < text_len - 1; i++) {
        if (text[i] == '\n') {
            size_t pos = i + 1;
            // Skip whitespace
            while (pos < text_len && (text[pos] == ' ' || text[pos] == '\t')) {
                pos++;
            }
            
            if (pos < text_len && text[pos] == '\n') {
                // Found a paragraph break, add breakpoint AFTER the break
                add_breakpoint(array, pos + 1, 8, "para_break");
            }
        }
    }
}

// Find single newlines
static void find_newlines(const char* text, size_t text_len, BreakpointArray* array) {
    for (size_t i = 0; i < text_len; i++) {
        if (text[i] == '\n') {
            // Add breakpoint AFTER the newline
            add_breakpoint(array, i + 1, 9, "newline");
        }
    }
}

// Compare function for qsort
static int compare_breakpoints(const void* a, const void* b) {
    const Breakpoint* bp_a = (const Breakpoint*)a;
    const Breakpoint* bp_b = (const Breakpoint*)b;
    
    // Sort by offset first
    if (bp_a->offset < bp_b->offset) return -1;
    if (bp_a->offset > bp_b->offset) return 1;
    
    // If same offset, sort by priority (lower number = higher priority)
    return bp_a->priority - bp_b->priority;
}

// Find all breakpoints in markdown text
BreakpointArray* find_all_breakpoints(const char* text, size_t text_len) {
    BreakpointArray* array = init_breakpoint_array();
    if (!array) return NULL;
    
    // Find different types of breakpoints
    find_headings(text, text_len, array);
    find_code_block_boundary(text, text_len, array);
    find_paragraph_breaks(text, text_len, array);
    find_newlines(text, text_len, array);
    
    // Sort breakpoints by offset and priority
    qsort(array->points, array->count, sizeof(Breakpoint), compare_breakpoints);

    // Remove duplicates at same offset, keep highest priority breakpoint
    {
        size_t write_idx = 0;
        for (size_t i = 0; i < array->count; i++) {
            if (i == 0 || array->points[i].offset != array->points[i-1].offset) {
                array->points[write_idx++] = array->points[i];
            }
        }
        array->count = write_idx;
    }

    return array;
}

// Print breakpoints for debugging
void print_breakpoints(const BreakpointArray* array) {
    if (!array) return;
    
    printf("Found %zu breakpoints:\n", array->count);
    printf("%-10s | %-10s | %-10s\n", "Offset", "Priority", "Type");
    printf("-----------------------------------------------\n");
    
    for (size_t i = 0; i < array->count; i++) {
        printf("%-10ld | %-10d | %-10s\n", 
               array->points[i].offset, 
               array->points[i].priority, 
               array->points[i].type);
    }
}

// Simple token counting approximation for the example
// Fixed unused parameter warning by using both parameters
static int estimate_tokens(const char* text, size_t len) {
    (void)text; // Explicitly mark as unused while keeping the parameter
    // Simple approximation: ~4 chars per token
    return (int)(len / 4) + 1;
}

// Create chunks based on breakpoints and maximum token counts
void create_chunks(const char* text, size_t text_len, const BreakpointArray* breakpoints, 
                   int max_tokens, void (*print_chunk)(const char*, size_t, size_t)) {
    if (!text || !breakpoints || breakpoints->count == 0) return;
    
    size_t chunk_start = 0;
    size_t last_valid_break = 0;
    // Removed unused chunk_id variable
    
    while (chunk_start < text_len) {
        // Find the furthest breakpoint that keeps us under the token limit
        size_t best_break = text_len;  // Default to end of text
        bool found_valid_break = false;
        
        for (size_t i = 0; i < breakpoints->count; i++) {
            // Fix sign comparison warning by casting to same type
            size_t break_pos = (size_t)breakpoints->points[i].offset;
            
            // Skip breakpoints before our current position
            if (break_pos <= chunk_start) continue;
            
            // Check if we can fit text up to this breakpoint
            size_t segment_len = break_pos - chunk_start;
            int tokens = estimate_tokens(&text[chunk_start], segment_len);
            
            if (tokens <= max_tokens) {
                // This is a valid breaking point
                best_break = break_pos;
                found_valid_break = true;
                last_valid_break = break_pos;
            } else {
                // We've gone past our token limit, use the last valid break
                if (found_valid_break) {
                    break;
                }
                // Fix sign comparison warning by casting
                else if (i == 0 || (size_t)break_pos < best_break) {
                    best_break = break_pos;
                    found_valid_break = true;
                    break;
                }
            }
        }
        
        // If we didn't find any valid break, use the end of text or force a break
        if (!found_valid_break) {
            if (last_valid_break > chunk_start) {
                best_break = last_valid_break;
            } else {
                // Forced break - take a small chunk to ensure progress
                best_break = chunk_start + 100;
                if (best_break > text_len) best_break = text_len;
            }
        }
        
        // Create the chunk
        size_t chunk_size = best_break - chunk_start;
        print_chunk(&text[chunk_start], chunk_start, chunk_size);
        
        // Move to next chunk
        chunk_start = best_break;
    }
}
