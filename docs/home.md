# Tiny.cpp Documentation

[中文文档](home_zh.md)

Welcome to the Tiny.cpp library documentation. This documentation provides comprehensive API references for all modules.

---

## Overview

Tiny.cpp is a lightweight C++ utility library providing cross-platform functionality for:

- **Terminal User Interface (TUI)** - Terminal rendering and UI components
- **Operating System (OS)** - File operations, path handling, system information
- **Command Parser** - Command-line argument parsing
- **Events** - Asynchronous event management and timers

## Quick Start

### Installation

```bash
git clone https://github.com/yourusername/Tiny.cpp.git
cd Tiny.cpp
mkdir build && cd build
cmake ..
cmake --build .
```

### Basic Usage

```cpp
#include "OS/File.hpp"
#include <iostream>

int main() {
    Tiny::OS::Path path("./example.txt");
    if (path.isValid()) {
        std::cout << "File exists: " << path.path() << std::endl;
    }
    return 0;
}
```

## Module Documentation

| Module | Description | Documentation |
|--------|-------------|---------------|
| **Events** | Asynchronous event system for timers and callbacks | [Events/Event.md](en/Events/Event.md) |
| **OS::File** | File and path operations | [OS/File.md](en/OS/File.md) |
| **OS::System** | System information (CPU, memory, disk) | [OS/System.md](en/OS/System.md) |
| **Parser** | Command-line argument parsing | [Parser/CommandParser.md](en/Parser/CommandParser.md) |
| **TUI** | Terminal user interface rendering | [TUI/TUI.md](en/TUI/TUI.md) |

## Directory Structure

```
docs/
├── home.md              # This file - Documentation entry point
├── zh/                  # Chinese documentation
│   ├── Events/
│   │   └── Event.md
│   ├── OS/
│   │   ├── File.md
│   │   └── System.md
│   ├── Parser/
│   │   └── CommandParser.md
│   └── TUI/
│       └── TUI.md
└── en/                  # English documentation
    ├── Events/
    │   └── Event.md
    ├── OS/
    │   ├── File.md
    │   └── System.md
    ├── Parser/
    │   └── CommandParser.md
    └── TUI/
        └── TUI.md
```

## License

This project is licensed under the MIT License.
