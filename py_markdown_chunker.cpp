#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include "markdown_chunker.hpp"

namespace py = pybind11;

PYBIND11_MODULE(markdown_chunker_py, m) {
    m.doc() = R"pbdoc(
        Python bindings for MarkdownChunker C++ library
        
        This module provides functionality to break down markdown text into chunks
        based on semantic breakpoints like headers, code blocks, etc.
    )pbdoc";
    
    // Expose the Breakpoint struct
    py::class_<Breakpoint>(m, "Breakpoint")
        .def(py::init<>())
        .def_readonly("offset", &Breakpoint::offset)
        .def_readonly("priority", &Breakpoint::priority)
        .def_readonly("type", &Breakpoint::type)
        .def("__repr__",
            [](const Breakpoint &bp) {
                std::stringstream ss;
                ss << "Breakpoint(offset=" << bp.offset 
                   << ", priority=" << bp.priority 
                   << ", type='" << bp.type << "')";
                return ss.str();
            }
        )
        .doc() = R"pbdoc(
            A breakpoint in markdown text that represents a potential split location.
            
            Attributes:
                offset (int): Character offset in the original text
                priority (int): Priority of the breakpoint (lower = higher priority)
                type (str): Type of breakpoint (e.g., "h1", "code_block")
        )pbdoc";
    
    // Expose the MarkdownChunker class
    py::class_<MarkdownChunker>(m, "MarkdownChunker")
        .def(py::init<>())
        .def("set_text", &MarkdownChunker::setText, py::arg("text"), 
             "Load a new markdown string and compute breakpoints")
        .def("create_chunks",
             static_cast<std::vector<std::string> (MarkdownChunker::*)(int) const>(&MarkdownChunker::createChunks),
             py::arg("max_tokens"),
             R"pbdoc(
                Create chunks based on the loaded text and breakpoints.
                
                Args:
                    max_tokens (int): Maximum number of tokens per chunk
                
                Returns:
                    List[str]: List of markdown text chunks
                
                Raises:
                    ValueError: If max_tokens is less than 1 or text hasn't been set
             )pbdoc")
        .def("print_breakpoints", [](const MarkdownChunker& self) {
                std::stringstream ss;
                self.printBreakpoints(ss);
                return ss.str();
            },
            "Return a string representation of all breakpoints in a formatted table"
        );
}
