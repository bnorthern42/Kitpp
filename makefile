# Compiler and Linker
CXX = g++

# Compile Flags
# Note: kitpp uses std::source_location, which requires C++20 or newer.
CXXFLAGS_COMMON = -std=c++23 -Wall -Wextra -pedantic -fopenmp
CXXFLAGS_DEBUG = -g
CXXFLAGS_RELEASE = -O2

# Default to a release build for 'make' and 'make all'
# For a debug build, run: make CXXFLAGS_EXTRA="-g"
CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_RELEASE) $(CXXFLAGS_EXTRA)

# Preprocessor Flags (for include paths, defines)
# -Iinclude: Allows #include "kitpp/header.hpp"
CPPFLAGS = -Iinclude

# Directories
BINDIR = bin # For example executables

# Executable for example
EXAMPLE_SRC = examples/usage_example.cpp
EXAMPLE_EXE = $(BINDIR)/usage_example

# Default target: build the example program
all: example

# Example program
example: $(EXAMPLE_EXE)

$(EXAMPLE_EXE): $(EXAMPLE_SRC)
	@mkdir -p $(BINDIR)
	@echo "Compiling example: $(EXAMPLE_SRC)"
	# Since kitpp is header-only, we just need to compile the example with the correct include path.
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@
	@echo "To run the example: "
	@echo "  ./$(EXAMPLE_EXE)"

kitpp.o: kitpp.cpp
	@echo "Compiling kitpp: kitpp.cpp"
	$(CXX)

libkitpp.a: kitpp.o
	ar rcs libkitpp.a kitpp.o
# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	@rm -rf $(BINDIR)
	@echo "Clean complete."

# Phony targets (targets that are not actual files)
.PHONY: all example clean
