# Module Integration Guide

This project can be configured via CMake or XMake. Alternatively, you may directly copy any subdirectory under the `src` directory into your own project.

## Static Library Linkage List

Some modules require additional static libraries to be linked during compilation. See the table below for details:

| Module (Corresponding Directory) | Windows                                | Unix (Linux/MacOS)                                         |
|----------------------------------|----------------------------------------|------------------------------------------------------------|
| `DateTime`                       | None                                   | None                                                       |
| `Event`                          | None                                   | `pthread` (Manual linkage required on BSD systems such as FreeBSD) |
| `Net`                            | `ws2_32`                               | None                                                       |
| `OS`                             | `pdh`                                  | `pthread` (Manual linkage required on BSD systems such as FreeBSD) |
| `Parser`                         | None                                   | None                                                       |
| `TUI`                            | None                                   | None                                                       |

## Example: How to Compile Manually Imported Modules

Suppose your project imports the `OS` and `TUI` modules, with the following directory structure:

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

Depending on the compiler you use, follow the steps below:

**GCC / Clang / MinGW**

Taking the `g++` compiler as an example, to compile the project shown above, run the following commands:

```bash
gcc TUI/win/wcwidth.c -o TUI/win/wcwidth.o
g++ main.cpp TUI/Terminal.cpp TUI/TUI.cpp TUI/win/wcwidth.o OS/File.cpp OS/System.cpp -o main
```

> [!note]
>
> The Clang compiler works in a similar way, but remember to replace `gcc` with `clang` and `g++` with `clang++` before running the commands.
>
> When compiling with MinGW, you must append `-lpdh` to the end of the `g++` command for it to compile correctly!

**MSVC**

First, build each of the imported modules one by one (this only needs to be done for the first build):

```powershell
cl /EHsc /W3 /std:c++11 /c OS\File.cpp
cl /EHsc /W3 /std:c++11 /c OS\System.cpp
cl /EHsc /W3 /std:c++11 /c TUI\Terminal.cpp
cl /EHsc /W3 /std:c++11 /c TUI\TUI.cpp
cl /c TUI\win\wcwidth.c
```

For every subsequent build of the project, simply run the following commands:

```powershell
cl /EHsc /W3 /std:c++11 /c main.cpp
link main.obj OS\File.obj OS\System.obj TUI\Terminal.obj TUI\TUI.obj TUI\win\wcwidth.obj pdh.lib /out:app.exe
```

> [!note]
>
> Note that the OS module requires linking `pdh.lib` to run.

