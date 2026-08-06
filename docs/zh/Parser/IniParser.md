# Parser 模块 - IniParser 类

命名空间: `Tiny`

---

## 目录

1. [类概述](#1-类概述)
2. [头文件](#2-头文件)
3. [类型定义](#3-类型定义)
4. [数据结构](#4-数据结构)
5. [构造函数与析构函数](#5-构造函数与析构函数)
6. [成员函数](#6-成员函数)
7. [使用示例](#7-使用示例)
8. [注意事项](#8-注意事项)

---

## 1. 类概述

`IniParser` 类提供 INI 配置文件解析和操作功能，支持基于分组的键值对存储、从字符串解析以及将数据转储回 INI 格式。

### 核心特性

- **分组组织**: 支持将键值对组织到命名的分组（节）中
- **字符串解析**: 可将 INI 格式的字符串解析为结构化数据
- **序列化**: 可将内部数据结构转储回 INI 格式字符串
- **灵活访问**: 提供多种方式来访问和修改配置数据
- **默认分组**: 支持使用默认 "ungrouped" 节的未分组键

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/Parser/IniParser.hpp>
// 直接源码拷贝方式
#include "Parser/IniParser.hpp"
```

---

## 3. 类型定义

```cpp
using IniConf = std::pair<std::string, std::string>;
using IniGroup = std::vector<IniConf>;
using IniMap  = std::unordered_map<std::string, IniGroup>;
```

| 类型别名 | 底层类型 | 描述 |
|----------|----------|------|
| `IniConf` | `std::pair<std::string, std::string>` | 单个键值对 |
| `IniGroup` | `std::vector<IniConf>` | 分组中的键值对集合 |
| `IniMap` | `std::unordered_map<std::string, IniGroup>` | 分组名称到其键值对的映射 |

---

## 4. 数据结构

### 4.1 IniParserError 枚举

```cpp
enum class IniParserError : uint8_t {
    Success,           // 操作成功
    InvalidCharacter,  // 遇到无效字符
    InvalidFormat      // 无效格式
};
```

| 枚举值 | 值 | 描述 |
|--------|-----|------|
| `Success` | 0 | 解析或操作成功完成 |
| `InvalidCharacter` | 1 | INI 内容中遇到无效字符 |
| `InvalidFormat` | 2 | INI 格式无效或格式错误 |

---

## 5. 构造函数与析构函数

### 5.1 默认构造函数

```cpp
IniParser();
```
- **功能**: 创建一个使用默认分组名 "ungrouped" 的 IniParser
- **参数**: 无

### 5.2 带分组名构造函数

```cpp
IniParser(const std::string& group_name);
```
- **功能**: 创建一个使用指定初始分组名的 IniParser
- **参数**: 
  - `group_name` - 设置为当前组的初始分组名

### 5.3 析构函数

```cpp
~IniParser();
```
- **功能**: 清理资源
- **说明**: 默认析构函数，无需手动清理

---

## 6. 成员函数

### 6.1 解析与序列化

#### parse（无参数）

```cpp
IniParserError parse();
```
- **功能**: 解析内部上下文字符串
- **返回值**: `IniParserError` 枚举，指示成功或错误类型
- **说明**: 使用之前设置的内部 `_context` 字符串

#### parse（带缓冲区）

```cpp
IniParserError parse(const char* context, size_t length);
```
- **功能**: 从字符缓冲区解析 INI 内容
- **参数**:
  - `context` - 指向 INI 格式字符串的指针
  - `length` - 字符串的字节长度
- **返回值**: `IniParserError` 枚举，指示成功或错误类型

#### parse（带字符串）

```cpp
IniParserError parse(const std::string& context);
```
- **功能**: 从字符串解析 INI 内容
- **参数**:
  - `context` - INI 格式字符串
- **返回值**: `IniParserError` 枚举，指示成功或错误类型

#### dump

```cpp
std::string dump(bool include_empty_group = true);
```
- **功能**: 将内部数据结构序列化为 INI 格式字符串
- **参数**:
  - `include_empty_group` - 是否在输出中包含空分组（默认：true）
- **返回值**: 数据的 INI 格式字符串表示

### 6.2 分组管理

#### setGroup

```cpp
void setGroup(const std::string& group);
```
- **功能**: 设置当前工作分组
- **参数**:
  - `group` - 设置为当前的分组名
- **返回值**: 无
- **说明**: 后续操作将影响此分组

#### currentGroupName

```cpp
const std::string& currentGroupName() const;
```
- **功能**: 获取当前工作分组的名称
- **返回值**: 当前分组名称字符串的常量引用

#### removeGroup

```cpp
void removeGroup(const std::string& group = {});
```
- **功能**: 移除分组及其所有键值对
- **参数**:
  - `group` - 要移除的分组名（默认：空字符串，移除当前分组）
- **返回值**: 无

### 6.3 键值操作

#### setValue

```cpp
void setValue(const std::string &key, std::string &value);
```
- **功能**: 在当前分组中设置键值对
- **参数**:
  - `key` - 键名
  - `value` - 要设置的值（通过引用传递，可能被修改）
- **返回值**: 无
- **说明**: 如果键存在，则更新值；否则创建新的键值对

#### unsetValue

```cpp
void unsetValue(const std::string& key);
```
- **功能**: 从当前分组中移除键值对
- **参数**:
  - `key` - 要移除的键名
- **返回值**: 无

#### value

```cpp
std::string value(const std::string& key, bool parse_escaped_char = true, bool *ok = nullptr);
```
- **功能**: 获取与键关联的值
- **参数**:
  - `key` - 要查找的键名
  - `parse_escaped_char` - 是否解析值中的转义序列（默认：true）
  - `ok` - 可选的布尔指针，指示是否找到键（默认：nullptr）
- **返回值**: 值字符串，如果未找到键则返回空字符串
- **说明**: 如果提供了 `ok`，当键存在时设置为 true，否则为 false

#### clearKeys

```cpp
void clearKeys();
```
- **功能**: 清除当前分组中的所有键值对
- **返回值**: 无

#### clearKeys（带分组参数）

```cpp
void clearKeys(const std::string &group);
```
- **功能**: 清除特定分组中的所有键值对
- **参数**:
  - `group` - 要清除键的分组名
- **返回值**: 无

### 6.4 查询函数

#### isKey

```cpp
bool isKey(const std::string& key) const;
```
- **功能**: 检查键是否存在于当前分组中
- **参数**:
  - `key` - 要检查的键名
- **返回值**: 如果键存在返回 `true`，否则返回 `false`

#### keys

```cpp
std::vector<std::string> keys() const;
```
- **功能**: 获取当前分组中的所有键名
- **返回值**: 键名字符串向量

#### groups

```cpp
std::vector<std::string> groups() const;
```
- **功能**: 获取所有分组名
- **返回值**: 分组名字符串向量

#### keysCount

```cpp
size_t keysCount() const;
```
- **功能**: 获取当前分组中的键数量
- **返回值**: 当前分组中键值对的数量

#### groupsCount

```cpp
size_t groupsCount() const;
```
- **功能**: 获取分组总数
- **返回值**: 分组数量

### 6.5 运算符

#### operator[]

```cpp
std::string& operator[](const std::string& key);
```
- **功能**: 按键获取或创建值引用（类似 std::map）
- **参数**:
  - `key` - 键名
- **返回值**: 值字符串的引用
- **说明**: 如果键不存在，在当前分组中创建它并赋空值

---

## 7. 使用示例

### 7.1 基本解析示例

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser;
    
    std::string ini_content = R"(
[database]
host=localhost
port=3306
username=root

[server]
address=0.0.0.0
port=8080
)";
    
    // 解析 INI 内容
    auto err = parser.parse(ini_content);
    if (err != Tiny::IniParserError::Success) {
        std::cerr << "解析错误！" << std::endl;
        return 1;
    }
    
    // 访问值
    parser.setGroup("database");
    std::cout << "数据库主机: " << parser.value("host") << std::endl;
    std::cout << "数据库端口: " << parser.value("port") << std::endl;
    
    parser.setGroup("server");
    std::cout << "服务器地址: " << parser.value("address") << std::endl;
    
    return 0;
}
```

### 7.2 程序化构建配置

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser;
    
    // 设置数据库配置
    parser.setGroup("database");
    std::string host = "localhost";
    std::string port = "3306";
    parser.setValue("host", host);
    parser.setValue("port", port);
    
    // 设置服务器配置
    parser.setGroup("server");
    std::string addr = "0.0.0.0";
    std::string port2 = "8080";
    parser.setValue("address", addr);
    parser.setValue("port", port2);
    
    // 转储为 INI 格式
    std::string output = parser.dump();
    std::cout << output << std::endl;
    
    // 检查统计信息
    std::cout << "总分组数: " << parser.groupsCount() << std::endl;
    std::cout << "当前分组键数: " << parser.keysCount() << std::endl;
    
    return 0;
}
```

### 7.3 使用 operator[] 快速访问

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser("config");
    
    // 使用 operator[] 快速设置
    parser["username"] = "admin";
    parser["password"] = "secret";
    parser["timeout"] = "30";
    
    // 使用 operator[] 快速获取
    std::cout << "用户名: " << parser["username"] << std::endl;
    
    // 检查键是否存在
    if (parser.isKey("username")) {
        std::cout << "用户名已配置" << std::endl;
    }
    
    // 列出所有键
    std::cout << "所有键:" << std::endl;
    for (const auto& key : parser.keys()) {
        std::cout << "  - " << key << std::endl;
    }
    
    return 0;
}
```

### 7.4 错误处理示例

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser;
    
    std::string invalid_ini = R"(
[database]
host=localhost
invalid line without equals
port=3306
)";
    
    auto err = parser.parse(invalid_ini);
    
    switch (err) {
        case Tiny::IniParserError::Success:
            std::cout << "解析成功" << std::endl;
            break;
        case Tiny::IniParserError::InvalidCharacter:
            std::cerr << "错误: INI 内容中存在无效字符" << std::endl;
            break;
        case Tiny::IniParserError::InvalidFormat:
            std::cerr << "错误: INI 格式无效" << std::endl;
            break;
    }
    
    return 0;
}
```

### 7.5 进阶：带状态检查的值检索

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser;
    
    std::string ini = R"(
[settings]
theme=dark
language=en
)";
    
    parser.parse(ini);
    parser.setGroup("settings");
    
    // 使用 ok 参数检查键是否存在
    bool found = false;
    std::string theme = parser.value("theme", true, &found);
    
    if (found) {
        std::cout << "主题: " << theme << std::endl;
    } else {
        std::cout << "主题未设置，使用默认值" << std::endl;
    }
    
    // 尝试获取不存在的键
    std::string missing = parser.value("nonexistent", true, &found);
    if (!found) {
        std::cout << "键 'nonexistent' 不存在" << std::endl;
    }
    
    return 0;
}
```

---

## 8. 注意事项

### 8.1 INI 格式支持

解析器支持标准 INI 格式：

```ini
; 这是注释
# 这也是注释

[分组名]
key1=value1
key2=value2

[另一个分组]
key3=value3
```

### 8.2 默认分组行为

- 在任何分组头之前定义的键被放置在默认的 "ungrouped" 分组中
- 默认分组名可以通过向构造函数传递分组名来覆盖
- 使用 `setGroup("ungrouped")` 访问没有分组头的键

### 8.3 转义字符处理

- `value()` 方法有一个 `parse_escaped_char` 参数（默认：true）
- 启用时，会解析 `\n`、`\t`、`\\` 等转义序列
- 设置为 false 以获取原始值字符串而不进行转义处理

### 8.4 分组与键操作

- 分组在首次引用时自动创建
- 移除分组会删除其所有键值对
- 无参数的 `clearKeys()` 仅清除当前分组
- `clearKeys(group_name)` 清除特定分组

### 8.5 迭代器与容器访问

```cpp
// 获取所有分组
auto all_groups = parser.groups();
for (const auto& group : all_groups) {
    std::cout << "分组: " << group << std::endl;
}

// 获取当前分组中的所有键
auto all_keys = parser.keys();
for (const auto& key : all_keys) {
    std::cout << "键: " << key << " = " << parser.value(key) << std::endl;
}
```

### 8.6 性能考虑

- 分组查找使用哈希表，时间复杂度 O(1)
- 分组内的键查找是线性的 O(n)，其中 n 是分组中键的数量
- 适用于中小型配置文件
- 对于大型配置，考虑将所有数据存储在内存中的内存开销

### 8.7 线程安全

- `IniParser` 类不是线程安全的
- 如果在线程间共享，需要外部同步
- 每个线程应使用自己的实例进行并发操作
