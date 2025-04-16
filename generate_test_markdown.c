#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Helper to write a random paragraph
void write_random_paragraph(FILE* fp, int sentences) {
    const char* sentence_starters[] = {
        "The quick brown fox ", "A gentle breeze ", "In the distant hills ",
        "Scientists recently discovered ", "According to recent studies ",
        "Experts agree that ", "Contrary to popular belief ", "Historical records show "
    };
    
    const char* sentence_middles[] = {
        "jumps over ", "runs past ", "carefully examines ", "completely ignores ",
        "thoroughly investigates ", "rapidly transforms ", "quietly observes ",
        "gently touches "
    };
    
    const char* sentence_ends[] = {
        "the lazy dog.", "the tall mountain.", "the deep blue sea.",
        "the ancient ruins.", "the modern city.", "the quantum computer.",
        "the distant galaxy.", "the microscopic particles."
    };
    
    for (int i = 0; i < sentences; i++) {
        // Select random parts
        int start = rand() % (sizeof(sentence_starters) / sizeof(sentence_starters[0]));
        int middle = rand() % (sizeof(sentence_middles) / sizeof(sentence_middles[0]));
        int end = rand() % (sizeof(sentence_ends) / sizeof(sentence_ends[0]));
        
        // Write sentence
        fprintf(fp, "%s%s%s ", 
                sentence_starters[start], 
                sentence_middles[middle], 
                sentence_ends[end]);
        
        // Random formatting
        if (rand() % 10 == 0) {
            fprintf(fp, "**This is bold text.** ");
        }
        if (rand() % 12 == 0) {
            fprintf(fp, "*This is italic text.* ");
        }
        if (rand() % 15 == 0) {
            fprintf(fp, "[This is a link](https://example.com) ");
        }
    }
    fprintf(fp, "\n\n");
}

// Write code block
void write_code_block(FILE* fp, const char* language) {
    fprintf(fp, "```%s\n", language);
    
    if (strcmp(language, "python") == 0) {
        fprintf(fp, "def example_function(param1, param2):\n");
        fprintf(fp, "    \"\"\"This is a docstring for the example function.\"\"\"\n");
        fprintf(fp, "    result = param1 + param2\n");
        fprintf(fp, "    return result\n\n");
        fprintf(fp, "class ExampleClass:\n");
        fprintf(fp, "    def __init__(self, name):\n");
        fprintf(fp, "        self.name = name\n");
        fprintf(fp, "        \n");
        fprintf(fp, "    def greet(self):\n");
        fprintf(fp, "        return f\"Hello, {self.name}!\"\n");
    } else if (strcmp(language, "javascript") == 0) {
        fprintf(fp, "function calculateSum(a, b) {\n");
        fprintf(fp, "    // Add two numbers together\n");
        fprintf(fp, "    return a + b;\n");
        fprintf(fp, "}\n\n");
        fprintf(fp, "const person = {\n");
        fprintf(fp, "    name: 'John',\n");
        fprintf(fp, "    age: 30,\n");
        fprintf(fp, "    greet: function() {\n");
        fprintf(fp, "        console.log(`Hello, ${this.name}!`);\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "};\n");
    } else if (strcmp(language, "c") == 0) {
        fprintf(fp, "#include <stdio.h>\n\n");
        fprintf(fp, "int main() {\n");
        fprintf(fp, "    printf(\"Hello, World!\\n\");\n");
        fprintf(fp, "    return 0;\n");
        fprintf(fp, "}\n");
    }
    
    fprintf(fp, "```\n\n");
}

// Write a table
void write_table(FILE* fp, int rows, int cols) {
    // Header
    for (int c = 0; c < cols; c++) {
        fprintf(fp, "| Column %d ", c+1);
    }
    fprintf(fp, "|\n");
    
    // Separator
    for (int c = 0; c < cols; c++) {
        fprintf(fp, "| -------- ");
    }
    fprintf(fp, "|\n");
    
    // Data rows
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fprintf(fp, "| Data %d-%d ", r+1, c+1);
        }
        fprintf(fp, "|\n");
    }
    fprintf(fp, "\n");
}

