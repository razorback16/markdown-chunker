#include "markdown_chunker.hpp"
#include <iostream>
#include <string>

int main() {
    const std::string markdown_text = R"md(
# Main Heading

This is a paragraph with some text.
This is on the same paragraph.

This is another paragraph with a **bold** and *italic* text.

## Subheading Level 2

Here's a list:
- Item 1
- Item 2
- Item 3

### Subheading Level 3

```python
def hello_world():
    print("Hello, World!")
```

#### Subheading Level 4

Here's a table:

| Column 1 | Column 2 | Column 3 |
| -------- | -------- | -------- |
| Value 1  | Value 2  | Value 3  |
| Value 4  | Value 5  | Value 6  |

##### Subheading Level 5

This is a [link to Google](https://www.google.com).

###### Subheading Level 6

Here's an image: ![Alt text](image.jpg)

> This is a blockquote
> With multiple lines

```javascript
function calculateSum(a, b) {
    return a + b;
}
```

This is the end of the test document.
)md";

    std::cout << "Markdown text length: " << markdown_text.size() << " characters\n";

    // Initialize the chunker and load text
    MarkdownChunker chunker;
    chunker.setText(markdown_text);
    chunker.printBreakpoints();

    std::cout << "\nCreating chunks with maximum 100 tokens per chunk using member function...\n";
    auto chunks = chunker.createChunks(100);
    for (std::size_t i = 0; i < chunks.size(); ++i) {
        std::cout << "\n--- CHUNK " << i 
                  << " (size " << chunks[i].size() << ") ---\n";
        std::cout << chunks[i] << "\n";
        std::cout << "--- END CHUNK ---\n";
    }

    return 0;
}
