# Using the GPM Library on Linux TTY Terminals

## Problem Description

On Linux terminals, when using TTY mode (without a desktop environment), mouse event handling encounters issues. By default, the `libgpm` library is not installed, making it impossible to capture mouse events.

## Installing the `libgpm` Library and Enabling the `gpm` Service

On the Linux console, use the following commands to install the `libgpm` library and enable the `gpm` service (if not already enabled):

**Debian/Ubuntu**

```bash
sudo apt update
sudo apt install libgpm-dev gpm -y
sudo systemctl enable --now gpm
```

**Fedora/RockyOS/Redhat**

```bash
sudo dnf install gpm gpm-devel gpm-libs -y
sudo systemctl enable --now gpm
```

**Arch Linux**

```bash
sudo pacman -S libgpm gpm
sudo systemctl enable --now gpm
```

## Compiling and Linking the GPM Library

### CMake Projects

If you are using CMake as your primary project management tool, you only need to recompile and install the `Tiny.cpp` library. Specifically:

```bash
cd /path/to/Tiny.cpp
cmake -B build . -DCMAKE_INSTALL_PREFIX="/path/to/install/Tiny.cpp" -DTINY_USE_GPM=ON
cmake --build build --target install
```

- Replace `/path/to/install/Tiny.cpp` with your actual installation path.
- `DTINY_USE_GPM=ON` enables the GPM library.

Afterwards, you can use the `Tiny::TUI::Terminal` module in your project to capture mouse events.

### Direct Source Import

If you are using the TUI module by directly copying the source code, you need to predefine the `TINY_CPP_USE_GPM` macro before including the `TUI/Terminal.hpp` header in your project.

```cpp
#define TINY_CPP_USE_GPM
#include "TUI/Terminal.hpp"
```

Then, when compiling with g++, please add the `-lgpm` option to link the GPM library when building your project.
