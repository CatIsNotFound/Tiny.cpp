# OS::System 模块

命名空间: `Tiny::OS`

---

## 目录

1. [模块简介](#1-模块简介)
2. [头文件](#2-头文件)
3. [数据结构](#3-数据结构)
4. [系统信息函数](#4-系统信息函数)
5. [FileSystem 类](#5-filesystem-类)
6. [使用示例](#6-使用示例)
7. [注意事项](#7-注意事项)

---

## 1. 模块简介

`OS::System` 模块提供系统信息获取和文件系统操作功能：

- **系统信息**: CPU、内存、磁盘、主机信息获取
- **文件系统**: 目录创建、文件复制移动删除、路径遍历

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/OS/System.hpp>
// 直接复制源代码方式
#include "OS/System.hpp"
```

---

## 3. 全局常量

### Name

```cpp
constexpr const char* Name;
```

**说明**: 当前操作系统名称常量，根据编译平台自动定义。

| 平台 | 值 |
|------|-----|
| Windows | `"windows"` |
| Linux | `"linux"` |
| macOS | `"apple"` |
| Unix | `"unix"` |
| Android | `"android"` |
| OpenHarmony | `"openHarmony"` |
| 其他 | `"unknown"` |

**使用示例**:
```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    std::cout << "Current OS: " << Tiny::OS::Name << std::endl;
    return 0;
}
```

---

## 4. 数据结构

### 4.1 HostInfo 结构体

```cpp
struct HostInfo {
    std::string host_name;   // 主机名
    std::string user_name;   // 当前用户名
    std::string os_name;     // 操作系统名称
    std::string machine;     // 机器架构
    std::string version;     // 系统版本
};
```

| 字段 | 类型 | 说明 |
|------|------|------|
| `host_name` | `std::string` | 计算机在网络中的名称 |
| `user_name` | `std::string` | 当前登录用户名 |
| `os_name` | `std::string` | 操作系统名称（如 "Windows 10"、"Ubuntu"） |
| `machine` | `std::string` | 硬件架构（如 "x86_64"） |
| `version` | `std::string` | 操作系统版本号 |

### 4.2 CPU_Arch 枚举

```cpp
enum class CPU_Arch {
    Unknown     = 0,
    X86         = 1,
    X86_64      = 2,
    ARM32       = 3,
    ARM64       = 4,
    LoongArch   = 5,  // 龙芯架构
    MIPS        = 6,  // MIPS 架构
    RISCV       = 7,  // RISC-V 架构
    IA64        = 8   // Intel Itanium
};
```

### 4.3 CPU 结构体

```cpp
struct CPU {
    CPU_Arch machine;           // CPU 架构
    uint32_t cores;             // 核心数
    uint32_t page_size;         // 内存页大小（字节）
    float total_usage;          // 总体使用率（百分比）
    std::vector<float> usages;  // 各核心使用率
};
```

| 字段 | 类型 | 说明 |
|------|------|------|
| `machine` | `CPU_Arch` | CPU 架构类型 |
| `cores` | `uint32_t` | 逻辑核心数量 |
| `page_size` | `uint32_t` | 系统内存页大小 |
| `total_usage` | `float` | 总体 CPU 使用率（0-100） |
| `usages` | `std::vector<float>` | 每个核心的使用率 |

### 4.4 Memory 结构体

```cpp
struct Memory {
    size_t total_ram;        // 总物理内存
    size_t free_ram;         // 空闲物理内存
    size_t available_ram;    // 可用物理内存
    size_t used_ram;         // 已用物理内存
    size_t total_swap;       // 总交换空间
    size_t free_swap;        // 空闲交换空间
    
    // macOS 特有字段
    size_t app_free_mem;     // 应用可用内存
    size_t app_active_mem;   // 活跃内存
    size_t app_inactive_mem; // 非活跃内存
    size_t app_wired_mem;    // wired 内存
    size_t app_compress_mem; // 压缩内存
    size_t app_speculative_mem; // 推测内存
};
```

| 字段 | 类型 | 说明 | 平台支持 |
|------|------|------|----------|
| `total_ram` | `size_t` | 总物理内存（字节） | All |
| `free_ram` | `size_t` | 完全空闲内存（字节） | All |
| `available_ram` | `size_t` | 应用程序可用内存（字节） | All |
| `used_ram` | `size_t` | 已使用内存（字节） | All |
| `total_swap` | `size_t` | 总交换空间（字节） | All |
| `free_swap` | `size_t` | 空闲交换空间（字节） | All |
| `app_*` | `size_t` | macOS 特有内存统计 | macOS only |

### 4.5 DiskSpace 结构体

```cpp
struct DiskSpace {
    size_t total_bytes;      // 总容量
    size_t free_bytes;       // 空闲容量
    size_t used_bytes;       // 已用容量
    size_t available_bytes;  // 可用容量（考虑保留空间）
};
```

---

## 5. 系统信息函数

### 4.1 getCPUArchName

```cpp
const char* getCPUArchName(CPU_Arch cpu_arch);
```
- **功能**: 获取 CPU 架构名称字符串
- **参数**: `cpu_arch` - CPU 架构枚举值
- **返回值**: 架构名称（如 "x86_64"、"arm64"）

### 4.2 currentHostInfo

```cpp
HostInfo currentHostInfo();
```
- **功能**: 获取当前主机信息
- **返回值**: `HostInfo` 结构体

### 4.3 currentCPUInfo

```cpp
CPU currentCPUInfo();
```
- **功能**: 获取当前 CPU 信息
- **返回值**: `CPU` 结构体
- **注意事项**: 获取使用率会有短暂延迟（约 50ms）

### 4.4 currentMemory

```cpp
Memory currentMemory();
```
- **功能**: 获取当前内存信息
- **返回值**: `Memory` 结构体

### 4.5 currentDiskSpace

```cpp
DiskSpace currentDiskSpace();
```
- **功能**: 获取当前磁盘空间信息
- **返回值**: `DiskSpace` 结构体

### 4.6 getHostInfo

```cpp
bool getHostInfo(HostInfo& info);
```
- **功能**: 获取主机信息到引用参数
- **参数**: `info` - 输出参数
- **返回值**: `true` 表示成功

### 4.7 getCPUInfo

```cpp
bool getCPUInfo(CPU& info, size_t internal = 50);
```
- **功能**: 获取 CPU 信息
- **参数**: 
  - `info` - 输出参数
  - `internal` - 采样间隔（毫秒），默认 50ms
- **返回值**: `true` 表示成功

### 4.8 getMemory

```cpp
bool getMemory(Memory& memory);
```
- **功能**: 获取内存信息
- **参数**: `memory` - 输出参数
- **返回值**: `true` 表示成功

### 4.9 getDiskSpace

```cpp
bool getDiskSpace(DiskSpace& disk_space);
```
- **功能**: 获取磁盘空间信息
- **参数**: `disk_space` - 输出参数
- **返回值**: `true` 表示成功

### 4.10 getCurrentCPUArch

```cpp
CPU_Arch getCurrentCPUArch();
```
- **功能**: 获取当前 CPU 架构
- **返回值**: `CPU_Arch` 枚举值

---

## 6. FileSystem 类

### 5.1 类简介

提供静态文件系统操作函数，包括目录创建、文件复制移动删除、路径遍历等。

### 5.2 目录操作

#### chDir

```cpp
static bool chDir(const Path& path);
static bool chDir(const std::string& path);
```
- **功能**: 切换当前工作目录
- **参数**: `path` - 目标目录路径
- **返回值**: `true` 表示成功

#### mkDir

```cpp
static bool mkDir(const Path& path);
static bool mkDir(const std::string& path);
```
- **功能**: 创建目录
- **参数**: `path` - 目录路径
- **返回值**: `true` 表示成功
- **约束**: 父目录必须存在

### 5.3 文件操作

#### mkFile (重载)

```cpp
static bool mkFile(const Path& path, const std::vector<uint8_t>& data = {});
static bool mkFile(const std::string& path, const std::vector<uint8_t>& data = {});
static bool mkFile(const Path& path, const std::string& data);
static bool mkFile(const std::string& path, const std::string& data);
```
- **功能**: 创建文件（可附带初始内容）
- **参数**: 
  - `path` - 文件路径
  - `data` - 初始内容（可选）
- **返回值**: `true` 表示成功

#### mkLink

```cpp
static bool mkLink(const std::string& path, const std::string& link_dest);
static bool mkLink(const std::string& path, const Path& link_dest);
```
- **功能**: 创建符号链接
- **参数**: 
  - `path` - 链接路径
  - `link_dest` - 目标路径
- **返回值**: `true` 表示成功

#### cpFile

```cpp
static bool cpFile(const Path& src, const Path& dest);
static bool cpFile(const Path& src, const std::string& dest);
```
- **功能**: 复制文件
- **参数**: 
  - `src` - 源文件路径
  - `dest` - 目标路径
- **返回值**: `true` 表示成功

#### cpDir

```cpp
static bool cpDir(const Path& src, const Path& dest);
static bool cpDir(const Path& src, const std::string& dest);
```
- **功能**: 复制目录（递归）
- **参数**: 
  - `src` - 源目录路径
  - `dest` - 目标路径
- **返回值**: `true` 表示成功

#### mvFile

```cpp
static bool mvFile(const Path& src, const Path& dest);
static bool mvFile(const Path& src, const std::string& dest);
```
- **功能**: 移动/重命名文件
- **参数**: 
  - `src` - 源文件路径
  - `dest` - 目标路径
- **返回值**: `true` 表示成功

#### mvDir

```cpp
static bool mvDir(const Path& src, const Path& dest);
static bool mvDir(const Path& src, const std::string& dest);
```
- **功能**: 移动/重命名目录
- **参数**: 
  - `src` - 源目录路径
  - `dest` - 目标路径
- **返回值**: `true` 表示成功

#### rmFile

```cpp
static bool rmFile(const Path& path);
static bool rmFile(const std::string& path);
```
- **功能**: 删除文件
- **参数**: `path` - 文件路径
- **返回值**: `true` 表示成功

#### rmDir

```cpp
static bool rmDir(const Path& path, bool recursion = false);
static bool rmDir(const std::string& path, bool recursion = false);
```
- **功能**: 删除目录
- **参数**: 
  - `path` - 目录路径
  - `recursion` - 是否递归删除内容，默认 false
- **返回值**: `true` 表示成功
- **约束**: 非递归模式下目录必须为空

### 5.4 路径查询

#### currentPath

```cpp
static Path currentPath();
```
- **功能**: 获取当前工作目录
- **返回值**: 当前目录的 Path 对象

#### homePath

```cpp
static Path homePath();
```
- **功能**: 获取用户主目录
- **返回值**: 用户主目录的 Path 对象

#### cachePath

```cpp
static Path cachePath();
```
- **功能**: 获取用户缓存目录
- **返回值**: 缓存目录的 Path 对象（`~/.cache` 或 Windows 等效路径）

#### localDataPath

```cpp
static Path localDataPath();
```
- **功能**: 获取用户本地数据目录
- **返回值**: 本地数据目录的 Path 对象（`~/.local/share` 或 Windows 等效路径）

### 5.5 目录遍历

#### listPath (重载)

```cpp
static std::vector<Path> listPath(const Path& path, 
                                   uint8_t recursion_count = 1, 
                                   const std::function<bool(const Path&)>& filter = {});
static std::vector<Path> listPath(const std::string& path, 
                                   uint8_t recursion_count = 1, 
                                   const std::function<bool(const Path&)>& filter = {});
static std::vector<Path> listPath(uint8_t recursion_count = 1, 
                                   const std::function<bool(const Path&)>& filter = {});
```
- **功能**: 列出目录内容
- **参数**: 
  - `path` - 目标目录路径（可选，默认为当前目录）
  - `recursion_count` - 递归深度，0 或 255 表示无限，默认 1（仅当前层）
  - `filter` - 过滤函数（可选）
- **返回值**: Path 对象数组

---

## 7. 使用示例

### 6.1 系统信息获取示例

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    // 主机信息
    auto host = Tiny::OS::currentHostInfo();
    std::cout << "主机名: " << host.host_name << std::endl;
    std::cout << "用户名: " << host.user_name << std::endl;
    std::cout << "系统: " << host.os_name << " " << host.version << std::endl;
    
    // CPU 信息
    auto cpu = Tiny::OS::currentCPUInfo();
    std::cout << "架构: " << Tiny::OS::getCPUArchName(cpu.machine) << std::endl;
    std::cout << "核心数: " << cpu.cores << std::endl;
    std::cout << "使用率: " << cpu.total_usage << "%" << std::endl;
    
    // 内存信息
    auto mem = Tiny::OS::currentMemory();
    std::cout << "总内存: " << mem.total_ram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "可用内存: " << mem.available_ram / 1024 / 1024 << " MB" << std::endl;
    
    // 磁盘信息
    auto disk = Tiny::OS::currentDiskSpace();
    std::cout << "总容量: " << disk.total_bytes / 1024 / 1024 / 1024 << " GB" << std::endl;
    std::cout << "可用容量: " << disk.available_bytes / 1024 / 1024 / 1024 << " GB" << std::endl;
    
    return 0;
}
```

### 6.2 文件系统操作示例

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    using namespace Tiny::OS;
    
    // 创建目录
    FileSystem::mkDir("./test_dir");
    
    // 创建文件
    FileSystem::mkFile("./test_dir/file.txt", "Hello, World!");
    
    // 复制文件
    FileSystem::cpFile("./test_dir/file.txt", "./test_dir/file_copy.txt");
    
    // 列出目录
    auto files = FileSystem::listPath("./test_dir");
    for (const auto& f : files) {
        std::cout << f.shortFileName() << std::endl;
    }
    
    // 删除文件和目录
    FileSystem::rmFile("./test_dir/file.txt");
    FileSystem::rmFile("./test_dir/file_copy.txt");
    FileSystem::rmDir("./test_dir");
    
    // 获取特殊路径
    std::cout << "Home: " << FileSystem::homePath().path() << std::endl;
    std::cout << "Current: " << FileSystem::currentPath().path() << std::endl;
    
    return 0;
}
```

### 6.3 目录遍历示例

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    using namespace Tiny::OS;
    
    // 递归列出所有 .cpp 文件
    auto cpp_files = FileSystem::listPath("./src", 255, 
        [](const Path& p) {
            return p.extensionName() == "cpp";
        });
    
    std::cout << "找到 " << cpp_files.size() << " 个 .cpp 文件:" << std::endl;
    for (const auto& f : cpp_files) {
        std::cout << "  " << f.path() << std::endl;
    }
    
    // 仅遍历一层目录
    auto current_files = FileSystem::listPath("./src", 1);
    
    return 0;
}
```

### 6.4 监控 CPU 使用率示例

```cpp
#include "OS/System.hpp"
#include <iostream>
#include <thread>

int main() {
    Tiny::OS::CPU cpu;
    
    for (int i = 0; i < 10; ++i) {
        Tiny::OS::getCPUInfo(cpu, 500);  // 采样 500ms
        
        std::cout << "总体使用率: " << cpu.total_usage << "%" << std::endl;
        std::cout << "各核心使用率: ";
        for (size_t j = 0; j < cpu.usages.size(); ++j) {
            std::cout << "Core" << j << ":" << cpu.usages[j] << "% ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

---

## 8. 注意事项

### 7.1 CPU 使用率获取

- `getCPUInfo()` 需要采样间隔（默认 50ms）
- 调用会阻塞指定的时间
- 采样间隔越长，结果越准确
- Windows 使用性能计数器 API
- Unix/Linux 读取 `/proc/stat`
- macOS 使用 `host_statistics()`

### 7.2 内存信息差异

- **Windows**: 使用 `GlobalMemoryStatusEx()`
- **Linux**: 读取 `/proc/meminfo`
- **macOS**: 使用 `host_statistics64()`，提供额外的内存分类

### 7.3 文件系统操作权限

- 创建/删除目录需要写权限
- 跨文件系统移动可能失败
- 符号链接创建需要管理员权限（Windows）

### 7.4 路径遍历性能

- 递归遍历大量文件时可能较慢
- 使用过滤函数减少返回结果
- 深度优先遍历

### 7.5 特殊路径

| 路径 | Windows | Unix/Linux | macOS |
|------|---------|------------|-------|
| Home | `%USERPROFILE%` | `$HOME` | `$HOME` |
| Cache | `%LOCALAPPDATA%\cache` | `~/.cache` | `~/Library/Caches` |
| Local Data | `%LOCALAPPDATA%` | `~/.local/share` | `~/Library/Application Support` |

### 7.6 错误处理

- 所有函数返回 `bool` 指示成功/失败
- 不抛出异常
- 失败原因可通过系统错误码获取（平台相关）
