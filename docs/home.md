# Tiny.cpp Documentation

![C++17](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Branch](https://img.shields.io/badge/branch-main-green.svg)

---

## Languages

- [中文文档](home_zh.md) - Chinese Documentation
- [English Documentation](en/) - English Documentation

---

## Overview

**Tiny.cpp** is an extremely tiny, lightweight, and easy-to-use foundational library. It provides core functionalities including an event system, file operations, system information retrieval, command-line parsing, and Terminal User Interface (TUI). The library is designed with modern C++ and supports Windows and Unix-like systems (Linux, macOS, etc.).

---

## Key Features

- **Modular Architecture**: Each module in the `src` directory can be reused independently without any configuration
- **Zero Dependencies**: No third-party library dependencies, only cross-platform architectural design
- **Easy to Use**: Adopts modern C++ standards (minimum C++17 support)

---

## Included Modules

| Module | Path | Description | Documentation |
|--------|------|-------------|---------------|
| **TUI** | `src/TUI` | Terminal User Interface, provides basic terminal drawing functionality for easier terminal rendering | [en/TUI/TUI.md](en/TUI/TUI.md) |
| **OS** | `src/OS` | Operating System, supports basic file and path operations as well as system information viewing | [en/OS/File.md](en/OS/File.md), [en/OS/System.md](en/OS/System.md) |
| **CommandParser** | `src/Parser` | Command parser, provides basic command parameter parsing functionality for easy implementation of command-line tools | [en/Parser/CommandParser.md](en/Parser/CommandParser.md) |
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
    mkdir build && cd build
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
| `Tiny::TUI` | Terminal user interface functions |

### Runtime Environment

- **C++ Standard**: C++17 or higher
- **Operating Systems**: Windows (Windows 7+), Linux, macOS, Unix-like systems
- **Compilers**: GCC 7+, Clang 5+, MSVC 2017+

### Dependencies

- **Windows**: Depends on `windows.h`, `shlobj.h`, `pdh.h` (performance counters)
- **Unix/Linux**: Depends on standard POSIX interfaces, `termios`, `sys/ioctl.h`
- **macOS**: Additional dependencies on `mach/mach.h` and other system interfaces

---

## Module Documentation

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

## Error Codes and Exception Handling

### CommandParser Error Codes

| Error Code | Enum Value | Trigger Scenario | Solution |
|------------|------------|------------------|----------|
| `NoError` | 0 | Parse successful | None |
| `UnknownOption` | 1 | Encountered undefined option | Check command definitions, confirm option name is correct |
| `FullOptionOnly` | 2 | Short option used for long-only option | Use `--option` instead of `-o` |
| `InvalidValue` | 3 | Parameter value invalid or empty | Check parameter value format, provide valid value |
| `MissingArgument` | 4 | Missing required parameter value | Provide parameter for options requiring values |
| `FormatError` | 5 | Parameter format error | Check parameter format, ensure it starts with `-` |

### File Operation Errors

File operation classes (`File`, `Path`) do not throw exceptions, indicating errors through return values:

| Scenario | Return Value | Description |
|----------|--------------|-------------|
| Invalid path | `isValid()` returns false | Path does not exist or is inaccessible |
| File open failure | `open()` returns false | Insufficient permissions or file in use |
| Read failure | Returns empty data | File not open or at end |
| Write failure | Returns false | File not opened in write mode |

### Event System Error Handling

Exceptions in event callbacks are caught and output to stderr:

```cpp
Event event(1, "Test", [](const std::atomic<bool>&) {
    throw std::runtime_error("Error!");
});
// Exception is caught, outputs: "Tiny::Event: An error has occurred: Error!"
```

---

## FAQ

### Q1: How to properly handle UTF-8 strings?

**A:** The library internally handles UTF-8 encoding. Use the `splitUTF8()` function to split strings into character arrays:

```cpp
auto chars = Tiny::TUI::splitUTF8("Chinese String");
for (const auto& ch : chars) {
    std::cout << ch << std::endl;
}
```

### Q2: Why does the `File` class prohibit copying?

**A:** File handles are system resources; copying would cause duplicate close issues. Please use move semantics:

```cpp
Tiny::OS::File file1("test.txt", Tiny::OS::ReadOnly);
Tiny::OS::File file2 = std::move(file1);  // Correct
// Tiny::OS::File file3 = file1;  // Error: copy constructor deleted
```

### Q3: How to get accurate CPU usage?

**A:** `getCPUInfo()` requires a sampling interval (default 50ms), calling will briefly block:

```cpp
Tiny::OS::CPU cpu;
Tiny::OS::getCPUInfo(cpu, 100);  // Sample 100ms, more accurate results
```

### Q4: How to exit program in raw mode?

**A:** Ensure `leaveRawMode()` is called before program exit, or use RAII pattern:

```cpp
{
    Tiny::TUI::Terminal::enterRawMode();
    // ... program logic
    Tiny::TUI::Terminal::leaveRawMode();
}
// Or use Renderer, whose destructor automatically exits raw mode
```

### Q5: How does the command parser handle parameters with equals signs?

**A:** Supports both `--option=value` and `--option value` formats:

```bash
myapp --output=file.txt   # Format 1
myapp --output file.txt   # Format 2
myapp -o file.txt         # Short option format
```

### Q6: How to traverse directories and subdirectories?

**A:** Use the recursion parameter of `listPath()`:

```cpp
// Recursion depth 255 (actually unlimited)
auto all_files = Tiny::OS::FileSystem::listPath("./src", 255);

// Traverse only one level
auto current_files = Tiny::OS::FileSystem::listPath("./src", 1);
```

### Q7: How are Windows and Unix path separators handled?

**A:** The library internally handles them automatically; both `/` and `\` work uniformly:

```cpp
Tiny::OS::Path path1("C:/Users/name/file.txt");   // Correct
Tiny::OS::Path path2("C:\\Users\\name\\file.txt"); // Correct
```

### Q8: How to safely stop events?

**A:** Use the `stop()` method to set the stop flag; the event will check on the next loop iteration:

```cpp
Tiny::Event event(1, "Loop", [](const std::atomic<bool>& running) {
    while (running) {  // Check stop flag
        // Execute task
    }
});
event.run();
// ...
event.stop();  // Request stop
```

---

## Version and Compatibility

### Version Information

- **Current Version**: 1.0.0
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

- **Required**: C++17 or higher
- **Recommended**: C++20
- **Tested Standards**: C++17, C++20

### Known Limitations

1. **Windows 7**: Some system information retrieval APIs may not be available
2. **macOS**: CPU usage calculation differs slightly from Linux
3. **Terminal**: TUI module requires terminal supporting ANSI escape sequences
4. **Encoding**: File paths use UTF-8 encoding on Windows

### Changelog

#### v1.0.0 (2026)
- Initial release
- Implemented five modules: Events, OS::File, OS::System, Parser, TUI
- Support for Windows and Unix-like systems
- Complete cross-platform file operation API
- Command-line parameter parser implementation
- Terminal user interface rendering functionality

---

## License

`Tiny.cpp` is licensed under the [MIT License](../LICENSE). You can view detailed information in the `LICENSE` file or any other header or source file.
