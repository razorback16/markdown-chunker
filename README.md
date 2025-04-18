# markdown-chunker

A Python package (with a C++ pybind11 extension) for breaking Markdown text into semantic chunks based on headers, code blocks, and other breakpoints.

## Features

- Identify semantic breakpoints in Markdown
- Split text into chunks with a maximum token limit
- Exposes C++ performance via a Python API
- Command-line entry point for quick usage

## Installation

```bash
pip install markdown-chunker
```

Or install from source:

```bash
git clone https://github.com/yourusername/markdown-chunker.git
cd markdown-chunker
pip install .
```

## Usage

### Python API

```python
from markdown_chunker import MarkdownChunker

text = "# Title\nSome content..."
chunker = MarkdownChunker()
chunker.set_text(text)
chunks = chunker.create_chunks(max_tokens=100)
print(chunks)
```

### Command Line

```bash
markdown-chunker input.md --max-tokens 200 > output_chunks.json
```

## Chunking Algorithm

The chunking algorithm works as follows:

1. **Breakpoint Detection**: The algorithm scans the markdown text to identify semantic breakpoints, including:
   - Headings (h1–h6)
   - Code block boundaries (fenced code blocks)
   - Paragraph breaks (double newlines)
   - Newlines
   - Sentence ends (period, question mark, exclamation, or semicolon followed by a space)
   - Spaces
2. **Breakpoint Prioritization**: Each breakpoint is assigned a priority (lower is better), with headings and code blocks having higher priority (lower number) than spaces or newlines.
3. **Dynamic Programming Optimization**: The algorithm uses dynamic programming to select an optimal set of breakpoints so that:
   - Each chunk does not exceed a specified maximum token limit (tokens are estimated as roughly one per four characters).
   - The sum of breakpoint priorities is minimized, and forced breaks (when no suitable breakpoint is found within the token limit) are penalized.
4. **Chunk Creation**: Chunks are created between the selected breakpoints, resulting in semantically meaningful and size-constrained segments of the original markdown text.

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Development

```bash
# Create a virtual environment
env=$(python3 -m venv venv) && source $env/bin/activate

# Install dev dependencies
pip install .[test]

# Run tests
pytest
```