int main() {
    srand(time(NULL));
    FILE* fp = fopen("test_markdown.md", "w");
    
    if (!fp) {
        printf("Error opening file for writing\n");
        return 1;
    }
    
    // Write a comprehensive test document
    fprintf(fp, "# Comprehensive Markdown Test Document\n\n");
    write_random_paragraph(fp, 3);
    
    fprintf(fp, "## Headings and Subheadings\n\n");
    write_random_paragraph(fp, 2);
    
    fprintf(fp, "### Level 3 Heading\n\n");
    write_random_paragraph(fp, 2);
    
    fprintf(fp, "#### Level 4 Heading with *italic* and **bold**\n\n");
    write_random_paragraph(fp, 1);
    
    fprintf(fp, "##### Level 5 Heading\n\n");
    write_random_paragraph(fp, 1);
    
    fprintf(fp, "###### Level 6 Heading\n\n");
    write_random_paragraph(fp, 1);
    
    fprintf(fp, "## Code Blocks\n\n");
    fprintf(fp, "Here are some example code blocks in different languages:\n\n");
    write_code_block(fp, "python");
    write_random_paragraph(fp, 1);
    write_code_block(fp, "javascript");
    write_random_paragraph(fp, 1);
    write_code_block(fp, "c");
    
    fprintf(fp, "## Lists\n\n");
    fprintf(fp, "### Unordered List\n\n");
    fprintf(fp, "- Item 1\n");
    fprintf(fp, "  - Nested item 1.1\n");
    fprintf(fp, "  - Nested item 1.2\n");
    fprintf(fp, "- Item 2\n");
    fprintf(fp, "- Item 3\n\n");
    
    fprintf(fp, "### Ordered List\n\n");
    fprintf(fp, "1. First item\n");
    fprintf(fp, "   1. Nested first item\n");
    fprintf(fp, "   2. Nested second item\n");
    fprintf(fp, "2. Second item\n");
    fprintf(fp, "3. Third item\n\n");
    
    fprintf(fp, "## Tables\n\n");
    fprintf(fp, "Tables are useful for organizing data:\n\n");
    write_table(fp, 3, 4);
    
    fprintf(fp, "## Blockquotes\n\n");
    fprintf(fp, "> This is a blockquote.\n");
    fprintf(fp, ">\n");
    fprintf(fp, "> It can span multiple lines and can contain *formatted* text.\n\n");
    
    fprintf(fp, "Nested blockquotes:\n\n");
    fprintf(fp, "> Main quote\n");
    fprintf(fp, ">\n");
    fprintf(fp, "> > Nested quote\n");
    fprintf(fp, ">\n");
    fprintf(fp, "> Main quote continues\n\n");
    
    fprintf(fp, "## Horizontal Rules\n\n");
    fprintf(fp, "---\n\n");
    write_random_paragraph(fp, 1);
    fprintf(fp, "***\n\n");
    write_random_paragraph(fp, 1);
    fprintf(fp, "___\n\n");
    
    fprintf(fp, "## Links and Images\n\n");
    fprintf(fp, "- [Link to Google](https://www.google.com)\n");
    fprintf(fp, "- [Link with title](https://www.example.com \"Example Title\")\n");
    fprintf(fp, "- <https://www.example.org> (Autolink)\n\n");
    
    fprintf(fp, "![Example Image](https://via.placeholder.com/150)\n\n");
    fprintf(fp, "![Image with title](https://via.placeholder.com/300 \"Example Image Title\")\n\n");
    
    fprintf(fp, "## Special Characters and Escaping\n\n");
    fprintf(fp, "Escaping special characters: \\* \\` \\\\ \\_ \\# \\+ \\- \\. \\! \\[ \\]\n\n");
    
    fprintf(fp, "## Mixed Content for Edge Cases\n\n");
    fprintf(fp, "1. Item with `inline code` and **bold** text\n");
    fprintf(fp, "2. Item with a [link](https://example.com) and *italic* text\n\n");
    
    fprintf(fp, "Code inside list:\n\n");
    fprintf(fp, "- Item 1\n");
    fprintf(fp, "  ```python\n");
    fprintf(fp, "  def inside_list():\n");
    fprintf(fp, "      return \"Code inside a list item\"\n");
    fprintf(fp, "  ```\n");
    fprintf(fp, "- Item 2\n\n");
    
    fprintf(fp, "## The End\n\n");
    fprintf(fp, "This concludes our comprehensive markdown test document.\n");
    
    fclose(fp);
    printf("Test markdown file 'test_markdown.md' has been generated.\n");
    
    return 0;
}