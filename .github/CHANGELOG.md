# Changelog

## English

---

### 2026-08

#### Added
- FreeBSD support for CPU and memory information retrieval
- `du` demo program for checking disk usage under a specified path
- FreeBSD CI workflow for automated testing
- Wide character output support in `Terminal` module
- Standard time format output in `DateTime` module
- Escape sequence support in `DateTime` format strings to avoid unnecessary parsing
- Additional `Socket` options in `Net` module
- `ping` demo program for network connectivity testing
- `IniParser` class for INI configuration file parsing
- XMake build system support
- Multiple architecture support in CI workflows
- Custom `Socket` option configuration in `Net` module
- Hostname resolution to IP address conversion in `Net` module
- `std::cout`-like stream-style output in `Terminal` module
- Additional test files to enhance `Net` module stability
- Windows MSVC/MinGW64 compiler CI workflows

#### Fixed
- Segmentation fault when frequently polling events in Unix environment
- Compilation errors in Unix-like systems
- `/` directory recognition issue in Linux/Mac for `File` module
- Known issues in `ping` program
- FreeBSD CI workflow build issues
- Package retrieval failure in FreeBSD workflow
- Unit test file compilation errors
- Compilation issues in Unix environments
- Improper handling of alternative installation paths in workflows

#### Changed
- Enhanced `Renderer` rendering mechanism and `Terminal` continuous wide character output handling
- Strengthened `TUI` module and adjusted related APIs
- Optimized `Net` module functionality for proper `ping` feedback
- Reverted version number to `0.X.X` in source code
- Updated CMake/XMake configuration files
- Optimized `Net` module with registered available `Socket` options
- Adjusted `TUI` module and introduced `Net` module (preliminary)
- Enhanced `Terminal` API with stream-style output
- Improved workflow for multi-platform support

---

### 2026-07

#### Added
- Linux CI workflow
- macOS CI workflow
- `Tree` demo program
- Formatted string pattern support in `TUI` module
- Optimized `Event` module with new features
- New demo programs

#### Fixed
- Workflow-related issues
- Minor issues in `DateTime` class
- Compilation issues in unit test files
- Various known issues in `OS` module

#### Changed
- Updated version to 1.1.1
- Refined `DateTime` module details
- Completed and optimized CI workflows
- Updated all module documentation
- Optimized `TUI` module
- Optimized `OS` module with bug fixes
- Optimized `Events` module and related test units

---

### 2026-06

#### Added
- `EventsMap` for multi-event management
- `Misc` module in `Demo` for development efficiency
- Dialog box demo program
- Editor demo (preliminary)
- Mouse event support in `Terminal` (Windows)
- `Renderer` fill row/column functions
- Auto-redraw on window resize for `Renderer`

#### Fixed
- Unicode display issues in terminal
- Windows early version API compatibility issues
- Mouse button detection logic in Unix-like systems
- `TUI` logic issues and internal function improvements

#### Changed
- Added LICENSE header to test files
- Updated docs and API documentation
- Enhanced `CommandParser` parsing functionality
- Added more special key support for Windows
- Significantly optimized Unix (Linux/Mac) input logic with boundary checks
- Modified API for format string with nested parsing support

---

### 2026-05

#### Added
- `DateTime` module for date/time handling
- Time unit user-defined literals
- Local time to UTC conversion and vice versa
- `File` module for cross-platform file operations (Windows, Linux, macOS, Unix)
- `TUI` module for terminal user interface
- `Events` module for asynchronous event management
- `OS::System` module for system information (CPU, memory, disk)
- `Terminal` module for terminal control (raw mode, cursor, colors)
- `CommandParser` module for command-line argument parsing
- `Path` class for path handling
- CMake build system configuration
- Initial project structure with `File` module (Windows-only initially)

#### Fixed
- Known issues in `DateTime` module
- Unix system compatibility for `DateTime`
- C++11 compilation issues in `File` module
- Logic issues in `File` module
- Unix-like system compilation issues
- Build system configuration

#### Changed
- Completed `DateTime` module implementation
- Updated all project documentation
- Improved `File` module read/write mechanism
- Updated README documentation
- Added `TINY_CPP_MY_OS_WINDOWS` and `TINY_CPP_MY_OS_UNIX` macros for platform detection
- Initial project setup and first commit
- Established `dev` branch for development

---

## 中文

---

### 2026 年 8 月

