CC = gcc
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g
CFLAGS = -Wall -Wextra -std=c11 -g

all: markdown_chunker test_generator

markdown_chunker: markdown_chunker.cpp main.cpp markdown_chunker.hpp
	$(CXX) $(CXXFLAGS) -o markdown_chunker markdown_chunker.cpp main.cpp

test_generator: generate_test_markdown.c
	$(CC) $(CFLAGS) -o test_generator generate_test_markdown.c

test: markdown_chunker test_generator
	./test_generator
	./markdown_chunker

clean:
	rm -f markdown_chunker test_generator test_markdown.md
