# Changelog

---

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
