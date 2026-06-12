# Parser Module - CommandParser Class

Namespace: `Tiny`

---

## Table of Contents

1. [Class Overview](#1-class-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Data Structures](#4-data-structures)
5. [Constructors and Destructor](#5-constructors-and-destructor)
6. [Member Functions](#6-member-functions)
7. [Static Member Functions](#7-static-member-functions)
8. [Usage Examples](#8-usage-examples)
9. [Notes](#9-notes)

---

## 1. Class Overview

The `CommandParser` class provides command-line argument parsing functionality, supporting GNU-style long/short options, value-bearing parameters, default value settings, and more.

### Core Features

- **Long/Short Options**: Supports both `--long` and `-s` formats
- **Value-bearing Parameters**: Supports `--option value` and `--option=value` formats
- **Default Values**: Can set default values for options
- **Long-only Options**: Supports long-format only options
- **Default Commands**: Supports default commands without option prefix

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/Parser/CommandParser.hpp>
// Direct source copy method
#include "Parser/CommandParser.hpp"
```

---

## 3. Type Definitions

```cpp
using iter = std::unordered_map<std::string, Command>::iterator;
using constIter = std::unordered_map<std::string, Command>::const_iterator;
```

---

## 4. Data Structures

### 4.1 Command Structure

```cpp
struct Command {
    std::string option_name;       // Full option name
    std::string short_options;     // Short option characters (e.g., "h" for -h)
    std::string description;       // Option description
    bool full_option_only;         // Whether only long option is supported
    bool is_default_command;       // Whether it is a default command
    bool is_need;                  // Whether it is a required command
    bool has_value;                // Whether it accepts a value
    std::string value;             // Actual passed value
    std::string default_value;     // Default value
};
```

| Field | Type | Description |
|-------|------|-------------|
| `option_name` | `std::string` | Long option name (e.g., "help") |
| `short_options` | `std::string` | Short option character set |
| `description` | `std::string` | Option description text |
| `full_option_only` | `bool` | true means short option not supported |
| `is_default_command` | `bool` | Whether it is a default execution command |
| `is_need` | `bool` | Whether this option is required |
| `has_value` | `bool` | Whether this option requires a parameter value |
| `value` | `std::string` | Actual value obtained after parsing |
| `default_value` | `std::string` | Default value when not provided |

### 4.2 ParseError Enum

```cpp
enum class ParseError : uint8_t {
    NoError,              // No error
    UnknownOption,        // Unknown option
    FullOptionOnly,       // Long option only but short option was used
    InvalidValue,         // Invalid value
    MissingArgument,      // Missing argument
    FormatError,          // Format error
    MissingDefaultCommand // Missing default command
};
```

| Enum Value | Value | Description |
|------------|-------|-------------|
| `NoError` | 0 | Parse successful |
| `UnknownOption` | 1 | Encountered undefined option |
| `FullOptionOnly` | 2 | Short format used for long-only option |
| `InvalidValue` | 3 | Parameter value invalid or empty |
| `MissingArgument` | 4 | Required parameter value missing |
| `FormatError` | 5 | Parameter format error |
| `MissingDefaultCommand` | 6 | Missing default command |

---

## 5. Constructors and Destructor

### 5.1 Constructor

```cpp
CommandParser(int argc, char** argv);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `argc` | `int` | Argument count |
| `argv` | `char**` | Argument array |

### 5.2 Destructor

```cpp
~CommandParser() = default;
```

---

## 6. Member Functions

### 6.1 Adding Commands

#### addCommand

```cpp
bool addCommand(const std::string& command_name, 
                const std::string& short_options, 
                const std::string& description = {}, 
                bool has_value = false, 
                const std::string& default_value = {}, 
                bool is_need = false,
                bool default_command = false);
```
- **Function**: Add command option
- **Parameter**: 
  - `command_name` - Long option name (e.g., "help")
  - `short_options` - Short option characters (e.g., "h")
  - `description` - Description text
  - `has_value` - Whether it accepts a value
  - `default_value` - Default value
  - `is_need` - Whether this option is required
  - `default_command` - Whether it is a default command
- **Return Value**: `true` means success (name not duplicate)

#### addFullCommand

```cpp
bool addFullCommand(const std::string& command_name, 
                    const std::string& description, 
                    bool has_value = false, 
                    const std::string& default_value = {}, 
                    bool is_need = false,
                    bool default_command = false);
```
- **Function**: Add long-format only command
- **Parameter**: Same as `addCommand` (without short_options)
- **Return Value**: `true` means success

### 6.2 Removing Commands

#### remove

```cpp
bool remove(const std::string& command_name);
```
- **Function**: Remove added command
- **Parameter**: `command_name` - Command name
- **Return Value**: `true` means successfully removed

#### clear

```cpp
void clear();
```
- **Function**: Clear all command definitions
- **Return Value**: None

### 6.3 Execute Parsing

#### exec

```cpp
ParseError exec(int* parsed_command_count = nullptr,
                int* err_arg_n = nullptr,
                std::vector<std::string>* missing_command_list = nullptr);
```
- **Function**: Execute parameter parsing
- **Parameter**: 
  - `parsed_command_count` - Output number of successfully parsed commands
  - `err_arg_n` - Output error argument position
  - `missing_command_list` - Output list of missing required commands
- **Return Value**: `ParseError` enum value

#### execCommandList

```cpp
const std::vector<Command>& execCommandList() const;
```
- **Function**: Get parsed command list
- **Return Value**: Constant reference to command array

### 6.4 Query Functions

#### size

```cpp
size_t size() const;
```
- **Function**: Get number of added commands
- **Return Value**: Command count

#### exist

```cpp
bool exist(const std::string& command_name) const;
```
- **Function**: Check if command exists
- **Parameter**: `command_name` - Command name
- **Return Value**: `true` means exists

#### at

```cpp
const Command& at(const std::string& command_name) const;
```
- **Function**: Get constant reference to command
- **Parameter**: `command_name` - Command name
- **Return Value**: Constant reference to command
- **Exception**: Throws exception if command does not exist

#### get

```cpp
Command& get(const std::string& command_name);
```
- **Function**: Get reference to command
- **Parameter**: `command_name` - Command name
- **Return Value**: Reference to command

#### operator[]

```cpp
Command& operator[](const std::string& command_name);
```
- **Function**: Get reference to command (same as get)
- **Parameter**: `command_name` - Command name
- **Return Value**: Reference to command

### 6.5 Iterators

#### begin / end

```cpp
iter begin();
iter end();
```
- **Function**: Get iterators (support range-based for loop)

#### cbegin / cend

```cpp
constIter cbegin() const;
constIter cend() const;
```
- **Function**: Get constant iterators

---

## 7. Static Member Functions

### 7.1 getParseErrorName

```cpp
static const char* getParseErrorName(ParseError error);
```
- **Function**: Get error code description string
- **Parameter**: `error` - Error enum value
- **Return Value**: Error description string

---

## 8. Usage Examples

### 8.1 Basic Usage Example

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // Add options
    parser.addCommand("help", "h", "Show help information");
    parser.addCommand("version", "v", "Show version");
    parser.addCommand("output", "o", "Output file", true, "output.txt");
    parser.addFullCommand("verbose", "Verbose output", false, "", true);
    
    // Parse arguments
    int parsed_count = 0;
    int err_pos = 0;
    auto err = parser.exec(&parsed_count, &err_pos);
    
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "Parse error: " << Tiny::CommandParser::getParseErrorName(err) 
                  << " at position " << err_pos << std::endl;
        return 1;
    }
    
    // Check options
    if (parser.exist("help")) {
        std::cout << "Usage: myapp [options]" << std::endl;
        return 0;
    }
    
    if (parser.exist("version")) {
        std::cout << "Version 1.0.0" << std::endl;
        return 0;
    }
    
    // Get value-bearing option
    if (parser.exist("output")) {
        const auto& cmd = parser["output"];
        std::cout << "Output file: " << cmd.value << std::endl;
    }
    
    // Iterate parse results
    for (const auto& cmd : parser.execCommandList()) {
        std::cout << "Command: " << cmd.option_name << std::endl;
    }
    
    return 0;
}
```

### 8.2 Complex Command Line Tool Example

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // Define all options
    parser.addCommand("help", "h", "Show help information");
    parser.addCommand("input", "i", "Input file", true);
    parser.addCommand("output", "o", "Output file", true, "output.txt");
    parser.addCommand("format", "f", "Output format", true, "txt");
    parser.addCommand("verbose", "v", "Verbose output");
    parser.addFullCommand("force", "Force overwrite");
    
    // Parse
    auto err = parser.exec();
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "Error: " << Tiny::CommandParser::getParseErrorName(err) << std::endl;
        return 1;
    }
    
    // Show help
    if (parser.exist("help")) {
        std::cout << "File Converter Tool\n\n";
        std::cout << "Usage: converter [options]\n\n";
        std::cout << "Options:\n";
        for (const auto& [name, cmd] : parser) {
            std::cout << "  --" << cmd.option_name;
            if (!cmd.short_options.empty()) {
                std::cout << ", -" << cmd.short_options;
            }
            std::cout << "\t" << cmd.description;
            if (!cmd.default_value.empty()) {
                std::cout << " (default: " << cmd.default_value << ")";
            }
            std::cout << "\n";
        }
        return 0;
    }
    
    // Check required arguments
    if (!parser.exist("input")) {
        std::cerr << "Error: Input file must be specified (-i)" << std::endl;
        return 1;
    }
    
    // Get parameter values
    std::string input_file = parser["input"].value;
    std::string output_file = parser["output"].value;
    std::string format = parser["format"].value;
    bool verbose = parser.exist("verbose");
    bool force = parser.exist("force");
    
    if (verbose) {
        std::cout << "Input: " << input_file << std::endl;
        std::cout << "Output: " << output_file << std::endl;
        std::cout << "Format: " << format << std::endl;
    }
    
    // Execute conversion...
    
    return 0;
}
```

### 8.3 Subcommand Style Example

```cpp
#include "Parser/CommandParser.hpp"
#include <iostream>

int main(int argc, char** argv) {
    Tiny::CommandParser parser(argc, argv);
    
    // Use default commands to implement subcommand style
    parser.addFullCommand("init", "Initialize project", false, "", true);
    parser.addFullCommand("build", "Build project", false, "", true);
    parser.addFullCommand("clean", "Clean project", false, "", true);
    parser.addCommand("config", "c", "Config file", true, ".projectrc");
    
    auto err = parser.exec();
    if (err != Tiny::CommandParser::ParseError::NoError) {
        std::cerr << "Error: " << Tiny::CommandParser::getParseErrorName(err) << std::endl;
        return 1;
    }
    
    // Check subcommands
    if (parser.exist("init")) {
        std::cout << "Initializing project..." << std::endl;
    } else if (parser.exist("build")) {
        std::cout << "Building project..." << std::endl;
    } else if (parser.exist("clean")) {
        std::cout << "Cleaning project..." << std::endl;
    } else {
        std::cout << "Please specify subcommand: init, build, clean" << std::endl;
    }
    
    return 0;
}
```

---

## 9. Notes

### 9.1 Supported Command Formats

```bash
# Short options
myapp -h
myapp -v
myapp -o file.txt

# Long options
myapp --help
myapp --version
myapp --output file.txt
myapp --output=file.txt

# Combined short options
myapp -hv
myapp -vo file.txt

# Long-only options (added with addFullCommand)
myapp verbose
```

### 9.2 Error Handling

| Error Type | Trigger Condition | Solution |
|------------|-------------------|----------|
| `UnknownOption` | Encountered undefined option | Check command definitions, confirm option name is correct |
| `FullOptionOnly` | Short format used for long-only option | Use `--option` instead of `-o` |
| `InvalidValue` | Parameter value invalid or empty | Check parameter value format, provide valid value |
| `MissingArgument` | Required parameter value missing | Provide parameter for options requiring values |
| `FormatError` | Parameter format error | Check parameter format, ensure it starts with `-` |
| `MissingDefaultCommand` | Missing default command | Ensure a default command is provided when required |

### 9.3 Default Value Handling

- If option has `has_value = true` but no value provided, use `default_value`
- If option has no default value and no value provided, trigger `InvalidValue` error
- Default value is filled into `Command::value` during parsing

### 9.4 Command Name Conflicts

- Long option names must be unique
- Short option characters can be duplicated (multiple long options share one short option)
- Both `addCommand` and `addFullCommand` check for name conflicts

### 9.5 Iterator Usage

```cpp
// Range-based for loop
for (const auto& [name, cmd] : parser) {
    std::cout << name << ": " << cmd.description << std::endl;
}

// Traditional iterator
for (auto it = parser.cbegin(); it != parser.cend(); ++it) {
    std::cout << it->first << ": " << it->second.description << std::endl;
}
```

### 9.6 Performance Considerations

- Command lookup uses hash table, time complexity O(1)
- Parsing process linearly scans parameter list
- Suitable for small to medium command line tools (< 100 options)
