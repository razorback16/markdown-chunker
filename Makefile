CC = gcc
CFLAGS = -Wall -Wextra -g

all: markdown_chunker test_generator

markdown_chunker: markdown_chunker.c main.c
	$(CC) $(CFLAGS) -o markdown_chunker markdown_chunker.c main.c

test_generator: generate_test_markdown.c
	$(CC) $(CFLAGS) -o test_generator generate_test_markdown.c

test: markdown_chunker test_generator
	./test_generator
	./markdown_chunker

clean:
	rm -f markdown_chunker test_generator test_markdown.md