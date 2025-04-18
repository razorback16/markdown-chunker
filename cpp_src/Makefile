CC = gcc
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++14 -g -fPIC -frtti -fexceptions
CFLAGS = -Wall -Wextra -std=c14 -g

# Python configuration for pybind11
PYTHON_INCLUDES = $(shell python3 -m pybind11 --includes)
PYTHON_EXT_SUFFIX = $(shell python3-config --extension-suffix)

# Linker flags for Python (use dynamic lookup on macOS)
PYTHON_LDFLAGS = -undefined dynamic_lookup

all: markdown_chunker test_generator python_module

markdown_chunker: markdown_chunker.cpp main.cpp markdown_chunker.hpp
	$(CXX) $(CXXFLAGS) -o markdown_chunker markdown_chunker.cpp main.cpp

test_generator: generate_test_markdown.c
	$(CC) $(CFLAGS) -o test_generator generate_test_markdown.c

# Python module target
python_module: py_markdown_chunker.cpp markdown_chunker.cpp markdown_chunker.hpp
	$(CXX) $(CXXFLAGS) -shared $(PYTHON_INCLUDES) $(PYTHON_LDFLAGS) \
	py_markdown_chunker.cpp markdown_chunker.cpp \
	-o markdown_chunker_py$(PYTHON_EXT_SUFFIX)

test: markdown_chunker test_generator
	./test_generator
	./markdown_chunker

clean:
	rm -f markdown_chunker test_generator test_markdown.md markdown_chunker_py$(PYTHON_EXT_SUFFIX)
