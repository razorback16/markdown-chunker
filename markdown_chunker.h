#ifndef MARKDOWN_CHUNKER_H
#define MARKDOWN_CHUNKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Structure to represent a breakpoint
typedef struct {
    long offset;     // Character offset in the original text
    int priority;    // Priority (lower number = higher priority)
    char type[20];   // Type of breakpoint (e.g., "h1", "code_end", etc.)
} Breakpoint;

// Structure to hold an array of breakpoints
typedef struct {
    Breakpoint* points;
    size_t count;
    size_t capacity;
} BreakpointArray;

// Initialize a breakpoint array
BreakpointArray* init_breakpoint_array();

// Free a breakpoint array
void free_breakpoint_array(BreakpointArray* array);

// Add a breakpoint to the array
void add_breakpoint(BreakpointArray* array, long offset, int priority, const char* type);

// Find all breakpoints in a markdown string
BreakpointArray* find_all_breakpoints(const char* text, size_t text_len);

// Print breakpoints for debugging
void print_breakpoints(const BreakpointArray* array);

// Create chunks based on breakpoints and maximum token counts
void create_chunks(const char* text, size_t text_len, const BreakpointArray* breakpoints, 
                   int max_tokens, void (*print_chunk)(const char*, size_t, size_t));

#endif // MARKDOWN_CHUNKER_H