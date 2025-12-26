# kitpp - A C++ Utility Library

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://isocpp.org/) 

`kitpp` is a C++ library designed to provide **a collection of general-purpose utilities and reusable components for C++ development**. It offers well-organized modules with both standard and performance-optimized implementations.

## Features

* **Modular Design**: Code is organized into logical modules within the `kitpp` namespace (e.g., `kitpp::Math`, `kitpp::StringUtils`). (Note: Some modules like `Math` and `StringUtils` are future plans, focusing on HPC-related utilities first, or based on user requests/issues.)
* **Reusable Functions**: Aims to provide common, battle-tested functions to avoid rewriting boilerplate code.
* **Optimized Implementations**: For performance-critical tasks, `kitpp` provides alternative implementations in `opt` sub-namespaces (e.g., `kitpp::Math::opt::add()`). Users can explicitly choose these when speed is paramount.
* **Clear Namespacing**: Prevents name collisions and improves code readability.
* **Makefile Build System**: Simple and standard build process using `make`.
* **Static and Shared Libraries**: Build `kitpp` as either a static (`.a`) or shared (`.so`/`.dylib`) library.
* **Terminal Colors**: Includes a modified version of `rang.hpp` for cross-platform terminal color and style manipulation.
* **Header-Only Components**: Some parts of the library can be used header-only, without linking to the compiled library.

## Prerequisites

* A C++ compiler that supports C++23 (e.g., g++-13, Clang-16 or newer).
* `make` build automation tool.
* `git` (for cloning the repository).

## Getting Started

### 1. Clone the Repository

```bash
git clone [https://github.com/your-username/kitpp.git](https://github.com/your-username/kitpp.git)
cd kitpp
```

### 2. Build the Library

`kitpp` uses a Makefile for building.

* **Build both static and shared libraries and the example program:**
    ```bash
    make
    ```
    (This is equivalent to `make all`)

* **Build only the static library (`libkitpp.a`):**
    ```bash
    make static
    ```

* **Build only the shared library (`libkitpp.so` or `libkitpp.dylib`):**
    ```bash
    make shared
    ```

* **Build with debug symbols:**
    Pass the `-g` flag via `CXXFLAGS_EXTRA`.
    ```bash
    make CXXFLAGS_EXTRA="-g"
    ```
    Or, for a specific target:
    ```bash
    make static CXXFLAGS_EXTRA="-g"
    ```

* **Compile and run the example program:**
    This will also build the library if it hasn't been built yet.
    ```bash
    make example
    ```
    To run the example (assuming it linked against the static library or shared library path is configured):
    ```bash
    ./bin/usage_example
    ```
    If the example was linked against the shared library and it's not installed system-wide, you might need to set your library path:
    * Linux: `LD_LIBRARY_PATH=. ./bin/usage_example`
    * macOS: `DYLD_LIBRARY_PATH=. ./bin/usage_example`

### 3. Install the Library (Optional)

You can install the library headers and compiled library files to a specified location.

* **Install to the default prefix (`/usr/local`):**
    ```bash
    sudo make install
    ```

* **Install to a custom prefix (e.g., `~/local`):**
    ```bash
    make install INSTALL_PREFIX=~/local
    ```
    This will install:
    * Headers to: `$(INSTALL_PREFIX)/include/kitpp/`
    * Libraries to: `$(INSTALL_PREFIX)/lib/`

### 4. Clean Build Files

To remove all compiled object files, libraries, and example executables:
```bash
make clean
```

## How to Use `kitpp` in Your Project

1.  **Include Headers:**
    In your C++ source files, include the necessary `kitpp` headers. If you installed `kitpp` or have its `include` directory in your compiler's search path:
    ```cpp
    #include <kitpp/math/operations.hpp> // Assuming you have a math/operations.hpp module
    #include <kitpp/module_a.hpp>      // Example for another module
    ```

