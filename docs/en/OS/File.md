# OS::File Module

Namespace: `Tiny::OS`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Enum Definitions](#4-enum-definitions)
5. [Path Class](#5-path-class)
6. [File Class](#6-file-class)
7. [Usage Examples](#7-usage-examples)
8. [Notes](#8-notes)

---

## 1. Module Overview

The `OS::File` module provides cross-platform file and path operation functionality, containing two core classes:

- **Path Class**: Path handling, supports path resolution, type detection, path concatenation, etc.
- **File Class**: File operations, supports open, read, write, seek, etc.

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/OS/File.hpp>
// Direct source copy method
#include "OS/File.hpp"
```

---

## 3. Type Definitions

```cpp
using FileData = std::vector<uint8_t>;
```

**Description**: Byte array type for file data, used for binary read/write.

---

## 4. Enum Definitions

### 4.1 FileType Enum

```cpp
enum class FileType : uint8_t {
    Unknown,      // Unknown type
    Directory,    // Directory
    File,         // Regular file
    SymbolLink,   // Symbolic link
    Device,       // Device file
    Socket        // Socket file
};
```

| Enum Value | Value | Description |
|------------|-------|-------------|
| `Unknown` | 0 | Path invalid or unrecognized |
| `Directory` | 1 | Directory/folder |
| `File` | 2 | Regular file |
| `SymbolLink` | 3 | Symbolic link/shortcut |
| `Device` | 4 | Device file |
| `Socket` | 5 | Socket file |

### fileTypeName Function

```cpp
inline const char* fileTypeName(FileType type);
```
- **Function**: Get file type name string
- **Parameter**: `type` - File type enum value
- **Return Value**: Type name string (e.g., "File", "Directory", "Symbol Link", etc.)
- **Example**:
```cpp
Tiny::OS::Path path("./test.txt");
std::cout << "Type: " << Tiny::OS::fileTypeName(path.fileType()) << std::endl;
```

### 4.2 OpenMode Enum

```cpp
enum OpenMode : uint8_t {
    Unknown   = 0,   // Unknown/not specified
    ReadOnly  = 1,   // Read-only mode
    WriteOnly = 2,   // Write-only mode
    ReadWrite = 4,   // Read-write mode
    Append    = 8    // Append mode
};
```

**Combined Usage**:
```cpp
uint8_t mode = Tiny::OS::ReadOnly | Tiny::OS::Append;
```

### 4.3 Size Unit Literals

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

| Literal | Factor | Example |
|---------|--------|---------|
| `_B` | 1 | `512_B` |
| `_KB` | 1000 | `5_KB` |
| `_KiB` | 1024 | `5_KiB` |
| `_MB` | 1000000 | `10_MB` |
| `_MiB` | 1048576 | `10_MiB` |
| `_GB` | 1000000000 | `2_GB` |
| `_GiB` | 1073741824 | `2_GiB` |
| `_TB` | 1000000000000 | `1_TB` |
| `_TiB` | 1099511627776 | `1_TiB` |

### 4.4 DataUnit Enum

```cpp
enum class DataUnit : int8_t {
    B,
    KiB,
    MiB,
    GiB,
    TiB
};
```

| Enum Value | Value | Description |
|------------|-------|-------------|
| `B` | 0 | Bytes |
| `KiB` | 1 | Kibibytes (1024 bytes) |
| `MiB` | 2 | Mebibytes (1024² bytes) |
| `GiB` | 3 | Gibibytes (1024³ bytes) |
| `TiB` | 4 | Tebibytes (1024⁴ bytes) |

### convertDataSize Function

```cpp
inline double convertDataSize(size_t size, DataUnit dst_unit, DataUnit src_unit = DataUnit::B);
```
- **Function**: Convert data size between different units
- **Parameters**:
  - `size` - Size value to convert
  - `dst_unit` - Target unit
  - `src_unit` - Source unit (default: `DataUnit::B`)
- **Return Value**: Converted size value
- **Example**:
```cpp
double size_in_mb = Tiny::OS::convertDataSize(1048576, Tiny::OS::DataUnit::MiB);
// Returns: 1.0 (1048576 bytes = 1 MiB)
```

---

## 5. Path Class

### 5.1 Class Overview

Cross-platform path handling class, supports path resolution, file type detection, path concatenation, and other operations.

### 5.2 Constructors

```cpp
Path(std::string path);           // Construct from string
Path(const Path& path);           // Copy constructor
Path(Path&& path) noexcept;       // Move constructor
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string` | Path string |

### 5.3 Destructor

```cpp
~Path();
```

### 5.4 Assignment Operators

```cpp
Path& operator=(const Path& path);
Path& operator=(Path&& path) noexcept;
```

### 5.5 Member Functions

#### setPath

```cpp
void setPath(const std::string& path);
void setPath(const Path& path);
```
- **Function**: Set new path
- **Parameter**: New path string or Path object
- **Return Value**: None
- **Notes**: Re-detects path type

#### unset

```cpp
void unset();
```
- **Function**: Clear path information
- **Return Value**: None

#### path

```cpp
[[nodiscard]] const std::string& path() const;
```
- **Function**: Get full path
- **Return Value**: Normalized absolute path

#### extensionName

```cpp
[[nodiscard]] std::string extensionName() const;
```
- **Function**: Get file extension
- **Return Value**: Extension (without dot), empty string for directories
- **Example**: `/path/file.txt` → `"txt"`

#### fileNameWithoutExtension

```cpp
[[nodiscard]] std::string fileNameWithoutExtension() const;
```
- **Function**: Get file name without extension
- **Return Value**: File name (without extension)
- **Example**: `/path/file.txt` → `"file"`

#### shortFileName

```cpp
[[nodiscard]] const std::string& shortFileName() const;
```
- **Function**: Get full file name (with extension)
- **Return Value**: File name
- **Example**: `/path/file.txt` → `"file.txt"`

#### parentDirectory

```cpp
[[nodiscard]] std::string parentDirectory() const;
```
- **Function**: Get parent directory path
- **Return Value**: Full parent directory path
- **Example**: `/path/to/file.txt` → `/path/to`

#### isValid

```cpp
[[nodiscard]] bool isValid() const;
```
- **Function**: Check if path is valid
- **Return Value**: `true` means path exists and is recognizable

#### isDirectory

```cpp
[[nodiscard]] bool isDirectory() const;
```
- **Function**: Check if it is a directory
- **Return Value**: `true` means it is a directory

#### isFile

```cpp
[[nodiscard]] bool isFile() const;
```
- **Function**: Check if it is a regular file
- **Return Value**: `true` means it is a regular file

#### isSymbolLink

```cpp
[[nodiscard]] bool isSymbolLink() const;
```
- **Function**: Check if it is a symbolic link
- **Return Value**: `true` means it is a symbolic link

#### fileType

```cpp
[[nodiscard]] FileType fileType() const;
```
- **Function**: Get file type
- **Return Value**: `FileType` enum value

#### fileSize

```cpp
[[nodiscard]] size_t fileSize() const;
```
- **Function**: Get file size
- **Return Value**: File size in bytes, 0 for directories

#### operator/

```cpp
Path& operator/(const std::string& path);
```
- **Function**: Path concatenation operator
- **Parameter**: `path` - Sub-path to concatenate
- **Return Value**: Reference to self (supports chaining)
- **Example**: `path / "subdir" / "file.txt"`

#### join

```cpp
Path& join(const std::string& path);
```
- **Function**: Concatenate path (same as operator/)
- **Parameter**: `path` - Sub-path to concatenate
- **Return Value**: Reference to self

#### parent

```cpp
Path& parent();
```
- **Function**: Move to parent directory (keep trailing slash)
- **Return Value**: Reference to self
- **Example**: `/path/to/file` → `/path/to/`

#### upper

```cpp
Path& upper();
```
- **Function**: Move to parent directory (no trailing slash)
- **Return Value**: Reference to self
- **Example**: `/path/to/file` → `/path/to`

### 5.6 Static Member Functions

#### exist

```cpp
static bool exist(const std::string& path);
```
- **Function**: Check if path exists
- **Parameter**: `path` - Path string
- **Return Value**: `true` means exists

#### isDirectory (static)

```cpp
static bool isDirectory(const std::string& path);
```
- **Function**: Check if specified path is a directory
- **Parameter**: `path` - Path string
- **Return Value**: `true` means it is a directory

#### isFile (static)

```cpp
static bool isFile(const std::string& path);
```
- **Function**: Check if specified path is a file
- **Parameter**: `path` - Path string
- **Return Value**: `true` means it is a file

#### isSymbolLink (static)

```cpp
static bool isSymbolLink(const std::string& path);
```
- **Function**: Check if specified path is a symbolic link
- **Parameter**: `path` - Path string
- **Return Value**: `true` means it is a symbolic link

---

## 6. File Class

### 6.1 Class Overview

Cross-platform file operation class, supports file open, read, write, seek, and other operations.

### 6.2 Constructors

```cpp
File(const std::string& path, uint8_t open_mode = Unknown);
File(Path path, uint8_t open_mode = Unknown);
File(File&& file) noexcept;  // Move constructor
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `std::string` / `Path` | File path |
| `open_mode` | `uint8_t` | Open mode (ReadOnly/WriteOnly/ReadWrite/Append) |

### 6.3 Destructor

```cpp
~File();
```
- Automatically closes file handle

### 6.4 Copy Control

```cpp
File(const File&) = delete;              // Copy prohibited
File& operator=(const File&) = delete;   // Copy assignment prohibited
File& operator=(File&& file) noexcept;   // Move assignment allowed
```

### 6.5 Member Functions

#### setPath

```cpp
void setPath(const std::string& path);
void setPath(const Path& path);
```
- **Function**: Change file path
- **Parameter**: New path
- **Constraint**: Closes original file if open

#### isFile

```cpp
bool isFile() const;
```
- **Function**: Check if the path points to a regular file
- **Return Value**: `true` means it is a file

#### isNull

```cpp
bool isNull() const;
```
- **Function**: Check if the file object has no valid path
- **Return Value**: `true` means the path is empty or invalid

#### isOpen

```cpp
bool isOpen() const;
```
- **Function**: Check if file is open
- **Return Value**: `true` means open

#### open

```cpp
bool open(uint8_t open_mode);
```
- **Function**: Open file in specified mode
- **Parameter**: `open_mode` - Open mode (ReadOnly/WriteOnly/ReadWrite/Append)
- **Return Value**: `true` means success
- **Constraint**: Closes file first if already open

#### read

```cpp
FileData read(size_t length);
```
- **Function**: Read specified length of data
- **Parameter**: `length` - Number of bytes to read
- **Return Value**: Read data (vector<uint8_t>)
- **Constraint**: File must be valid and open

#### readAll

```cpp
FileData readAll();
```
- **Function**: Read entire file content
- **Return Value**: All file data
- **Constraint**: File must be valid and open

#### readText

```cpp
std::string readText(size_t length);
```
- **Function**: Read specified length as text
- **Parameter**: `length` - Number of characters
- **Return Value**: Read string

#### readLine

```cpp
std::string readLine();
```
- **Function**: Read one line of text (until newline)
- **Return Value**: Line content (including newline)

#### readAllText

```cpp
std::string readAllText();
```
- **Function**: Read entire file as text
- **Return Value**: All file content

#### write (overloads)

```cpp
bool write(const FileData& data, size_t length);
bool write(const char* data, size_t length);
bool write(const std::string& string);
bool write(const FileData& data);
```
- **Function**: Write data to file
- **Parameter**: 
  - `data` - Data to write
  - `length` - Write length (some overloads)
- **Return Value**: `true` means success
- **Constraint**: File must be opened in write mode

#### writeLine

```cpp
bool writeLine(const std::string& string);
```
- **Function**: Write one line of text (automatically adds CRLF)
- **Parameter**: `string` - Line content
- **Return Value**: `true` means success

#### close

```cpp
void close();
```
- **Function**: Close file
- **Return Value**: None

#### isEOF

```cpp
bool isEOF() const;
```
- **Function**: Check if end of file is reached
- **Return Value**: `true` means end reached

#### moveToStart

```cpp
void moveToStart();
```
- **Function**: Move read/write position to file start

#### moveToEnd

```cpp
void moveToEnd();
```
- **Function**: Move read/write position to file end

#### moveTo

```cpp
void moveTo(int64_t pos);
```
- **Function**: Move read/write position to absolute offset
- **Parameter**: `pos` - Byte offset from file start

#### fileSize

```cpp
[[nodiscard]] size_t fileSize() const;
```
- **Function**: Get file size
- **Return Value**: File size in bytes

#### path

```cpp
[[nodiscard]] std::string path() const;
```
- **Function**: Get file path
- **Return Value**: Full path string

#### fileName

```cpp
[[nodiscard]] std::string fileName() const;
```
- **Function**: Get file name
- **Return Value**: File name (with extension)

---

## 7. Usage Examples

### 7.1 Path Class Usage Example

```cpp
#include "OS/File.hpp"
#include <iostream>

int main() {
    Tiny::OS::Path path("./test.txt");
    
    if (path.isValid()) {
        std::cout << "Full path: " << path.path() << std::endl;
        std::cout << "File name: " << path.shortFileName() << std::endl;
        std::cout << "Extension: " << path.extensionName() << std::endl;
        std::cout << "File size: " << path.fileSize() << " bytes" << std::endl;
    }
    
    // Path concatenation
    Tiny::OS::Path base("/home/user");
    base / "documents" / "file.txt";
    std::cout << "Concatenated: " << base.path() << std::endl;
    
    // Static check
    if (Tiny::OS::Path::exist("/etc/passwd")) {
        std::cout << "File exists" << std::endl;
    }
    
    return 0;
}
```

### 7.2 File Class Usage Example

```cpp
#include "OS/File.hpp"
#include <iostream>

int main() {
    // Write file
    {
        Tiny::OS::File file("test.txt", Tiny::OS::WriteOnly);
        if (file.isOpen()) {
            file.writeLine("Hello, World!");
            file.write("Second line\r\n");
        }
    }  // File automatically closes
    
    // Read file
    {
        Tiny::OS::File file("test.txt", Tiny::OS::ReadOnly);
        if (file.isOpen()) {
            std::string content = file.readAllText();
            std::cout << content << std::endl;
        }
    }
    
    // Binary read/write
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

### 7.3 File Move Example

```cpp
#include "OS/File.hpp"

int main() {
    Tiny::OS::File file("source.txt", Tiny::OS::ReadOnly);
    
    // Move to another path
    file.setPath("destination.txt");
    file.open(Tiny::OS::WriteOnly);
    file.write("New content");
    
    return 0;
}
```

---

## 8. Notes

### 8.1 File Handle Management

- `File` class prohibits copying, use move semantics for transfer
- Destructor automatically closes file
- Explicitly call `close()` to release resources early

### 8.2 Path Handling

- Windows and Unix path separators are automatically handled
- Supports absolute and relative paths
- Path concatenation automatically adds separators

### 8.3 Open Modes

| Mode | Description | File Not Exists | File Exists |
|------|-------------|-----------------|-------------|
| `ReadOnly` | Read-only | Fail | Open |
| `WriteOnly` | Write-only | Create | Truncate |
| `ReadWrite` | Read-write | Create | Open |
| `Append` | Append | Create | Append |

### 8.4 Error Handling

- All operations return `bool` indicating success/failure
- No exceptions thrown
- Status can be checked via `isValid()` and `isOpen()`

### 8.5 Encoding Issues

- File paths use UTF-8 encoding
- Text read/write uses system default encoding
- Binary read/write preserves raw bytes
