# Tiny.cpp

![C++11](https://img.shields.io/badge/C++-11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Branch](https://img.shields.io/badge/branch-main-green.svg)

一个极度微小、极度轻量、简单易用的基础库。

## 主要特点

- 模块化架构，可独立复用 `src` 目录下的任一模块，无需任何配置；
- 不依赖任何第三方库，仅使用跨平台架构设计；
- 简单易用，采用现代 C++ 标准（最低支持 C++11 标准）

## 包含模块

| 模块         | 路径           | 功能                            |
|------------|--------------|-------------------------------|
| **File**   | `src/OS`     | 文件操作系统，支持文件、路径的基本操作           |
| **Events** | `src/Events` | 事件系统，提供基本的组件，可方便实现定时器、异步等功能   |
| **TUI**    | `src/TUI`    | 终端用户界面，提供了基本的终端绘制功能，可更方便的渲染终端 |

## 安装

### 下载预编译项目

你可以直接在 [Github Release](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest) 下载最新发布的预编译版本项目。

### 编译源代码项目

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

## 快速开始

### 复制源代码（最直接）

在 `src` 目录下直接复制任何一个目录对应的模块到自己的项目中，每个模块都包含一个头文件（`*.hpp`）和源文件（`*.cpp`）。

### 通过 CMake 方式导入

若使用 CMake 作为主要的项目管理工具，建议直接采用 CMake 的方式导入 `Tiny` 项目。

**p.s: CMake 下使用 `Tiny` 作为项目库名称，而不是 `Tiny.cpp`！**

1. 通过[下载预编译二进制库](https://github.com/CatIsNotFound/Tiny.cpp/releases/latest)或[手动编译源代码](#编译源代码项目)到你的本地。

2. 具体参照如下示例：

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

## 如何使用

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

> P.S: 由于此项目还在开发阶段，目前暂时没有相关的 API 文档，后续可能会计划更新。

## 许可证说明

`Tiny.cpp` 采用 [MIT 许可证](LICENSE) 许可协议，您可以在 `LICENSE` 文件或其它任一头文件或源文件中查看详细信息。
