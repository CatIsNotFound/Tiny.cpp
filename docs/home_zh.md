# Tiny.cpp 手册

![C++11](https://img.shields.io/badge/C++-11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Branch](https://img.shields.io/badge/branch-main-green.svg)

---

## 目录

1. [库简介](#1-库简介)
2. [主要特点](#2-主要特点)
3. [包含模块](#3-包含模块)
4. [安装](#4-安装)
5. [快速开始](#5-快速开始)
6. [全局配置与宏定义](#6-全局配置与宏定义)
7. [模块文档](#7-模块文档)
8. [版本与兼容性说明](#8-版本与兼容性说明)
9. [许可证说明](#9-许可证说明)

---

## 1. 库简介

**Tiny.cpp** 是一个极度微小、极度轻量、简单易用的基础库。提供事件系统、文件操作、系统信息获取、命令行解析和终端用户界面（TUI）等核心功能。该库采用现代 C++ 设计，支持 Windows 和 Unix-like 系统（Linux、macOS 等）。

---

## 2. 主要特点

- **模块化架构**: 可独立复用 `src` 目录下的任一模块，无需任何配置
- **零依赖**: 不依赖任何第三方库，仅使用跨平台架构设计
- **简单易用**: 采用现代 C++ 标准（最低支持 C++ 11 标准）

---

## 3. 包含模块

| 模块 | 路径 | 功能 | 文档链接 |
|------|------|------|----------|
| **TUI** | `src/TUI` | 终端用户界面，提供了基本的终端绘制功能，可更方便的渲染终端 | [zh/TUI/TUI.md](zh/TUI/TUI.md) |
| **OS** | `src/OS` | 操作系统，支持文件、路径的基本操作以及查看系统信息等基本功能 | [zh/OS/File.md](zh/OS/File.md)、[zh/OS/System.md](zh/OS/System.md) |
| **CommandParser** | `src/Parser` | 命令解析器，提供基本的命令参数解析功能，可便于实现基本的命令行工具 | [zh/Parser/CommandParser.md](zh/Parser/CommandParser.md) |
| **DateTime** | `src/DateTime` | 日期时间工具，支持构造、格式化、算术运算和时间戳转换 | [zh/DateTime/DateTime.md](zh/DateTime/DateTime.md) |
| **Events** | `src/Events` | 事件系统，提供基本的组件，可方便实现定时器、异步等功能 | [zh/Events/Event.md](zh/Events/Event.md) |

---

## 4. 安装

### 4.1 下载预编译项目

你可以直接在 [Github Release](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest) 下载最新发布的预编译版本项目。

### 4.2 编译源代码项目

1. 通过 Github 下载项目源代码：
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git
    ```
    如要使用非稳定版本，请执行如下命令：
    ```bash
    git clone https://github.com/CatIsNotFound/Tiny.cpp.git -b beta
    ```
   
2. 通过 CMake 配置项目
    ```bash
    cd Tiny.cpp
    mkdir build ; cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/Tiny.cpp -DCMAKE_BUILD_TYPE=Release -DTINY_BUILD_TEST=OFF 
    ```
    **p.s: 请将 `/path/to/Tiny.cpp` 替换为实际安装的路径。**

3. 编译并安装本项目到本地
    ```bash
    cmake --build . --target install
    ```

---

## 5. 快速开始

### 5.1 复制源代码（最直接）

在 `src` 目录下直接复制任何一个目录对应的模块到自己的项目中，每个模块都包含一个头文件（`*.hpp`）和源文件（`*.cpp`）。

### 5.2 通过 CMake 方式导入

若使用 CMake 作为主要的项目管理工具，建议直接采用 CMake 的方式导入 `Tiny` 项目。

**p.s: CMake 下使用 `Tiny` 作为项目库名称，而不是 `Tiny.cpp`！**

1. 通过[下载预编译二进制库](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest)或[手动编译源代码](#42-编译源代码项目)到你的本地。

2. 具体参照如下示例：

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

### 5.3 如何使用

以导入 TUI 模块为例，假设要使用 Terminal 模块以简单输出 `Hello, Tiny.cpp!`，请执行如下代码：

```cpp
// 若使用 CMake 方式导入，请用以下方式导入：
#include <Tiny/TUI/Terminal.hpp>
// 若直接将 TUI 目录复制到你的项目里，大致可能以这种形式导入：
#include "TUI/Terminal.hpp"

using ter = Tiny::TUI::Terminal;

int main() {
   ter::printLine("Hello, Tiny.cpp!");   
   return 0;
}
```

### 5.4 引入方式

若使用 CMake 方式安装本项目，请使用如下引入方式：

```cpp
// 主头文件，包含所有模块
#include <Tiny/Tiny.hpp>

// 或单独引入特定模块
#include <Tiny/DateTime/DateTime.hpp>
#include <Tiny/Events/Events.hpp>
#include <Tiny/OS/File.hpp>
#include <Tiny/OS/System.hpp>
#include <Tiny/Parser/CommandParser.hpp>
#include <Tiny/TUI/TUI.hpp>
```

若直接复制源代码 `src` 目录，则使用如下引入方式：

```cpp
// 主头文件，包含所有模块
#include "Tiny.hpp"

// 或单独引入特定模块
#include "DateTime/DateTime.hpp"
#include "Events/Events.hpp"
#include "OS/File.hpp"
#include "OS/System.hpp"
#include "Parser/CommandParser.hpp"
#include "TUI/TUI.hpp"
```

### 5.5 手动编译

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

## 6. 全局配置与宏定义

### 6.1 操作系统检测宏

| 宏定义 | 说明 | 自动定义条件 |
|--------|------|--------------|
| `TINY_CPP_MY_OS_WINDOWS` | 启用 Windows 平台代码 | `_WIN32` 或 `_WIN64` 已定义 |
| `TINY_CPP_MY_OS_UNIX` | 启用 Unix 平台代码 | `__APPLE__`、`__linux__` 或 `__unix__` 已定义 |

**手动指定平台:**

```cpp
// 在包含头文件前手动定义
#define TINY_CPP_MY_OS_WINDOWS
#include <Tiny/Tiny.hpp>
```

### 6.2 模块命名空间

| 命名空间 | 说明 |
|----------|------|
| `Tiny` | 根命名空间 |
| `Tiny::OS` | 操作系统相关功能（文件、系统信息） |
| `Tiny::DT` | 日期时间工具 |
| `Tiny::TUI` | 终端用户界面功能 |

### 6.3 运行环境

- **C++ 标准**: C++11 或更高版本
- **操作系统**: Windows (Windows 7+)、Linux、macOS、Unix-like 系统
- **编译器**: GCC 7+、Clang 5+、MSVC 2017+

### 6.4 依赖说明

- **Windows**: 依赖 `windows.h`、`shlobj.h`、`pdh.h`（性能计数器）
- **Unix/Linux**: 依赖标准 POSIX 接口、`termios`、`sys/ioctl.h`
- **macOS**: 额外依赖 `mach/mach.h` 等系统接口

---

## 7. API 文档

### 7.1 DateTime 模块

日期时间处理工具。

- [DateTime 模块详细文档](zh/DateTime/DateTime.md)

### 7.2 Events 模块

异步事件管理系统。

- [Event 类详细文档](zh/Events/Event.md)

### 7.3 OS 模块

操作系统相关功能。

- [File 模块 - 文件操作](zh/OS/File.md)
- [System 模块 - 系统信息](zh/OS/System.md)

### 7.4 Parser 模块

命令行参数解析器。

- [CommandParser 类详细文档](zh/Parser/CommandParser.md)

### 7.5 TUI 模块

终端用户界面。

- [TUI 模块详细文档](zh/TUI/TUI.md)

---

## 8. 版本与兼容性说明

### 8.1 版本信息

- **当前版本**: 暂未发布
- **发布日期**: 2026
- **许可证**: MIT License
- **作者**: CatIsNotFound

### 8.2 平台兼容性

| 平台 | 支持状态 | 测试版本 |
|------|----------|----------|
| Windows 10/11 | 完全支持 | 测试通过 |
| Windows 7/8 | 部分支持 | 未测试 |
| Linux (x86_64) | 完全支持 | Ubuntu 22.04 |
| Linux (ARM64) | 完全支持 | 未测试 |
| macOS (Intel) | 完全支持 | 未测试 |
| macOS (Apple Silicon) | 完全支持 | 未测试 |

### 8.3 编译器兼容性

| 编译器 | 最低版本 | 支持状态 |
|--------|----------|----------|
| GCC | 7.0 | 完全支持 |
| Clang | 5.0 | 完全支持 |
| MSVC | 2017 | 完全支持 |
| MinGW-w64 | 8.0 | 完全支持 |

### 8.4 C++ 标准

- **要求**: C++11 或更高版本
- **推荐**: C++20
- **测试标准**: C++11, C++20

### 8.5 已知限制

1. **Windows 7**: 部分系统信息获取 API 可能不可用
2. **macOS**: CPU 使用率计算方式与 Linux 略有不同
3. **终端**: TUI 模块需要支持 ANSI 转义序列的终端
4. **编码**: 文件路径在 Windows 上使用 UTF-8 编码

---

## 9. 许可证说明

`Tiny.cpp` 采用 [MIT 许可证](../LICENSE) 许可协议，您可以在 `LICENSE` 文件或其它任一头文件或源文件中查看详细信息。