2.  **Link Against `kitpp`:**
    When compiling your application, you need to tell the linker where to find the `kitpp` library and to link against it.

    * **If `kitpp` is installed in a standard location (e.g., `/usr/local`):**
        ```bash
        g++ your_app.cpp -o my_app -lkitpp -std=c++23
        ```

    * **If `kitpp` is installed in a custom location or you are linking against the build directory:**
        You'll need to specify the include path (`-I`) for headers and library path (`-L`) for the library.
        ```bash
        g++ your_app.cpp -o my_app -I/path/to/kitpp/include -L/path/to/kitpp/lib -lkitpp -std=c++17
        ``` # NOTE: Change -std=c++17 to -std=c++23 for C++23 compatibility
        (Replace `/path/to/kitpp/` with the actual path to the `kitpp` build or installation directory).

### Example Usage Snippet
```cpp
 #include <iostream>
 #include <string>

 // Example for using the logger as a header-only component
 #define KITPP_LOGGER_HEADER_ONLY
 #include <kitpp/log/logger.hpp>

 // Example for using a compiled component (e.g., Math, if it were implemented)
 // #include <kitpp/math/operations.hpp>

 int main() {
     // --- Using kitpp::log (header-only example) ---
     // Initialize the logger (optional, default is console output)
     kitpp::log::Logger::init(kitpp::log::LogLevel::INFO);

     // Log messages at different levels
     kitpp::log::Logger::debug("This is a debug message.");
     kitpp::log::Logger::info("This is an info message.");
     kitpp::log::Logger::warn("This is a warning message.");
     kitpp::log::Logger::error("This is an error message.");
     kitpp::log::Logger::critical("This is a critical message!");

     // Log with formatting
     std::string name = "World";
     kitpp::log::Logger::info("Hello, {}!", name);

     // Change log level dynamically
     kitpp::log::Logger::setLogLevel(kitpp::log::LogLevel::DEBUG);
     kitpp::log::Logger::debug("This debug message will now be shown.");

     // --- Example for a compiled component (hypothetical Math module) ---
     // int a = 10;
     // int b = 5;

     // // Using a standard function (requires linking to libkitpp)
     // int sum_standard = kitpp::Math::add(a, b);
     // std::cout << "Standard sum: " << sum_standard << std::endl;

     // // Using an optimized function from the 'opt' namespace (requires linking)
     // int sum_optimized = kitpp::Math::opt::add(a, b);
     // std::cout << "Optimized sum: " << sum_optimized << std::endl;

     // --- Example with another hypothetical module ---
     // kitpp::StringUtils::FormattedString str = kitpp::StringUtils::format("Hello, %s!", "kitpp");
     // std::cout << kitpp::StringUtils::opt::toUpperCase(str.get()) << std::endl;


    // [Add more relevant examples for your library's main features]

    return 0;
}
```

### Using Optimized Versions (`opt` Namespace)

The `kitpp` library provides a clear way to access performance-optimized versions of certain functions. These are located within an `opt` sub-namespace of the module they belong to.

* Standard version: `kitpp::ModuleName::functionName()`
* Optimized version: `kitpp::ModuleName::opt::functionName()`

The "standard" version aims for clarity and correctness, potentially being a more straightforward implementation. The "optimized" version is designed for speed and may use more complex algorithms or techniques. The choice of which to use depends on the specific needs of your application at the call site. Both versions are compiled with the optimization flags (e.g., `-O2`, `-O3`) specified in the Makefile during the library build.

## Project Structure

```
kitpp/
├── include/
│   └── kitpp/  # Public headers (e.g., module_a.hpp, math/operations.hpp)
├── src/        # Source (.cpp) files implementing the library's functionality
├── examples/   # Example programs demonstrating library usage
├── Makefile    # Build script
└── README.md   # This file
```

## Contributing

Contributions are welcome! If you'd like to contribute, please follow these steps:
1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/your-feature-name`).
3.  Make your changes.
4.  Ensure your code builds and tests pass (if you add tests).
5.  Commit your changes (`git commit -am 'Add some feature'`).
6.  Push to the branch (`git push origin feature/your-feature-name`).
7.  Create a new Pull Request.

Please ensure your code adheres to the existing style and includes documentation for new features.

## Third-Party Components

`kitpp` incorporates a modified version of the `rang` library for cross-platform terminal color and style manipulation.

*   **rang**: [https://github.com/agauniyal/rang](https://github.com/agauniyal/rang) (MIT License)

## License

`kitpp` is licensed under the **GNU General Public License v2.0**.
You can find a copy of the license at: [https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
