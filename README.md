# Tiny.cpp

[简体中文](README_zh.md)

![C++11](https://img.shields.io/badge/C++-11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Branch](https://img.shields.io/badge/branch-main-green.svg)

An extremely small, lightweight, and easy-to-use foundational library.

## Key Features

- Modular architecture, allowing independent reuse of any module in the `src` directory without any configuration;
- No dependencies on any third-party libraries, using only cross-platform architectural design;
- Simple and easy to use, adopting modern C++ standards (minimum support for C++11 standard)

## Included Modules

| Module     | Path         | Functionality                                      |
|------------|--------------|----------------------------------------------------|
| **File**   | `src/OS`     | File operating system, supporting basic file and path operations |
| **Events** | `src/Events` | Event system, providing basic components for easy implementation of timers, async functionality, etc. |
| **TUI**    | `src/TUI`    | Terminal User Interface, providing basic terminal drawing functionality for easier terminal rendering |

## Installation

You can download the latest precompiled release directly from [Github Release](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest).

Or download the project source code directly via Github:

```bash
git clone https://github.com/CatIsNotFound/Tiny.cpp.git
```

To use the unstable version, execute the following command:

```bash
git clone https://github.com/CatIsNotFound/Tiny.cpp.git -b beta
```

## Quick Start

### Copy Source Code (Most Direct)

Directly copy any directory's corresponding module from the `src` directory to your own project. Each module contains a header file (`*.hpp`) and a source file (`*.cpp`).

### Import via CMake

If using CMake as your primary project management tool, it is recommended to import the `Tiny` project directly via CMake.

**P.S: Use `Tiny` as the project library name in CMake, not `Tiny.cpp`!**

Refer to the following example:

```cmake
cmake_minimum_required(VERSION 3.24)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH "/path/to/Tiny.cpp")
find_package(Tiny REQUIRED)

add_executable(HelloWorld main.cpp)

target_link_libraries(HelloWorld PRIVATE
    Tiny::Tiny
)
```

## License

`Tiny.cpp` is licensed under the [MIT License](LICENSE). You can view detailed information in the `LICENSE` file or any other header or source file.