#### 新增
- 新增 FreeBSD 系统下获取 CPU、内存等信息的支持
- 新增 `du` 示例程序，用于查看指定路径下占用的数据大小
- 新增 FreeBSD CI 工作流进行自动化测试
- Terminal 新增宽字符输出支持
- DateTime 新增输出标准时间格式
- DateTime 格式化字符串新增转义字符支持，避免不必要的解析
- Net 模块新增更多 Socket 选项
- 新增 `ping` 示例程序用于测试网络连通性
- 新增 IniParser 配置文件解析器
- 新增 XMake 构建系统支持
- 新增多平台多架构 CI 工作流支持
- Net 模块新增自定义 Socket 设置功能
- Net 模块增加支持解析主机名并转换为 IP 地址的功能
- Terminal 新增流式输出 API（类似 std::cout 效果）
- 新增测试文件以加强 Net 模块稳定性
- 新增 Windows 系统下 MSVC/MinGW64 编译器的 CI 工作流

#### 修复
- 修复 Unix 环境下频繁获取事件时出现段错误的问题
- 修复 Unix-like 系统下的编译错误
- 修复 Linux/Mac 下不能正确识别 `/` 目录的问题
- 修复 ping 程序部分已知问题
- 修复 FreeBSD 工作流构建问题
- 修复 FreeBSD 工作流无法获取包的问题
- 修复单元测试文件的编译问题
- 修复 Unix 下的编译问题
- 修复安装路径非标准路径处理不当的问题

#### 变更
- 优化 Renderer 渲染机制以及 Terminal 连续输出宽字符处理功能
- 强化 TUI 模块，并进一步调整相关 API
- 完善 Net 相关功能，使 ping 程序能够正常反馈
- 版本号重新修正为 0.X.X
- 修改 CMake/XMake 相关配置
- 完善 Net 模块，注册了部分可用的 Socket 选项
- 新增或调整 TUI 模块，新增 Net 网络模块（暂未完善）
- 强化并新增 Terminal 相关 API，实现类似 std::cout 效果
- 优化 CI 工作流以支持多平台

---

### 2026 年 7 月

#### 新增
- 新增 Linux CI 工作流
- 新增 macOS CI 工作流
- 新增 Tree 示例程序
- 新增格式化字符串模板模式支持
- 优化 Event 模块并新增功能
- 新增示例程序

#### 修复
- 修复工作流相关问题
- 修复 DateTime 类的细微问题
- 修复部分单元测试文件的编译问题
- 修复 OS 模块的已知问题

#### 变更
- 针对 DateTime 类做了细微的修改
- 完善 CI 工作流
- 更新了所有模块的文档
- 优化 TUI 模块
- 优化 OS 模块并修复问题
- 优化 Events 模块及其测试单元

---

### 2026 年 6 月

#### 新增
- 新增 EventsMap 用于多事件管理
- Demo 模块新增 Misc 辅助模块
- 新增对话框示例程序
- 新增编辑器示例（未完善）
- Terminal 新增鼠标操作支持（Windows）
- Renderer 新增填充行和列的功能
- Renderer 支持窗口大小变化时自动重绘

#### 修复
- 修复终端 Unicode 字符显示问题
- 修复 Windows 早期版本 API 兼容性问题
- 修复 Unix-like 系统下鼠标按键检测逻辑问题
- 修复 TUI 逻辑问题及内部函数改进

#### 变更
- 为测试文件添加 LICENSE 头
- 更新文档及 API 文档
- 增强 CommandParser 解析功能
- Windows 添加更多额外特殊键支持
- 大幅优化 Unix 输入逻辑及边界检查
- 修改 API 支持格式化字符串嵌套解析

---

### 2026 年 5 月

#### 新增
- 新增 DateTime 日期时间模块
- 新增时间单位字面量支持
- 支持本地时间与 UTC 时间转换
- 新增 File 模块，支持跨平台文件操作（Windows、Linux、macOS、Unix）
- 新增 TUI 终端界面模块
- 新增 Events 异步事件管理模块
- 新增 OS::System 系统信息模块（CPU、内存、磁盘）
- 新增 Terminal 终端控制模块（原始模式、光标、颜色）
- 新增 CommandParser 命令行参数解析模块
- 新增 Path 路径处理类
- 新增 CMake 构建系统配置
- 初始项目结构，仅支持 Windows 的 File 模块

#### 修复
- 修复 DateTime 模块已知问题
- 修复 Unix 系统下 DateTime 兼容性问题
- 修复 File 模块在 C++11 标准下的编译问题
- 修复 File 模块逻辑问题
- 修复 Unix-like 系统编译问题
- 修复构建系统配置

#### 变更
- 完善 DateTime 模块
- 更新所有项目文档
- 完善 File 模块读写机制
- 更新 README 文档
- 新增宏定义 TINY_CPP_MY_OS_WINDOWS 和 TINY_CPP_MY_OS_UNIX
- 初始项目提交
- 创建 dev 开发分支