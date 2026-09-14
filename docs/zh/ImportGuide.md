# 导入模块指南

本项目不仅可以通过 CMake、XMake 方式来配置项目，你仍可以直接复制 `src` 目录下的任一目录到你的项目之中。

## 静态库链接列表

对于部分模块，你仍需在编译时添加静态库的链接才能使用，详细信息见下表：

| 模块（对应目录） | Windows  | Unix (Linux/MacOS)                               |
| ---------------- | -------- | ------------------------------------------------ |
| `DateTime`       | 无       | 无                                               |
| `Event`          | 无       | `pthread`（FreeBSD 等 BSD 类系统下必需手动链接） |
| `Net`            | `ws2_32` | 无                                               |
| `OS`             | `pdh`    | `pthread`（FreeBSD 等 BSD 类系统下必需手动链接） |
| `Parser`         | 无       | 无                                               |
| `TUI`            | 无       | 无                                               |

## 示例：如何编译手动导入的模块

假设你的项目导入了 `OS` 和 `TUI` 模块，目录结构如下：

```
/path/to/MyProject
├─ OS
│   ├─ File.cpp
│   ├─ File.hpp
│   ├─ System.cpp
│   └─ System.hpp
├─ TUI
│   ├─ Terminal.cpp
│   ├─ Terminal.hpp
│   ├─ Terminal_Int.hpp
│   ├─ TUI.cpp
│   ├─ TUI.hpp
│   └─ win
│       ├─ wcwidth.c
│       └─ wcwidth.h
└─ main.cpp
```

根据不同的编译器，你需要进行如下操作：

**GCC / Clang / MinGW**

以使用 `g++` 编译器为例，要编译示例中的项目，需要执行如下：

```bash
gcc TUI/win/wcwidth.c -o TUI/win/wcwidth.o
g++ main.cpp TUI/Terminal.cpp TUI/TUI.cpp TUI/win/wcwidth.o OS/File.cpp OS/System.cpp -o main
```
> [!note]
>
> Clang 编译器也是类似如上那样执行，但请记得将 `gcc` 替换为 `clang`，`g++` 替换为 `clang++` 后再执行。
>
> 若使用 MinGW 进行编译，需要在使用 `g++` 编译时额外在尾部添加 `-lpdh` 才能正确编译！

**MSVC**

首先，将导入的模块进行一一构建（仅需第一次构建时执行）

```powershell
cl /EHsc /W3 /std:c++11 /c OS\File.cpp
cl /EHsc /W3 /std:c++11 /c OS\System.cpp
cl /EHsc /W3 /std:c++11 /c TUI\Terminal.cpp
cl /EHsc /W3 /std:c++11 /c TUI\TUI.cpp
cl /c TUI\win\wcwidth.c
```

后续每次构建项目，只需执行如下命令即可：

```powershell
cl /EHsc /W3 /std:c++11 /c main.cpp
link main.obj OS\File.obj OS\System.obj TUI\Terminal.obj TUI\TUI.obj TUI\win\wcwidth.obj pdh.lib /out:app.exe
```

> [!note]
>
> 其中， OS 模块需要链接 `pdh.lib` 才能执行。
