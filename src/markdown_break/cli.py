import sys
import argparse
import json
from . import MarkdownChunker


def main():
    parser = argparse.ArgumentParser(
        prog="markdown-break",
        description="Split markdown into semantic chunks"
    )
    parser.add_argument(
        "input",
        nargs='?', 
        help="Path to markdown input file (or read from stdin if omitted)"
    )
    parser.add_argument(
        "--max-tokens",
        type=int,
        default=100,
        dest="max_tokens",
        help="Maximum tokens per chunk"
    )
    args = parser.parse_args()

    if args.input:
        with open(args.input, 'r', encoding='utf-8') as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    chunker = MarkdownChunker()
    chunker.set_text(text)
    chunks = chunker.create_chunks(args.max_tokens)
    json.dump(chunks, sys.stdout)


if __name__ == "__main__":
    main()
