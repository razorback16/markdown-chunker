#include "markdown_chunker.h"

// Function to print a chunk for testing
void print_test_chunk(const char* chunk_text, size_t offset, size_t size) {
    printf("\n--- CHUNK at offset %zu, size %zu ---\n", offset, size);
    printf("%.*s\n", (int)size, chunk_text);
    printf("--- END CHUNK ---\n");
}

int main() {
    // Test markdown content with various features to test
    const char* markdown_text = 
        "# Main Heading\n\n"
        "This is a paragraph with some text.\n"
        "This is on the same paragraph.\n\n"
        "This is another paragraph with a **bold** and *italic* text.\n\n"
        "## Subheading Level 2\n\n"
        "Here's a list:\n"
        "- Item 1\n"
        "- Item 2\n"
        "- Item 3\n\n"
        "### Subheading Level 3\n\n"
        "```python\n"
        "def hello_world():\n"
        "    print(\"Hello, World!\")\n"
        "```\n\n"
        "#### Subheading Level 4\n\n"
        "Here's a table:\n\n"
        "| Column 1 | Column 2 | Column 3 |\n"
        "| -------- | -------- | -------- |\n"
        "| Value 1  | Value 2  | Value 3  |\n"
        "| Value 4  | Value 5  | Value 6  |\n\n"
        "##### Subheading Level 5\n\n"
        "This is a [link to Google](https://www.google.com).\n\n"
        "###### Subheading Level 6\n\n"
        "Here's an image: ![Alt text](image.jpg)\n\n"
        "> This is a blockquote\n"
        "> With multiple lines\n\n"
        "```javascript\n"
        "function calculateSum(a, b) {\n"
        "    return a + b;\n"
        "}\n"
        "```\n\n"
        "This is the end of the test document.\n";

    size_t text_len = strlen(markdown_text);
    printf("Markdown text length: %zu characters\n", text_len);
    
    // Find all breakpoints
    BreakpointArray* breakpoints = find_all_breakpoints(markdown_text, text_len);
    
    // Print the breakpoints
    print_breakpoints(breakpoints);
    
    // Create chunks with a maximum of 100 tokens per chunk
    printf("\nCreating chunks with maximum 100 tokens per chunk...\n");
    create_chunks(markdown_text, text_len, breakpoints, 100, print_test_chunk);
    
    // Free resources
    free_breakpoint_array(breakpoints);
    
    return 0;
}