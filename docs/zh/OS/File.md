# OS::File 模块

命名空间: `Tiny::OS`

---

## 目录

1. [模块简介](#1-模块简介)
2. [头文件](#2-头文件)
3. [类型定义](#3-类型定义)
4. [枚举定义](#4-枚举定义)
5. [Path 类](#5-path-类)
6. [File 类](#6-file-类)
7. [使用示例](#7-使用示例)
8. [注意事项](#8-注意事项)

---

## 1. 模块简介

`OS::File` 模块提供跨平台的文件与路径操作功能，包含两个核心类：

- **Path 类**: 路径处理，支持路径解析、类型检测、路径拼接等
- **File 类**: 文件操作，支持打开、读取、写入、定位等

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/OS/File.hpp>
// 直接复制源代码方式
#include "OS/File.hpp"
```

---

## 3. 类型定义

```cpp
using FileData = std::vector<uint8_t>;
```

**说明**: 文件数据的字节数组类型，用于二进制读写。

---

## 4. 枚举定义

### 4.1 FileType 枚举

```cpp
enum class FileType : uint8_t {
    Unknown,      // 未知类型
    Directory,    // 目录
    File,         // 普通文件
    SymbolLink,   // 符号链接
    Device,       // 设备文件
    Socket        // 套接字
};
```

| 枚举值 | 数值 | 说明 |
|--------|------|------|
| `Unknown` | 0 | 路径无效或无法识别 |
| `Directory` | 1 | 目录/文件夹 |
| `File` | 2 | 普通文件 |
| `SymbolLink` | 3 | 符号链接/快捷方式 |
| `Device` | 4 | 设备文件 |
| `Socket` | 5 | 套接字文件 |

### fileTypeName 函数

```cpp
inline const char* fileTypeName(FileType type);
```
- **功能**: 获取文件类型名称字符串
- **参数**: `type` - 文件类型枚举值
- **返回值**: 类型名称字符串（如 "File"、"Directory"、"Symbol Link" 等）
- **示例**:
```cpp
Tiny::OS::Path path("./test.txt");
std::cout << "Type: " << Tiny::OS::fileTypeName(path.fileType()) << std::endl;
```

### 4.2 OpenMode 枚举

```cpp
enum OpenMode : uint8_t {
    Unknown   = 0,   // 未知/未指定
    ReadOnly  = 1,   // 只读模式
    WriteOnly = 2,   // 只写模式
    ReadWrite = 4,   // 读写模式
    Append    = 8    // 追加模式
};
```

**组合使用**:
```cpp
uint8_t mode = Tiny::OS::ReadOnly | Tiny::OS::Append;
```

### 4.3 容量单位字面量

```cpp
inline size_t operator""_B(unsigned long long n) noexcept;
inline size_t operator""_KB(unsigned long long n) noexcept;
inline size_t operator""_KiB(unsigned long long n) noexcept;
inline size_t operator""_MB(unsigned long long n) noexcept;
inline size_t operator""_MiB(unsigned long long n) noexcept;
inline size_t operator""_GB(unsigned long long n) noexcept;
inline size_t operator""_GiB(unsigned long long n) noexcept;
inline size_t operator""_TB(unsigned long long n) noexcept;
inline size_t operator""_TiB(unsigned long long n) noexcept;
```

| 字面量 | 换算系数 | 示例 |
|--------|----------|------|
| `_B` | 1 | `512_B` |
| `_KB` | 1000 | `5_KB` |
| `_KiB` | 1024 | `5_KiB` |
| `_MB` | 1000000 | `10_MB` |
| `_MiB` | 1048576 | `10_MiB` |
| `_GB` | 1000000000 | `2_GB` |
| `_GiB` | 1073741824 | `2_GiB` |
| `_TB` | 1000000000000 | `1_TB` |
| `_TiB` | 1099511627776 | `1_TiB` |

---

## 5. Path 类

### 5.1 类简介

跨平台路径处理类，支持路径解析、文件类型检测、路径拼接等操作。

### 5.2 构造函数

```cpp
Path(std::string path);           // 从字符串构造
Path(const Path& path);           // 拷贝构造
Path(Path&& path) noexcept;       // 移动构造
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `path` | `std::string` | 路径字符串 |

### 5.3 析构函数

```cpp
~Path();
```

### 5.4 赋值运算符

```cpp
Path& operator=(const Path& path);
Path& operator=(Path&& path) noexcept;
```

### 5.5 成员函数

#### setPath

```cpp
void setPath(const std::string& path);
void setPath(const Path& path);
```
- **功能**: 设置新路径
- **参数**: 新的路径字符串或 Path 对象
- **返回值**: 无
- **注意事项**: 会重新检测路径类型

#### unset

```cpp
void unset();
```
- **功能**: 清空路径信息
- **返回值**: 无

#### path

```cpp
[[nodiscard]] const std::string& path() const;
```
- **功能**: 获取完整路径
- **返回值**: 规范化后的绝对路径

#### extensionName

```cpp
[[nodiscard]] std::string extensionName() const;
```
- **功能**: 获取文件扩展名
- **返回值**: 扩展名（不含点号），目录返回空字符串
- **示例**: `/path/file.txt` → `"txt"`

#### fileNameWithoutExtension

```cpp
[[nodiscard]] std::string fileNameWithoutExtension() const;
```
- **功能**: 获取不含扩展名的文件名
- **返回值**: 文件名（不含扩展名）
- **示例**: `/path/file.txt` → `"file"`

#### shortFileName

```cpp
[[nodiscard]] const std::string& shortFileName() const;
```
- **功能**: 获取完整文件名（含扩展名）
- **返回值**: 文件名
- **示例**: `/path/file.txt` → `"file.txt"`

#### parentDirectory

```cpp
[[nodiscard]] std::string parentDirectory() const;
```
- **功能**: 获取父目录路径
- **返回值**: 父目录完整路径
- **示例**: `/path/to/file.txt` → `/path/to`

#### isValid

```cpp
[[nodiscard]] bool isValid() const;
```
- **功能**: 检查路径是否有效
- **返回值**: `true` 表示路径存在且可识别

#### isDirectory

```cpp
[[nodiscard]] bool isDirectory() const;
```
- **功能**: 检查是否为目录
- **返回值**: `true` 表示是目录

#### isFile

```cpp
[[nodiscard]] bool isFile() const;
```
- **功能**: 检查是否为普通文件
- **返回值**: `true` 表示是普通文件

#### isSymbolLink

```cpp
[[nodiscard]] bool isSymbolLink() const;
```
- **功能**: 检查是否为符号链接
- **返回值**: `true` 表示是符号链接

#### fileType

```cpp
[[nodiscard]] FileType fileType() const;
```
- **功能**: 获取文件类型
- **返回值**: `FileType` 枚举值

#### fileSize

```cpp
[[nodiscard]] size_t fileSize() const;
```
- **功能**: 获取文件大小
- **返回值**: 文件大小（字节），目录返回 0

#### operator/

```cpp
Path& operator/(const std::string& path);
```
- **功能**: 路径拼接运算符
- **参数**: `path` - 要拼接的子路径
- **返回值**: 自身引用（支持链式调用）
- **示例**: `path / "subdir" / "file.txt"`

#### join

```cpp
Path& join(const std::string& path);
```
- **功能**: 拼接路径（功能同 operator/）
- **参数**: `path` - 要拼接的子路径
- **返回值**: 自身引用

#### parent

```cpp
Path& parent();
```
- **功能**: 移动到父目录（保留末尾斜杠）
- **返回值**: 自身引用
- **示例**: `/path/to/file` → `/path/to/`

#### upper

```cpp
Path& upper();
```
- **功能**: 移动到上级目录（不保留斜杠）
- **返回值**: 自身引用
- **示例**: `/path/to/file` → `/path/to`

### 5.6 静态成员函数

#### exist

```cpp
static bool exist(const std::string& path);
```
- **功能**: 检查路径是否存在
- **参数**: `path` - 路径字符串
- **返回值**: `true` 表示存在

#### isDirectory (static)

```cpp
static bool isDirectory(const std::string& path);
```
- **功能**: 检查指定路径是否为目录
- **参数**: `path` - 路径字符串
- **返回值**: `true` 表示是目录

#### isFile (static)

```cpp
static bool isFile(const std::string& path);
```
- **功能**: 检查指定路径是否为文件
- **参数**: `path` - 路径字符串
- **返回值**: `true` 表示是文件

#### isSymbolLink (static)

```cpp
static bool isSymbolLink(const std::string& path);
```
- **功能**: 检查指定路径是否为符号链接
- **参数**: `path` - 路径字符串
- **返回值**: `true` 表示是符号链接

---

## 6. File 类

### 6.1 类简介

跨平台文件操作类，支持文件的打开、读取、写入、定位等操作。

### 6.2 构造函数

```cpp
File(const std::string& path, uint8_t open_mode = Unknown);
File(Path path, uint8_t open_mode = Unknown);
File(File&& file) noexcept;  // 移动构造
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `path` | `std::string` / `Path` | 文件路径 |
| `open_mode` | `uint8_t` | 打开模式（ReadOnly/WriteOnly/ReadWrite/Append） |

### 6.3 析构函数

```cpp
~File();
```
- 自动关闭文件句柄

### 6.4 拷贝控制

```cpp
File(const File&) = delete;              // 禁止拷贝
File& operator=(const File&) = delete;   // 禁止拷贝赋值
File& operator=(File&& file) noexcept;   // 允许移动赋值
```

### 6.5 成员函数

#### setPath

```cpp
void setPath(const std::string& path);
void setPath(const Path& path);
```
- **功能**: 更改文件路径
- **参数**: 新的路径
- **约束**: 文件打开时会先关闭原文件

#### isFile

```cpp
bool isFile() const;
```
- **功能**: 检查路径是否指向普通文件
- **返回值**: `true` 表示是文件

#### isNull

```cpp
bool isNull() const;
```
- **功能**: 检查文件对象是否没有有效路径
- **返回值**: `true` 表示路径为空或无效

#### isOpen

```cpp
bool isOpen() const;
```
- **功能**: 检查文件是否已打开
- **返回值**: `true` 表示已打开

#### open

```cpp
bool open(uint8_t open_mode);
```
- **功能**: 以指定模式打开文件
- **参数**: `open_mode` - 打开模式（ReadOnly/WriteOnly/ReadWrite/Append）
- **返回值**: `true` 表示成功
- **约束**: 文件已打开时会先关闭

#### read

```cpp
FileData read(size_t length);
```
- **功能**: 读取指定长度数据
- **参数**: `length` - 要读取的字节数
- **返回值**: 读取的数据（vector<uint8_t>）
- **约束**: 文件必须有效且已打开

#### readAll

```cpp
FileData readAll();
```
- **功能**: 读取整个文件内容
- **返回值**: 文件全部数据
- **约束**: 文件必须有效且已打开

#### readText

```cpp
std::string readText(size_t length);
```
- **功能**: 以文本形式读取指定长度
- **参数**: `length` - 字符数
- **返回值**: 读取的字符串

#### readLine

```cpp
std::string readLine();
```
- **功能**: 读取一行文本（到换行符为止）
- **返回值**: 行内容（包含换行符）

#### readAllText

```cpp
std::string readAllText();
```
- **功能**: 以文本形式读取整个文件
- **返回值**: 文件全部内容

#### write (重载)

```cpp
bool write(const FileData& data, size_t length);
bool write(const char* data, size_t length);
bool write(const std::string& string);
bool write(const FileData& data);
```
- **功能**: 写入数据到文件
- **参数**: 
  - `data` - 要写入的数据
  - `length` - 写入长度（部分重载）
- **返回值**: `true` 表示成功
- **约束**: 文件必须以写模式打开

#### writeLine

```cpp
bool writeLine(const std::string& string);
```
- **功能**: 写入一行文本（自动添加 CRLF）
- **参数**: `string` - 行内容
- **返回值**: `true` 表示成功

#### close

```cpp
void close();
```
- **功能**: 关闭文件
- **返回值**: 无

#### isEOF

```cpp
bool isEOF() const;
```
- **功能**: 检查是否到达文件末尾
- **返回值**: `true` 表示已到末尾

#### moveToStart

```cpp
void moveToStart();
```
- **功能**: 移动读写位置到文件开头

#### moveToEnd

```cpp
void moveToEnd();
```
- **功能**: 移动读写位置到文件末尾

#### moveTo

```cpp
void moveTo(int64_t pos);
```
- **功能**: 移动读写位置到绝对偏移处
- **参数**: `pos` - 相对于文件开头的字节偏移

#### fileSize

```cpp
[[nodiscard]] size_t fileSize() const;
```
- **功能**: 获取文件大小
- **返回值**: 文件大小（字节）

#### path

```cpp
[[nodiscard]] std::string path() const;
```
- **功能**: 获取文件路径
- **返回值**: 完整路径字符串

#### fileName

```cpp
[[nodiscard]] std::string fileName() const;
```
- **功能**: 获取文件名
- **返回值**: 文件名（含扩展名）

---

## 7. 使用示例

### 7.1 Path 类使用示例

```cpp
#include "OS/File.hpp"
#include <iostream>

int main() {
    Tiny::OS::Path path("./test.txt");
    
    if (path.isValid()) {
        std::cout << "完整路径: " << path.path() << std::endl;
        std::cout << "文件名: " << path.shortFileName() << std::endl;
        std::cout << "扩展名: " << path.extensionName() << std::endl;
        std::cout << "文件大小: " << path.fileSize() << " bytes" << std::endl;
    }
    
    // 路径拼接
    Tiny::OS::Path base("/home/user");
    base / "documents" / "file.txt";
    std::cout << "拼接后: " << base.path() << std::endl;
    
    // 静态检查
    if (Tiny::OS::Path::exist("/etc/passwd")) {
        std::cout << "文件存在" << std::endl;
    }
    
    return 0;
}
```

### 7.2 File 类使用示例

```cpp
#include "OS/File.hpp"
#include <iostream>

int main() {
    // 写入文件
    {
        Tiny::OS::File file("test.txt", Tiny::OS::WriteOnly);
        if (file.isOpen()) {
            file.writeLine("Hello, World!");
            file.write("Second line\r\n");
        }
    }  // 文件自动关闭
    
    // 读取文件
    {
        Tiny::OS::File file("test.txt", Tiny::OS::ReadOnly);
        if (file.isOpen()) {
            std::string content = file.readAllText();
            std::cout << content << std::endl;
        }
    }
    
    // 二进制读写
    {
        Tiny::OS::File file("data.bin", Tiny::OS::ReadWrite);
        if (file.isOpen()) {
            Tiny::OS::FileData data = {0x01, 0x02, 0x03, 0x04};
            file.write(data);
            
            file.moveToStart();
            auto read_data = file.read(4);
        }
    }
    
    return 0;
}
```

### 7.3 文件移动示例

```cpp
#include "OS/File.hpp"

int main() {
    Tiny::OS::File file("source.txt", Tiny::OS::ReadOnly);
    
    // 移动到另一个路径
    file.setPath("destination.txt");
    file.open(Tiny::OS::WriteOnly);
    file.write("New content");
    
    return 0;
}
```

---

## 8. 注意事项

### 8.1 文件句柄管理

- `File` 类禁止拷贝，使用移动语义传递
- 析构函数自动关闭文件
- 显式调用 `close()` 可提前释放资源

### 8.2 路径处理

- Windows 和 Unix 路径分隔符自动处理
- 支持绝对路径和相对路径
- 路径拼接自动添加分隔符

### 8.3 打开模式

| 模式 | 说明 | 文件不存在 | 文件已存在 |
|------|------|------------|------------|
| `ReadOnly` | 只读 | 失败 | 打开 |
| `WriteOnly` | 只写 | 创建 | 清空 |
| `ReadWrite` | 读写 | 创建 | 打开 |
| `Append` | 追加 | 创建 | 追加 |

### 8.4 错误处理

- 所有操作返回 `bool` 指示成功/失败
- 不抛出异常
- 可通过 `isValid()` 和 `isOpen()` 检查状态

### 8.5 编码问题

- 文件路径使用 UTF-8 编码
- 文本读写使用系统默认编码
- 二进制读写保持原始字节
