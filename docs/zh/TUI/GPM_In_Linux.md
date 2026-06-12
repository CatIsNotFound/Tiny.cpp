# 针对 Linux TTY 终端下使用 GPM 库
## 问题描述

在 Linux 终端下，使用 TTY 模式（无桌面环境下）时，鼠标事件处理会遇到问题。默认情况下，`libgpm` 库未安装，导致无法获取鼠标事件。

## 安装 `libgpm` 库并启用 `gpm` 服务

在 Linux 控制台下，使用以下命令安装 `libgpm` 库，并启用 `gpm` 服务（如果未启用）：

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

## 编译链接 GPM 库

### CMake 项目

若你是通过 CMake 作为主要的项目管理工具，你只需要重新编译并安装 `Tiny.cpp` 库即可。具体如下：

```bash
cd /path/to/Tiny.cpp
cmake -B build . -DCMAKE_INSTALL_PREFIX="/path/to/install/Tiny.cpp" -DTINY_USE_GPM=ON
cmake --build build --target install
```

- 其中的 `/path/to/install/Tiny.cpp` 为安装路径，根据实际情况修改。
- `DTINY_USE_GPM=ON` 表示启用 GPM 库。

之后，你可以在项目中使用 `Tiny::TUI::Terminal` 模块来获取鼠标事件。

### 通过源代码导入

若你是通过复制源代码的方式直接使用 `TUI` 模块，你需要在项目中包含 `TUI/Terminal.hpp` 头文件之前，提前预定义 `TINY_CPP_USE_GPM` 宏。

```cpp
#define TINY_CPP_USE_GPM
#include "TUI/Terminal.hpp"
```

之后，通过 g++ 编译时，请额外附带选项 `-lgpm` 来链接 GPM 库以编译项目即可。


