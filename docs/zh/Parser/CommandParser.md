# Parser 模块 - CommandParser 类

命名空间: `Tiny`

---

## 目录

1. [类简介](#1-类简介)
2. [头文件](#2-头文件)
3. [类型定义](#3-类型定义)
4. [数据结构](#4-数据结构)
5. [构造函数与析构函数](#5-构造函数与析构函数)
6. [成员函数](#6-成员函数)
7. [静态成员函数](#7-静态成员函数)
8. [使用示例](#8-使用示例)
9. [注意事项](#9-注意事项)

---

## 1. 类简介

`CommandParser` 类提供命令行参数解析功能，支持 GNU 风格的长短选项、带值参数、默认值设置等功能。

### 核心特性

- **长短选项**: 支持 `--long` 和 `-s` 两种格式
- **带值参数**: 支持 `--option value` 和 `--option=value` 格式
- **默认值**: 可为选项设置默认值
- **仅长选项**: 支持仅长格式的选项
- **默认命令**: 支持无选项前缀的默认命令

---

## 2. 头文件

```cpp
#include "Parser/CommandParser.hpp"
```

---

## 3. 类型定义

```cpp
using iter = std::unordered_map<std::string, Command>::iterator;
using constIter = std::unordered_map<std::string, Command>::const_iterator;
```

---

## 4. 数据结构

### 4.1 Command 结构体

```cpp
struct Command {
    std::string option_name;       // 选项完整名称
    std::string short_options;     // 短选项字符（如 "h" 对应 -h）
    std::string description;       // 选项描述
    bool full_option_only;         // 是否仅支持长选项
    bool is_default_command;       // 是否为默认命令
    bool has_value;                // 是否接受值
    std::string value;             // 实际传入的值
    std::string default_value;     // 默认值
};
```

| 字段 | 类型 | 说明 |
|------|------|------|
| `option_name` | `std::string` | 长选项名称（如 "help"） |
| `short_options` | `std::string` | 短选项字符集合 |
| `description` | `std::string` | 选项说明文本 |
| `full_option_only` | `bool` | true 表示不支持短选项 |
| `is_default_command` | `bool` | 是否为默认执行命令 |
| `has_value` | `bool` | 该选项是否需要参数值 |
| `value` | `std::string` | 解析后获得的实际值 |
| `default_value` | `std::string` | 未提供值时的默认值 |

### 4.2 ParseError 枚举

```cpp
enum class ParseError : uint8_t {
    NoError,           // 无错误
    UnknownOption,     // 未知选项
    FullOptionOnly,    // 仅支持长选项但使用了短选项
    InvalidValue,      // 无效值
    MissingArgument,   // 缺少参数
    FormatError        // 格式错误
};
```

| 枚举值 | 数值 | 说明 |
|--------|------|------|
| `NoError` | 0 | 解析成功 |
| `UnknownOption` | 1 | 遇到未定义的选项 |
| `FullOptionOnly` | 2 | 对仅长选项使用了短格式 |
| `InvalidValue` | 3 | 参数值无效或为空 |
| `MissingArgument` | 4 | 缺少必需的参数值 |
| `FormatError` | 5 | 参数格式错误 |

---

## 5. 构造函数与析构函数

### 5.1 构造函数

```cpp
CommandParser(int argc, char** argv);
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `argc` | `int` | 参数个数 |
| `argv` | `char**` | 参数数组 |

### 5.2 析构函数

```cpp
~CommandParser() = default;
```

---

## 6. 成员函数

### 6.1 添加命令

#### addCommand

```cpp
bool addCommand(const std::string& command_name, 
                const std::string& short_options, 
                const std::string& description = {}, 
                bool has_value = false, 
                const std::string& default_value = {}, 
                bool default_command = false);
```
- **功能**: 添加命令选项
- **参数**: 
  - `command_name` - 长选项名称（如 "help"）
  - `short_options` - 短选项字符（如 "h"）
  - `description` - 描述信息
  - `has_value` - 是否接受值
  - `default_value` - 默认值
  - `default_command` - 是否为默认命令
- **返回值**: `true` 表示成功（名称不重复）

#### addFullCommand

```cpp
bool addFullCommand(const std::string& command_name, 
                    const std::string& description, 
                    bool has_value = false, 
                    const std::string& default_value = {}, 
                    bool default_command = false);
```
- **功能**: 添加仅支持长格式的命令
- **参数**: 同 `addCommand`（无 short_options）
- **返回值**: `true` 表示成功

### 6.2 移除命令

#### remove

```cpp
bool remove(const std::string& command_name);
```
- **功能**: 移除已添加的命令
- **参数**: `command_name` - 命令名称
- **返回值**: `true` 表示成功移除

#### clear

```cpp
void clear();
```
- **功能**: 清空所有命令定义
- **返回值**: 无

### 6.3 执行解析

#### exec

```cpp
ParseError exec(int* parsed_command_count = nullptr,
                int* err_arg_n = nullptr,
                const char* err_cmd_name = nullptr);
```
- **功能**: 执行参数解析
- **参数**: 
  - `parsed_command_count` - 输出解析成功的命令数
  - `err_arg_n` - 输出错误参数位置
  - `err_cmd_name` - 输出错误命令名称
- **返回值**: `ParseError` 枚举值

#### execCommandList

```cpp
const std::vector<Command>& execCommandList() const;
```
- **功能**: 获取解析后的命令列表
- **返回值**: 命令数组的常量引用

### 6.4 查询函数

#### size

```cpp
size_t size() const;
```
- **功能**: 获取已添加的命令数量
- **返回值**: 命令数

#### exist

```cpp
bool exist(const std::string& command_name) const;
```
- **功能**: 检查命令是否存在
- **参数**: `command_name` - 命令名称
- **返回值**: `true` 表示存在

#### at

```cpp
const Command& at(const std::string& command_name) const;
```
- **功能**: 获取命令常量引用
- **参数**: `command_name` - 命令名称
- **返回值**: 命令常量引用
- **异常**: 命令不存在时抛出异常

#### get

```cpp
Command& get(const std::string& command_name);
```
- **功能**: 获取命令引用
- **参数**: `command_name` - 命令名称
- **返回值**: 命令引用

#### operator[]

```cpp
Command& operator[](const std::string& command_name);
```
- **功能**: 获取命令引用（同 get）
- **参数**: `command_name` - 命令名称
- **返回值**: 命令引用

### 6.5 迭代器

#### begin / end

```cpp
iter begin();
iter end();
```
- **功能**: 获取迭代器（支持范围 for 循环）

#### cbegin / cend

```cpp
constIter cbegin() const;
constIter cend() const;
```
- **功能**: 获取常量迭代器

---

## 7. 静态成员函数

### 7.1 getParseErrorName

```cpp
static const char* getParseErrorName(ParseError error);
```
- **功能**: 获取错误码描述字符串
- **参数**: `error` - 错误枚举值
- **返回值**: 错误描述字符串

---

## 8. 使用示例

### 8.1 基本使用示例

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // 添加选项
    parser.addCommand("help", "h", "显示帮助信息");
    parser.addCommand("version", "v", "显示版本号");
    parser.addCommand("output", "o", "输出文件", true, "output.txt");
    parser.addFullCommand("verbose", "详细输出", false, "", true);
    
    // 解析参数
    int parsed_count = 0;
    int err_pos = 0;
    auto err = parser.exec(&parsed_count, &err_pos);
    
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "解析错误: " << Tiny::CommandParser::getParseErrorName(err) 
                  << " at position " << err_pos << std::endl;
        return 1;
    }
    
    // 检查选项
    if (parser.exist("help")) {
        std::cout << "Usage: myapp [options]" << std::endl;
        return 0;
    }
    
    if (parser.exist("version")) {
        std::cout << "Version 1.0.0" << std::endl;
        return 0;
    }
    
    // 获取带值选项
    if (parser.exist("output")) {
        const auto& cmd = parser["output"];
        std::cout << "输出文件: " << cmd.value << std::endl;
    }
    
    // 遍历解析结果
    for (const auto& cmd : parser.execCommandList()) {
        std::cout << "命令: " << cmd.option_name << std::endl;
    }
    
    return 0;
}
```

### 8.2 复杂命令行工具示例

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // 定义所有选项
    parser.addCommand("help", "h", "显示帮助信息");
    parser.addCommand("input", "i", "输入文件", true);
    parser.addCommand("output", "o", "输出文件", true, "output.txt");
    parser.addCommand("format", "f", "输出格式", true, "txt");
    parser.addCommand("verbose", "v", "详细输出");
    parser.addFullCommand("force", "强制覆盖");
    
    // 解析
    auto err = parser.exec();
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "错误: " << Tiny::CommandParser::getParseErrorName(err) << std::endl;
        return 1;
    }
    
    // 显示帮助
    if (parser.exist("help")) {
        std::cout << "文件转换工具\n\n";
        std::cout << "用法: converter [选项]\n\n";
        std::cout << "选项:\n";
        for (const auto& [name, cmd] : parser) {
            std::cout << "  --" << cmd.option_name;
            if (!cmd.short_options.empty()) {
                std::cout << ", -" << cmd.short_options;
            }
            std::cout << "\t" << cmd.description;
            if (!cmd.default_value.empty()) {
                std::cout << " (默认: " << cmd.default_value << ")";
            }
            std::cout << "\n";
        }
        return 0;
    }
    
    // 检查必需参数
    if (!parser.exist("input")) {
        std::cerr << "错误: 必须指定输入文件 (-i)" << std::endl;
        return 1;
    }
    
    // 获取参数值
    std::string input_file = parser["input"].value;
    std::string output_file = parser["output"].value;
    std::string format = parser["format"].value;
    bool verbose = parser.exist("verbose");
    bool force = parser.exist("force");
    
    if (verbose) {
        std::cout << "输入: " << input_file << std::endl;
        std::cout << "输出: " << output_file << std::endl;
        std::cout << "格式: " << format << std::endl;
    }
    
    // 执行转换...
    
    return 0;
}
```

### 8.3 子命令风格示例

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // 使用默认命令实现子命令风格
    parser.addFullCommand("init", "初始化项目", false, "", true);
    parser.addFullCommand("build", "构建项目", false, "", true);
    parser.addFullCommand("clean", "清理项目", false, "", true);
    parser.addCommand("config", "c", "配置文件", true, ".projectrc");
    
    auto err = parser.exec();
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "错误: " << Tiny::CommandParser::getParseErrorName(err) << std::endl;
        return 1;
    }
    
    // 检查子命令
    if (parser.exist("init")) {
        std::cout << "初始化项目..." << std::endl;
    } else if (parser.exist("build")) {
        std::cout << "构建项目..." << std::endl;
    } else if (parser.exist("clean")) {
        std::cout << "清理项目..." << std::endl;
    } else {
        std::cout << "请指定子命令: init, build, clean" << std::endl;
    }
    
    return 0;
}
```

---

## 9. 注意事项

### 9.1 支持的命令格式

```bash
# 短选项
myapp -h
myapp -v
myapp -o file.txt

# 长选项
myapp --help
myapp --version
myapp --output file.txt
myapp --output=file.txt

# 组合短选项
myapp -hv
myapp -vo file.txt

# 仅长选项（使用 addFullCommand 添加）
myapp verbose
```

### 9.2 错误处理

| 错误类型 | 触发条件 | 解决方式 |
|----------|----------|----------|
| `UnknownOption` | 遇到未定义的选项 | 检查命令定义，确认选项名称正确 |
| `FullOptionOnly` | 对仅长选项使用了短格式 | 使用 `--option` 而非 `-o` |
| `InvalidValue` | 参数值无效或为空 | 检查参数值格式，提供有效值 |
| `MissingArgument` | 缺少必需的参数值 | 为需要值的选项提供参数 |
| `FormatError` | 参数格式错误 | 检查参数格式，确保以 `-` 开头 |

### 9.3 默认值处理

- 如果选项设置了 `has_value = true` 但未提供值，使用 `default_value`
- 如果选项未设置默认值且未提供值，触发 `InvalidValue` 错误
- 默认值在解析时填充到 `Command::value` 中

### 9.4 命令名称冲突

- 长选项名称必须唯一
- 短选项字符可以重复（多个长选项共享一个短选项）
- `addCommand` 和 `addFullCommand` 都会检查名称冲突

### 9.5 迭代器使用

```cpp
// 范围 for 循环
for (const auto& [name, cmd] : parser) {
    std::cout << name << ": " << cmd.description << std::endl;
}

// 传统迭代器
for (auto it = parser.cbegin(); it != parser.cend(); ++it) {
    std::cout << it->first << ": " << it->second.description << std::endl;
}
```

### 9.6 性能考虑

- 命令查找使用哈希表，时间复杂度 O(1)
- 解析过程线性扫描参数列表
- 适合中小型命令行工具（< 100 个选项）
