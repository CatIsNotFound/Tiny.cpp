# Tiny.cpp Documentation

![C++11](https://img.shields.io/badge/C++-11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Branch](https://img.shields.io/badge/branch-main-green.svg)

---

## Look for other language documentation?

- [中文文档](home_zh.md) - Chinese Documentation

---

## Overview

**Tiny.cpp** is an extremely tiny, lightweight, and easy-to-use foundational library. It provides core functionalities including an event system, file operations, system information retrieval, command-line parsing, and Terminal User Interface (TUI). The library is designed with modern C++ and supports Windows and Unix-like systems (Linux, macOS, etc.).

---

## Key Features

- **Modular Architecture**: Each module in the `src` directory can be reused independently without any configuration
- **Zero Dependencies**: No third-party library dependencies, only cross-platform architectural design
- **Easy to Use**: Adopts modern C++ standards (minimum C++11 support)

---

## Included Modules

| Module | Path | Description | Documentation |
|--------|------|-------------|---------------|
| **TUI** | `src/TUI` | Terminal User Interface, provides basic terminal drawing functionality for easier terminal rendering | [en/TUI/TUI.md](en/TUI/TUI.md) |
| **OS** | `src/OS` | Operating System, supports basic file and path operations as well as system information viewing | [en/OS/File.md](en/OS/File.md), [en/OS/System.md](en/OS/System.md) |
| **CommandParser** | `src/Parser` | Command parser, provides basic command parameter parsing functionality for easy implementation of command-line tools | [en/Parser/CommandParser.md](en/Parser/CommandParser.md) |
| **DateTime** | `src/DateTime` | Date and time utilities, supporting construction, formatting, arithmetic, and timestamp conversion | [en/DateTime/DateTime.md](en/DateTime/DateTime.md) |
| **Events** | `src/Events` | Event system, provides basic components for easy implementation of timers, asynchronous functions, etc. | [en/Events/Event.md](en/Events/Event.md) |

---

## Installation

### Download Pre-built Project

You can directly download the latest pre-built release from [Github Release](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest).

### Build from Source

1. Download project source code via Github:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git
    ```
    For non-stable versions, execute:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git -b beta
    ```

2. Configure project via CMake
    ```bash
    cd Tiny.cpp
    mkdir build ; cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/Tiny.cpp -DCMAKE_BUILD_TYPE=Release -DTINY_BUILD_TEST=OFF 
    ```
    **Note: Please replace `/path/to/Tiny.cpp` with your actual installation path.**

3. Build and install to local
    ```bash
    cmake --build . --target install
    ```

---

## Quick Start

### Copy Source Code (Most Direct)

Directly copy any directory corresponding to a module from the `src` directory to your own project. Each module contains a header file (`*.hpp`) and source file (`*.cpp`).

### Import via CMake

If using CMake as your primary project management tool, it is recommended to import the `Tiny` project directly via CMake.

**Note: Use `Tiny` as the project library name under CMake, not `Tiny.cpp`!**

1. [Download pre-built binary library](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest) or [manually build from source](#build-from-source) to your local machine.

2. Refer to the following example:

```cmake
cmake_minimum_required(VERSION 3.24)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "/path/to/Tiny.cpp")
find_package(Tiny REQUIRED)

add_executable(${CMAKE_PROJECT_NAME} 
        main.cpp
        # ....
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Tiny::Tiny
)
```

### How to Use

Taking the TUI module as an example, assuming you want to use the Terminal module to simply output `Hello, Tiny.cpp!`, execute the following code:

```cpp
// If importing via CMake method, use the following import:
#include <Tiny/TUI/Terminal.hpp>
// If directly copying the TUI directory to your project, it might be imported like this:
#include "TUI/Terminal.hpp"

using ter = Tiny::TUI::Terminal;

int main() {
   ter::printLine("Hello, Tiny.cpp!");   
   return 0;
}
```

### Include Methods

If using CMake to install this project, use the following include methods:

```cpp
// Main header file, includes all modules
#include <Tiny/Tiny.hpp>

// Or include specific modules separately
#include <Tiny/DateTime/DateTime.hpp>
#include <Tiny/Events/Events.hpp>
#include <Tiny/OS/File.hpp>
#include <Tiny/OS/System.hpp>
#include <Tiny/Parser/CommandParser.hpp>
#include <Tiny/TUI/TUI.hpp>
```

If directly copying the `src` directory source code, use the following include methods:

```cpp
// Main header file, includes all modules
#include "Tiny.hpp"

// Or include specific modules separately
#include "DateTime/DateTime.hpp"
#include "Events/Events.hpp"
#include "OS/File.hpp"
#include "OS/System.hpp"
#include "Parser/CommandParser.hpp"
#include "TUI/TUI.hpp"
```

### Manual Compilation

```bash
# Linux/macOS
g++ -std=c++17 -I./src main.cpp src/Tiny.cpp \
    src/DateTime/DateTime.cpp \
    src/Events/Events.cpp \
    src/OS/File.cpp \
    src/OS/System.cpp \
    src/Parser/CommandParser.cpp \
    src/TUI/TUI.cpp \
    src/TUI/Terminal.cpp \
    -o myapp

# Windows (MinGW)
g++ -std=c++17 -I./src main.cpp src/*.cpp src/*/*.cpp -o myapp.exe
```

---

## Global Configuration and Macros

### Operating System Detection Macros

| Macro | Description | Auto-define Condition |
|-------|-------------|----------------------|
| `TINY_CPP_MY_OS_WINDOWS` | Enable Windows platform code | `_WIN32` or `_WIN64` defined |
| `TINY_CPP_MY_OS_UNIX` | Enable Unix platform code | `__APPLE__`, `__linux__`, or `__unix__` defined |

**Manually specify platform:**

```cpp
// Manually define before including header files
#define TINY_CPP_MY_OS_WINDOWS
#include <Tiny/Tiny.hpp>
```

### Module Namespaces

| Namespace | Description |
|-----------|-------------|
| `Tiny` | Root namespace |
| `Tiny::OS` | Operating system related functions (files, system info) |
| `Tiny::DT` | Date and time utilities |
| `Tiny::TUI` | Terminal user interface functions |

### Runtime Environment

- **C++ Standard**: C++ 11 or higher
- **Operating Systems**: Windows (Windows 7+), Linux, macOS, Unix-like systems
- **Compilers**: GCC 7+, Clang 5+, MSVC 2017+

### Dependencies

- **Windows**: Depends on `windows.h`, `shlobj.h`, `pdh.h` (performance counters)
- **Unix/Linux**: Depends on standard POSIX interfaces, `termios`, `sys/ioctl.h`
- **macOS**: Additional dependencies on `mach/mach.h` and other system interfaces

---

## Module Documentation

### DateTime Module

Date and time handling utilities.

- [DateTime Module Detailed Documentation](en/DateTime/DateTime.md)

### Events Module

Asynchronous event management system.

- [Event Class Detailed Documentation](en/Events/Event.md)

### OS Module

Operating system related functions.

- [File Module - File Operations](en/OS/File.md)
- [System Module - System Information](en/OS/System.md)

### Parser Module

Command-line argument parser.

- [CommandParser Class Detailed Documentation](en/Parser/CommandParser.md)

### TUI Module

Terminal user interface.

- [TUI Module Detailed Documentation](en/TUI/TUI.md)

---

## Version and Compatibility

### Version Information

- **Current Version**: Unreleased
- **Release Date**: 2026
- **License**: MIT License
- **Author**: CatIsNotFound

### Platform Compatibility

| Platform | Support Status | Tested Version |
|----------|----------------|----------------|
| Windows 10/11 | Fully supported | Tested |
| Windows 7/8 | Partially supported | Not tested |
| Linux (x86_64) | Fully supported | Ubuntu 22.04 |
| Linux (ARM64) | Fully supported | Not tested |
| macOS (Intel) | Fully supported | Not tested |
| macOS (Apple Silicon) | Fully supported | Not tested |

### Compiler Compatibility

| Compiler | Minimum Version | Support Status |
|----------|-----------------|----------------|
| GCC | 7.0 | Fully supported |
| Clang | 5.0 | Fully supported |
| MSVC | 2017 | Fully supported |
| MinGW-w64 | 8.0 | Fully supported |

### C++ Standard

- **Required**: C++11 or higher
- **Recommended**: C++20
- **Tested Standards**: C++11, C++20

### Known Limitations

1. **Windows 7**: Some system information retrieval APIs may not be available
2. **macOS**: CPU usage calculation differs slightly from Linux
3. **Terminal**: TUI module requires terminal supporting ANSI escape sequences
4. **Encoding**: File paths use UTF-8 encoding on Windows

## License

`Tiny.cpp` is licensed under the [MIT License](../LICENSE). You can view detailed information in the `LICENSE` file or any other header or source file.
