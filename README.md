# Markdown Break

A Python package (with a C++ pybind11 extension) for breaking Markdown text into semantic chunks based on headers, code blocks, and other breakpoints.

## Features

- Identify semantic breakpoints in Markdown
- Split text into chunks with a maximum token limit
- Exposes C++ performance via a Python API
- Command-line entry point for quick usage

## Installation

```bash
pip install markdown-break
```

Or install from source:

```bash
git clone https://github.com/yourusername/markdown-break.git
cd markdown-break
pip install .
```

## Usage

### Python API

```python
from markdown_break import MarkdownChunker

text = "# Title\nSome content..."
chunker = MarkdownChunker()
chunker.set_text(text)
chunks = chunker.create_chunks(max_tokens=100)
print(chunks)
```

### Command Line

```bash
markdown-break input.md --max-tokens 200 > output_chunks.json
```

## Development

```bash
# Create a virtual environment
env=$(python3 -m venv venv) && source $env/bin/activate

# Install dev dependencies
pip install .[test]

# Run tests
pytest
```
