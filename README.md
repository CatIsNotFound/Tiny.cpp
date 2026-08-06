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

| Module            | Path           | Functionality                                                      |
|-------------------|----------------|--------------------------------------------------------------------|
| **TUI**           | `src/TUI`      | Terminal User Interface, providing basic terminal drawing functionality for easier terminal rendering |
| **OS**            | `src/OS`       | Operating System, supporting basic file and path operations as well as viewing system information |
| **Net**           | `src/Net`      | Network communication, providing Socket, Address, hostname resolution, and advanced socket options |
| **DateTime**      | `src/DateTime` | Date and time utilities, supporting construction, formatting, arithmetic, and timestamp conversion |
| **CommandParser** | `src/Parser`   | Command parser, providing basic command parameter parsing functionality for easy implementation of command-line tools |
| **IniParser**     | `src/Parser`   | INI configuration file parser, supporting parsing, manipulating, and serializing INI files |
| **Events**        | `src/Events`   | Event system, providing basic components for easy implementation of timers, asynchronous functions, etc. |

## Installation

### Download Precompiled Project

You can download the latest precompiled release directly from [Github Release](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest).

### Compile Source Code Project

#### CMake

1. Download the project source code via Github:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git
    ```
    To use the unstable version, execute the following command:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git -b beta
    ```

2. Configure the project via CMake
    ```bash
    cd Tiny.cpp
    mkdir build ; cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/Tiny.cpp -DTINY_BUILD_TEST=OFF 
    ```
    **P.S: Please replace `/path/to/Tiny.cpp` with the actual installation path.**

3. Compile and install this project locally
    ```bash
    cmake --build . --target install
    ```

#### XMake

> If XMake is not installed, execute the following command in the terminal to quickly install XMake:
> 
> **Linux/MacOS/Unix-like:** 
> 
> ```bash
> curl -fsSL https://xmake.io/shget.text | bash 
> ```
>
> ```bash
> wget https://xmake.io/shget.text -O - | bash
> ```
>
> **Windows:** 
>
> ```powershell
> irm https://xmake.io/psget.text | iex
> ```

1. Download the project source code via Github:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git
    ```
    To use the unstable version, execute the following command:
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git -b beta
    ```
   
2. Configure the project via XMake
    ```bash
    cd Tiny.cpp
    xmake f --build_test=n
    ```

3. Compile and install this project locally
    ```bash
    xmake
    xmake package -o /path/to/install
    ```
    **P.S: Please replace `/path/to/install` with the actual installation path.**

## Quick Start

### Copy Source Code (Most Direct)

Directly copy any directory's corresponding module from the `src` directory to your own project. Each module contains a header file (`*.hpp`) and a source file (`*.cpp`).

### Import via CMake

If using CMake as your primary project management tool, it is recommended to import the `Tiny` project directly via CMake.

**P.S: Use `Tiny` as the project library name in CMake, not `Tiny.cpp`!**

1. Download the [precompiled binary library](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest) or [manually compile the source code](#compile-source-code-project) to your local machine.

2. Refer to the following example:

```cmake
cmake_minimum_required(VERSION 3.24)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 20)
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

### Import via XMake

Starting from version 1.2.0, `Tiny.cpp` supports importing via XMake.

To use XMake to import the `Tiny.cpp` project, you need to first [configure the XMake project](#compile-source-code-project), then add `Tiny.cpp` as a dependency in your project.

1. Download the [manually compile the source code](#compile-source-code-project) to your local machine.

2. Manually set up the local repository

```bash
xmake repo -g -a local_repo /path/to/package
```
**P.S: Please replace `/path/to/package` with the actual absolute path of the installation package.**

You can check if the local repository was added successfully with the following command:

```bash
xmake repo -l
```

3. Refer to the following example:

```lua
add_rules("mode.debug", "mode.release")
set_languages("c++20")
add_requires("tiny.cpp")

target("HelloWorld")
    set_kind("binary")
    add_files("**.cpp")
    add_packages("tiny.cpp")
```

## How to Use

Taking the TUI module as an example, assuming you want to use the Terminal module to simply output `Hello, Tiny.cpp!`, please execute the following code:

```cpp
// If importing via CMake, use the following import method:
#include <Tiny/TUI/Terminal.hpp>
// If directly copying the TUI directory to your project, it might be imported like this:
#include "TUI/Terminal.hpp"

using ter = Tiny::TUI::Terminal;

int main() {
   ter::printLine("Hello, Tiny.cpp!");
   return 0;
}
```

**P.S: To view the API reference, please refer to [API Reference](docs/home.md).**

## License

`Tiny.cpp` is licensed under the [MIT License](LICENSE). You can view detailed information in the `LICENSE` file or any other header or source file.
