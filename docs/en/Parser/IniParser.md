# Parser Module - IniParser Class

Namespace: `Tiny`

---

## Table of Contents

1. [Class Overview](#1-class-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Data Structures](#4-data-structures)
5. [Constructors and Destructor](#5-constructors-and-destructor)
6. [Member Functions](#6-member-functions)
7. [Usage Examples](#7-usage-examples)
8. [Notes](#8-notes)

---

## 1. Class Overview

The `IniParser` class provides INI configuration file parsing and manipulation functionality, supporting group-based key-value pair storage, parsing from strings, and dumping back to INI format.

### Core Features

- **Group-Based Organization**: Supports organizing key-value pairs into named groups (sections)
- **String Parsing**: Can parse INI-formatted strings into structured data
- **Serialization**: Can dump internal data structure back to INI format string
- **Flexible Access**: Provides multiple ways to access and modify configuration data
- **Default Group**: Supports ungrouped keys with a default "ungrouped" section

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/Parser/IniParser.hpp>
// Direct source copy method
#include "Parser/IniParser.hpp"
```

---

## 3. Type Definitions

```cpp
using IniConf = std::pair<std::string, std::string>;
using IniGroup = std::vector<IniConf>;
using IniMap  = std::unordered_map<std::string, IniGroup>;
```

| Type Alias | Underlying Type | Description |
|------------|-----------------|-------------|
| `IniConf` | `std::pair<std::string, std::string>` | A single key-value pair |
| `IniGroup` | `std::vector<IniConf>` | A collection of key-value pairs in a group |
| `IniMap` | `std::unordered_map<std::string, IniGroup>` | Map of group names to their key-value pairs |

---

## 4. Data Structures

### 4.1 IniParserError Enum

```cpp
enum class IniParserError : uint8_t {
    Success,           // Operation successful
    InvalidCharacter,  // Invalid character encountered
    InvalidFormat      // Invalid format
};
```

| Enum Value | Value | Description |
|------------|-------|-------------|
| `Success` | 0 | Parse or operation completed successfully |
| `InvalidCharacter` | 1 | Encountered invalid character in INI content |
| `InvalidFormat` | 2 | INI format is invalid or malformed |

---

## 5. Constructors and Destructor

### 5.1 Default Constructor

```cpp
IniParser();
```
- **Function**: Create an IniParser with default group name "ungrouped"
- **Parameters**: None

### 5.2 Constructor with Group Name

```cpp
IniParser(const std::string& group_name);
```
- **Function**: Create an IniParser with specified initial group name
- **Parameter**: 
  - `group_name` - Initial group name to set as current group

### 5.3 Destructor

```cpp
~IniParser();
```
- **Function**: Clean up resources
- **Note**: Default destructor, no manual cleanup needed

---

## 6. Member Functions

### 6.1 Parsing and Serialization

#### parse (no parameters)

```cpp
IniParserError parse();
```
- **Function**: Parse the internal context string
- **Return Value**: `IniParserError` enum indicating success or error type
- **Note**: Uses the internal `_context` string set previously

#### parse (with buffer)

```cpp
IniParserError parse(const char* context, size_t length);
```
- **Function**: Parse INI content from a character buffer
- **Parameters**:
  - `context` - Pointer to INI-formatted string
  - `length` - Length of the string in bytes
- **Return Value**: `IniParserError` enum indicating success or error type

#### parse (with string)

```cpp
IniParserError parse(const std::string& context);
```
- **Function**: Parse INI content from a string
- **Parameter**:
  - `context` - INI-formatted string
- **Return Value**: `IniParserError` enum indicating success or error type

#### dump

```cpp
std::string dump(bool include_empty_group = true);
```
- **Function**: Serialize internal data structure to INI format string
- **Parameter**:
  - `include_empty_group` - Whether to include empty groups in output (default: true)
- **Return Value**: INI-formatted string representation of the data

### 6.2 Group Management

#### setGroup

```cpp
void setGroup(const std::string& group);
```
- **Function**: Set the current working group
- **Parameter**:
  - `group` - Group name to set as current
- **Return Value**: None
- **Note**: Subsequent operations will affect this group

#### currentGroupName

```cpp
const std::string& currentGroupName() const;
```
- **Function**: Get the name of the current working group
- **Return Value**: Constant reference to current group name string

#### removeGroup

```cpp
void removeGroup(const std::string& group = {});
```
- **Function**: Remove a group and all its key-value pairs
- **Parameter**:
  - `group` - Group name to remove (default: empty string, removes current group)
- **Return Value**: None

### 6.3 Key-Value Operations

#### setValue

```cpp
void setValue(const std::string &key, std::string &value);
```
- **Function**: Set a key-value pair in the current group
- **Parameters**:
  - `key` - Key name
  - `value` - Value to set (passed by reference, may be modified)
- **Return Value**: None
- **Note**: If key exists, updates the value; otherwise creates new pair

#### unsetValue

```cpp
void unsetValue(const std::string& key);
```
- **Function**: Remove a key-value pair from the current group
- **Parameter**:
  - `key` - Key name to remove
- **Return Value**: None

#### value

```cpp
std::string value(const std::string& key, bool parse_escaped_char = true, bool *ok = nullptr);
```
- **Function**: Get the value associated with a key
- **Parameters**:
  - `key` - Key name to look up
  - `parse_escaped_char` - Whether to parse escape sequences in value (default: true)
  - `ok` - Optional pointer to bool indicating if key was found (default: nullptr)
- **Return Value**: Value string, or empty string if key not found
- **Note**: If `ok` is provided, it will be set to true if key exists, false otherwise

#### clearKeys

```cpp
void clearKeys();
```
- **Function**: Clear all key-value pairs in the current group
- **Return Value**: None

#### clearKeys (with group)

```cpp
void clearKeys(const std::string &group);
```
- **Function**: Clear all key-value pairs in a specific group
- **Parameter**:
  - `group` - Group name whose keys should be cleared
- **Return Value**: None

### 6.4 Query Functions

#### isKey

```cpp
bool isKey(const std::string& key) const;
```
- **Function**: Check if a key exists in the current group
- **Parameter**:
  - `key` - Key name to check
- **Return Value**: `true` if key exists, `false` otherwise

#### keys

```cpp
std::vector<std::string> keys() const;
```
- **Function**: Get all key names in the current group
- **Return Value**: Vector of key name strings

#### groups

```cpp
std::vector<std::string> groups() const;
```
- **Function**: Get all group names
- **Return Value**: Vector of group name strings

#### keysCount

```cpp
size_t keysCount() const;
```
- **Function**: Get the number of keys in the current group
- **Return Value**: Number of key-value pairs in current group

#### groupsCount

```cpp
size_t groupsCount() const;
```
- **Function**: Get the total number of groups
- **Return Value**: Number of groups

### 6.5 Operators

#### operator[]

```cpp
std::string& operator[](const std::string& key);
```
- **Function**: Get or create a value reference by key (similar to std::map)
- **Parameter**:
  - `key` - Key name
- **Return Value**: Reference to the value string
- **Note**: If key doesn't exist, creates it with empty value in current group

---

## 7. Usage Examples

### 7.1 Basic Parsing Example

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
    
    // Parse INI content
    auto err = parser.parse(ini_content);
    if (err != Tiny::IniParserError::Success) {
        std::cerr << "Parse error!" << std::endl;
        return 1;
    }
    
    // Access values
    parser.setGroup("database");
    std::cout << "Database host: " << parser.value("host") << std::endl;
    std::cout << "Database port: " << parser.value("port") << std::endl;
    
    parser.setGroup("server");
    std::cout << "Server address: " << parser.value("address") << std::endl;
    
    return 0;
}
```

### 7.2 Building Configuration Programmatically

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser;
    
    // Set database configuration
    parser.setGroup("database");
    std::string host = "localhost";
    std::string port = "3306";
    parser.setValue("host", host);
    parser.setValue("port", port);
    
    // Set server configuration
    parser.setGroup("server");
    std::string addr = "0.0.0.0";
    std::string port2 = "8080";
    parser.setValue("address", addr);
    parser.setValue("port", port2);
    
    // Dump to INI format
    std::string output = parser.dump();
    std::cout << output << std::endl;
    
    // Check statistics
    std::cout << "Total groups: " << parser.groupsCount() << std::endl;
    std::cout << "Keys in current group: " << parser.keysCount() << std::endl;
    
    return 0;
}
```

### 7.3 Using operator[] for Quick Access

```cpp
#include "Parser/IniParser.hpp"
#include <iostream>

int main() {
    Tiny::IniParser parser("config");
    
    // Quick set using operator[]
    parser["username"] = "admin";
    parser["password"] = "secret";
    parser["timeout"] = "30";
    
    // Quick get using operator[]
    std::cout << "Username: " << parser["username"] << std::endl;
    
    // Check if key exists
    if (parser.isKey("username")) {
        std::cout << "Username is configured" << std::endl;
    }
    
    // List all keys
    std::cout << "All keys:" << std::endl;
    for (const auto& key : parser.keys()) {
        std::cout << "  - " << key << std::endl;
    }
    
    return 0;
}
```

### 7.4 Error Handling Example

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
            std::cout << "Parse successful" << std::endl;
            break;
        case Tiny::IniParserError::InvalidCharacter:
            std::cerr << "Error: Invalid character in INI content" << std::endl;
            break;
        case Tiny::IniParserError::InvalidFormat:
            std::cerr << "Error: Invalid INI format" << std::endl;
            break;
    }
    
    return 0;
}
```

### 7.5 Advanced: Value Retrieval with Status Check

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
    
    // Check if key exists with ok parameter
    bool found = false;
    std::string theme = parser.value("theme", true, &found);
    
    if (found) {
        std::cout << "Theme: " << theme << std::endl;
    } else {
        std::cout << "Theme not set, using default" << std::endl;
    }
    
    // Try to get non-existent key
    std::string missing = parser.value("nonexistent", true, &found);
    if (!found) {
        std::cout << "Key 'nonexistent' does not exist" << std::endl;
    }
    
    return 0;
}
```

---

## 8. Notes

### 8.1 INI Format Support

The parser supports standard INI format:

```ini
; This is a comment
# This is also a comment

[group_name]
key1=value1
key2=value2

[another_group]
key3=value3
```

### 8.2 Default Group Behavior

- Keys defined before any group header are placed in the default "ungrouped" group
- The default group name can be overridden by passing a group name to the constructor
- Use `setGroup("ungrouped")` to access keys without a group header

### 8.3 Escape Character Handling

- The `value()` method has a `parse_escaped_char` parameter (default: true)
- When enabled, escape sequences like `\n`, `\t`, `\\` are parsed
- Set to false to retrieve raw value strings without escape processing

### 8.4 Group and Key Operations

- Groups are created automatically when first referenced
- Removing a group removes all its key-value pairs
- `clearKeys()` without parameters clears the current group only
- `clearKeys(group_name)` clears a specific group

### 8.5 Iterator and Container Access

```cpp
// Get all groups
auto all_groups = parser.groups();
for (const auto& group : all_groups) {
    std::cout << "Group: " << group << std::endl;
}

// Get all keys in current group
auto all_keys = parser.keys();
for (const auto& key : all_keys) {
    std::cout << "Key: " << key << " = " << parser.value(key) << std::endl;
}
```

### 8.6 Performance Considerations

- Group lookup uses hash table, time complexity O(1)
- Key lookup within a group is linear O(n) where n is the number of keys in the group
- Suitable for small to medium configuration files
- For large configurations, consider the memory overhead of storing all data in memory

### 8.7 Thread Safety

- The `IniParser` class is not thread-safe
- External synchronization required if shared across threads
- Each thread should use its own instance for concurrent operations
