# kitpp – A C++ Utility Library

**kitpp** is a C++ library designed to provide a collection of general-purpose utilities and reusable components for C++ development. It offers well-organized modules with both standard and performance-optimized implementations.

## Features

* **Modular Design**: Code is organized into logical modules within the `kitpp` namespace.
* **Modern Build System**: Uses Meson for fast, reliable, and cross-platform builds.
* **Reusable Functions**: Common, battle-tested functions to avoid rewriting boilerplate.
* **Optimized Implementations**: Dedicated `opt` sub-namespaces for performance-critical tasks.
* **Terminal Colors**: Includes a modified version of `rang.hpp` for cross-platform terminal styling.
* **Header-Only Friendly**: Designed for easy integration.

## Prerequisites

* C++ compiler with **C++17** support (e.g. GCC 13+, Clang 16+)
* Meson build system (`pip install meson`)
* Ninja (`sudo apt install ninja-build` or equivalent)
* `git`

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/kitpp.git
cd kitpp
````

### 2. Build with Meson (Recommended)

Meson is the primary build system. It handles dependencies and configuration automatically.

```bash
# 1. Setup the build directory
meson setup build

# 2. Compile the library and examples
meson compile -C build

# 3. Run the usage example
./build/usage_example
```


### 3. Installation

To install headers and library files to your system (default: `/usr/local`):

```bash
meson install -C build
```

## How to Use kitpp in Your Project

### Option A: Using pkg-config (Recommended)

After installation, kitpp provides a package config file. You can use it with any build system or CLI.

```bash
g++ your_app.cpp $(pkg-config --cflags --libs kitpp) -std=c++23 -o my_app
```

### Option B: Manual Linking

If you are pointing directly to the build directory:

```bash
g++ your_app.cpp -o my_app \
    -I/path/to/kitpp/include \
    -L/path/to/kitpp/build \
    -lkitpp \
    -std=c++23
```

## Code Example

Others in Example directory.

```cpp
#include <iostream>
#include <kitpp/log/logger.hpp>

int main() {
    int my_var = 42;
    KITPP_LOG_VAR(my_var);

    KITPP_LOG_THREAD_CONTEXT("main thread final check");

    KITPP_LOG_ERROR("Exiting with a simulated error.");
    

    return 0;
}
```

## Project Structure

```
kitpp/
├── include/
│   └── kitpp/       # Public headers
├── examples/        # Usage examples
├── meson.build      # Meson build definition
├── meson_options.txt
└── README.md
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push and create a Pull Request

## License

kitpp is licensed under the **GNU General Public License v2.0

