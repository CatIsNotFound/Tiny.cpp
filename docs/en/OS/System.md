# OS::System Module

Namespace: `Tiny::OS`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Data Structures](#3-data-structures)
4. [System Information Functions](#4-system-information-functions)
5. [FileSystem Class](#5-filesystem-class)
6. [Usage Examples](#6-usage-examples)
7. [Notes](#7-notes)

---

## 1. Module Overview

The `OS::System` module provides system information retrieval and file system operation functionality:

- **System Information**: CPU, memory, disk, host information retrieval
- **File System**: Directory creation, file copy/move/delete, path traversal

---

## 2. Header File

```cpp
#include "OS/System.hpp"
```

---

## 3. Data Structures

### 3.1 HostInfo Structure

```cpp
struct HostInfo {
    std::string host_name;   // Host name
    std::string user_name;   // Current user name
    std::string os_name;     // Operating system name
    std::string machine;     // Machine architecture
    std::string version;     // System version
};
```

| Field | Type | Description |
|-------|------|-------------|
| `host_name` | `std::string` | Computer name on the network |
| `user_name` | `std::string` | Current logged-in user name |
| `os_name` | `std::string` | Operating system name (e.g., "Windows 10", "Ubuntu") |
| `machine` | `std::string` | Hardware architecture (e.g., "x86_64") |
| `version` | `std::string` | Operating system version number |

### 3.2 CPU_Arch Enum

```cpp
enum class CPU_Arch {
    Unknown     = 0,
    X86         = 1,
    X86_64      = 2,
    ARM32       = 3,
    ARM64       = 4,
    LoongArch   = 5,  // Loongson architecture
    MIPS        = 6,  // MIPS architecture
    RISCV       = 7,  // RISC-V architecture
    IA64        = 8   // Intel Itanium
};
```

### 3.3 CPU Structure

```cpp
struct CPU {
    CPU_Arch machine;           // CPU architecture
    uint32_t cores;             // Number of cores
    uint32_t page_size;         // Memory page size (bytes)
    float total_usage;          // Total usage percentage
    std::vector<float> usages;  // Per-core usage
};
```

| Field | Type | Description |
|-------|------|-------------|
| `machine` | `CPU_Arch` | CPU architecture type |
| `cores` | `uint32_t` | Number of logical cores |
| `page_size` | `uint32_t` | System memory page size |
| `total_usage` | `float` | Total CPU usage (0-100) |
| `usages` | `std::vector<float>` | Usage of each core |

### 3.4 Memory Structure

```cpp
struct Memory {
    size_t total_ram;        // Total physical memory
    size_t free_ram;         // Free physical memory
    size_t available_ram;    // Available physical memory
    size_t used_ram;         // Used physical memory
    size_t total_swap;       // Total swap space
    size_t free_swap;        // Free swap space
    
    // macOS specific fields
    size_t app_free_mem;     // Application free memory
    size_t app_active_mem;   // Active memory
    size_t app_inactive_mem; // Inactive memory
    size_t app_wired_mem;    // Wired memory
    size_t app_compress_mem; // Compressed memory
    size_t app_speculative_mem; // Speculative memory
};
```

| Field | Type | Description | Platform Support |
|-------|------|-------------|------------------|
| `total_ram` | `size_t` | Total physical memory (bytes) | All |
| `free_ram` | `size_t` | Completely free memory (bytes) | All |
| `available_ram` | `size_t` | Available memory for applications (bytes) | All |
| `used_ram` | `size_t` | Used memory (bytes) | All |
| `total_swap` | `size_t` | Total swap space (bytes) | All |
| `free_swap` | `size_t` | Free swap space (bytes) | All |
| `app_*` | `size_t` | macOS specific memory statistics | macOS only |

### 3.5 DiskSpace Structure

```cpp
struct DiskSpace {
    size_t total_bytes;      // Total capacity
    size_t free_bytes;       // Free capacity
    size_t used_bytes;       // Used capacity
    size_t available_bytes;  // Available capacity (considering reserved space)
};
```

---

## 4. System Information Functions

### 4.1 getCPUArchName

```cpp
const char* getCPUArchName(CPU_Arch cpu_arch);
```
- **Function**: Get CPU architecture name string
- **Parameter**: `cpu_arch` - CPU architecture enum value
- **Return Value**: Architecture name (e.g., "x86_64", "arm64")

### 4.2 currentHostInfo

```cpp
HostInfo currentHostInfo();
```
- **Function**: Get current host information
- **Return Value**: `HostInfo` structure

### 4.3 currentCPUInfo

```cpp
CPU currentCPUInfo();
```
- **Function**: Get current CPU information
- **Return Value**: `CPU` structure
- **Notes**: Getting usage has a brief delay (about 50ms)

### 4.4 currentMemory

```cpp
Memory currentMemory();
```
- **Function**: Get current memory information
- **Return Value**: `Memory` structure

### 4.5 currentDiskSpace

```cpp
DiskSpace currentDiskSpace();
```
- **Function**: Get current disk space information
- **Return Value**: `DiskSpace` structure

### 4.6 getHostInfo

```cpp
bool getHostInfo(HostInfo& info);
```
- **Function**: Get host information to reference parameter
- **Parameter**: `info` - Output parameter
- **Return Value**: `true` means success

### 4.7 getCPUInfo

```cpp
bool getCPUInfo(CPU& info, size_t internal = 50);
```
- **Function**: Get CPU information
- **Parameter**: 
  - `info` - Output parameter
  - `internal` - Sampling interval (milliseconds), default 50ms
- **Return Value**: `true` means success

### 4.8 getMemory

```cpp
bool getMemory(Memory& memory);
```
- **Function**: Get memory information
- **Parameter**: `memory` - Output parameter
- **Return Value**: `true` means success

### 4.9 getDiskSpace

```cpp
bool getDiskSpace(DiskSpace& disk_space);
```
- **Function**: Get disk space information
- **Parameter**: `disk_space` - Output parameter
- **Return Value**: `true` means success

### 4.10 getCurrentCPUArch

```cpp
CPU_Arch getCurrentCPUArch();
```
- **Function**: Get current CPU architecture
- **Return Value**: `CPU_Arch` enum value

---

## 5. FileSystem Class

### 5.1 Class Overview

Provides static file system operation functions, including directory creation, file copy/move/delete, path traversal, etc.

### 5.2 Directory Operations

#### chDir

```cpp
static bool chDir(const Path& path);
static bool chDir(const std::string& path);
```
- **Function**: Change current working directory
- **Parameter**: `path` - Target directory path
- **Return Value**: `true` means success

#### mkDir

```cpp
static bool mkDir(const Path& path);
static bool mkDir(const std::string& path);
```
- **Function**: Create directory
- **Parameter**: `path` - Directory path
- **Return Value**: `true` means success
- **Constraint**: Parent directory must exist

### 5.3 File Operations

#### mkFile (overloads)

```cpp
static bool mkFile(const Path& path, const std::vector<uint8_t>& data = {});
static bool mkFile(const std::string& path, const std::vector<uint8_t>& data = {});
static bool mkFile(const Path& path, const std::string& data);
static bool mkFile(const std::string& path, const std::string& data);
```
- **Function**: Create file (with optional initial content)
- **Parameter**: 
  - `path` - File path
  - `data` - Initial content (optional)
- **Return Value**: `true` means success

#### mkLink

```cpp
static bool mkLink(const std::string& path, const std::string& link_dest);
static bool mkLink(const std::string& path, const Path& link_dest);
```
- **Function**: Create symbolic link
- **Parameter**: 
  - `path` - Link path
  - `link_dest` - Target path
- **Return Value**: `true` means success

#### cpFile

```cpp
static bool cpFile(const Path& src, const Path& dest);
static bool cpFile(const Path& src, const std::string& dest);
```
- **Function**: Copy file
- **Parameter**: 
  - `src` - Source file path
  - `dest` - Destination path
- **Return Value**: `true` means success

#### cpDir

```cpp
static bool cpDir(const Path& src, const Path& dest);
static bool cpDir(const Path& src, const std::string& dest);
```
- **Function**: Copy directory (recursive)
- **Parameter**: 
  - `src` - Source directory path
  - `dest` - Destination path
- **Return Value**: `true` means success

#### mvFile

```cpp
static bool mvFile(const Path& src, const Path& dest);
static bool mvFile(const Path& src, const std::string& dest);
```
- **Function**: Move/rename file
- **Parameter**: 
  - `src` - Source file path
  - `dest` - Destination path
- **Return Value**: `true` means success

#### mvDir

```cpp
static bool mvDir(const Path& src, const Path& dest);
static bool mvDir(const Path& src, const std::string& dest);
```
- **Function**: Move/rename directory
- **Parameter**: 
  - `src` - Source directory path
  - `dest` - Destination path
- **Return Value**: `true` means success

#### rmFile

```cpp
static bool rmFile(const Path& path);
static bool rmFile(const std::string& path);
```
- **Function**: Delete file
- **Parameter**: `path` - File path
- **Return Value**: `true` means success

#### rmDir

```cpp
static bool rmDir(const Path& path, bool recursion = false);
static bool rmDir(const std::string& path, bool recursion = false);
```
- **Function**: Delete directory
- **Parameter**: 
  - `path` - Directory path
  - `recursion` - Whether to recursively delete contents, default false
- **Return Value**: `true` means success
- **Constraint**: Directory must be empty in non-recursive mode

### 5.4 Path Queries

#### currentPath

```cpp
static Path currentPath();
```
- **Function**: Get current working directory
- **Return Value**: Path object of current directory

#### homePath

```cpp
static Path homePath();
```
- **Function**: Get user home directory
- **Return Value**: Path object of user home directory

#### cachePath

```cpp
static Path cachePath();
```
- **Function**: Get user cache directory
- **Return Value**: Path object of cache directory (`~/.cache` or Windows equivalent)

#### localDataPath

```cpp
static Path localDataPath();
```
- **Function**: Get user local data directory
- **Return Value**: Path object of local data directory (`~/.local/share` or Windows equivalent)

### 5.5 Directory Traversal

#### listPath (overloads)

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
- **Function**: List directory contents
- **Parameter**: 
  - `path` - Target directory path (optional, defaults to current directory)
  - `recursion_count` - Recursion depth, 0 or 255 means unlimited, default 1 (current level only)
  - `filter` - Filter function (optional)
- **Return Value**: Array of Path objects

---

## 6. Usage Examples

### 6.1 System Information Retrieval Example

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    // Host information
    auto host = Tiny::OS::currentHostInfo();
    std::cout << "Host name: " << host.host_name << std::endl;
    std::cout << "User name: " << host.user_name << std::endl;
    std::cout << "OS: " << host.os_name << " " << host.version << std::endl;
    
    // CPU information
    auto cpu = Tiny::OS::currentCPUInfo();
    std::cout << "Architecture: " << Tiny::OS::getCPUArchName(cpu.machine) << std::endl;
    std::cout << "Cores: " << cpu.cores << std::endl;
    std::cout << "Usage: " << cpu.total_usage << "%" << std::endl;
    
    // Memory information
    auto mem = Tiny::OS::currentMemory();
    std::cout << "Total RAM: " << mem.total_ram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Available RAM: " << mem.available_ram / 1024 / 1024 << " MB" << std::endl;
    
    // Disk information
    auto disk = Tiny::OS::currentDiskSpace();
    std::cout << "Total capacity: " << disk.total_bytes / 1024 / 1024 / 1024 << " GB" << std::endl;
    std::cout << "Available capacity: " << disk.available_bytes / 1024 / 1024 / 1024 << " GB" << std::endl;
    
    return 0;
}
```

### 6.2 File System Operations Example

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    using namespace Tiny::OS;
    
    // Create directory
    FileSystem::mkDir("./test_dir");
    
    // Create file
    FileSystem::mkFile("./test_dir/file.txt", "Hello, World!");
    
    // Copy file
    FileSystem::cpFile("./test_dir/file.txt", "./test_dir/file_copy.txt");
    
    // List directory
    auto files = FileSystem::listPath("./test_dir");
    for (const auto& f : files) {
        std::cout << f.shortFileName() << std::endl;
    }
    
    // Delete files and directory
    FileSystem::rmFile("./test_dir/file.txt");
    FileSystem::rmFile("./test_dir/file_copy.txt");
    FileSystem::rmDir("./test_dir");
    
    // Get special paths
    std::cout << "Home: " << FileSystem::homePath().path() << std::endl;
    std::cout << "Current: " << FileSystem::currentPath().path() << std::endl;
    
    return 0;
}
```

### 6.3 Directory Traversal Example

```cpp
#include "OS/System.hpp"
#include <iostream>

int main() {
    using namespace Tiny::OS;
    
    // Recursively list all .cpp files
    auto cpp_files = FileSystem::listPath("./src", 255, 
        [](const Path& p) {
            return p.extensionName() == "cpp";
        });
    
    std::cout << "Found " << cpp_files.size() << " .cpp files:" << std::endl;
    for (const auto& f : cpp_files) {
        std::cout << "  " << f.path() << std::endl;
    }
    
    // Only traverse one level
    auto current_files = FileSystem::listPath("./src", 1);
    
    return 0;
}
```

### 6.4 Monitor CPU Usage Example

```cpp
#include "OS/System.hpp"
#include <iostream>
#include <thread>

int main() {
    Tiny::OS::CPU cpu;
    
    for (int i = 0; i < 10; ++i) {
        Tiny::OS::getCPUInfo(cpu, 500);  // Sample 500ms
        
        std::cout << "Total usage: " << cpu.total_usage << "%" << std::endl;
        std::cout << "Per-core usage: ";
        for (size_t j = 0; j < cpu.usages.size(); ++j) {
            std::cout << "Core" << j << ":" << cpu.usages[j] << "% ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

---

## 7. Notes

### 7.1 CPU Usage Retrieval

- `getCPUInfo()` requires sampling interval (default 50ms)
- Call blocks for the specified time
- Longer sampling intervals yield more accurate results
- Windows uses Performance Counter API
- Unix/Linux reads `/proc/stat`
- macOS uses `host_statistics()`

### 7.2 Memory Information Differences

- **Windows**: Uses `GlobalMemoryStatusEx()`
- **Linux**: Reads `/proc/meminfo`
- **macOS**: Uses `host_statistics64()`, provides additional memory categories

### 7.3 File System Operation Permissions

- Creating/deleting directories requires write permission
- Cross-filesystem moves may fail
- Symbolic link creation requires administrator privileges (Windows)

### 7.4 Path Traversal Performance

- Recursive traversal of large numbers of files may be slow
- Use filter functions to reduce returned results
- Depth-first traversal

### 7.5 Special Paths

| Path | Windows | Unix/Linux | macOS |
|------|---------|------------|-------|
| Home | `%USERPROFILE%` | `$HOME` | `$HOME` |
| Cache | `%LOCALAPPDATA%\cache` | `~/.cache` | `~/Library/Caches` |
| Local Data | `%LOCALAPPDATA%` | `~/.local/share` | `~/Library/Application Support` |

### 7.6 Error Handling

- All functions return `bool` indicating success/failure
- No exceptions thrown
- Failure reasons can be obtained via system error codes (platform dependent)
