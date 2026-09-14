# Changelog

### Added

- **OS**：新增 `Tiny::OS::exec()`，支持执行系统命令，可设置超时时间（毫秒）并分别捕获标准输出与标准错误。
- **OS**：新增 FreeBSD 系统下获取 CPU、内存等系统信息的支持。
- **TUI**：新增 tty 终端环境下对鼠标点击与释放事件的捕获支持。
- **TUI**：新增 Windows 系统下对 `wcwidth` 函数的支持，用于计算宽字符宽度。
- **Terminal**：新增输出宽字符的能力。
- **DateTime**：新增输出标准时间格式的支持，并完善相关功能与 API。
- **Net**：新增 `ping` 示例程序，用于测试网络连通性。
- 新增 `du` 示例程序，用于查看指定路径实际占用的数据大小。
- 添加项目相关文档（含模块导入指南等）。

### Changed

- 强化 TUI 模块并进一步调整相关 API；完善 TUI 模块整体功能。
- 优化 Renderer 渲染机制，以及 Terminal 连续输出宽字符时的处理逻辑。
- **Net**：新增更多 Socket 选项，并修复部分已有 Socket 选项；完善 Net 相关功能使 `ping` 程序能够正常反馈。
- **DateTime**：格式化字符串功能新增对反转义符的支持，以避免不必要的解析。
- 新增 FreeBSD CI 工作流，并持续修复多平台工作流与包获取问题。

### Fixed

- 修复 `OS::exec()` 在命令已超时的情况下程序无法退出的问题。
- 修复除 Linux 外其它 Unix 类系统（如 FreeBSD、macOS）环境下的编译错误。
- 修复 FreeBSD 系统环境下按下键盘时程序崩溃的问题。
- 修复 Unix 环境下频繁捕获事件时出现段错误的问题。
- 修复 macOS 系统下的编译问题。
- 修复 `ping` 程序的部分已知问题。

---

# Changelog (English)

### Added

- **OS**: Added `Tiny::OS::exec()`, which executes system commands with a configurable timeout (in milliseconds) and can capture stdout and stderr separately.
- **OS**: Added support for retrieving CPU, memory, and other system information on FreeBSD.
- **TUI**: Added support for capturing mouse press and release events in tty terminal environments.
- **TUI**: Added support for the `wcwidth` function on Windows, used to calculate the width of wide characters.
- **Terminal**: Added the ability to output wide characters.
- **DateTime**: Added support for outputting standard time formats and improved related features and APIs.
- **Net**: Added the `ping` sample program for testing network connectivity.
- Added the `du` sample program for viewing the actual data size occupied by a specified path.
- Added project-related documentation (including the module integration guide).

### Changed

- Enhanced the TUI module and further adjusted related APIs; improved the overall functionality of the TUI module.
- Optimized the Renderer rendering mechanism and Terminal's handling of continuous wide-character output.
- **Net**: Added more Socket options and fixed some existing Socket options; improved Net-related functionality so the `ping` program can report results correctly.
- **DateTime**: The format-string feature now supports escape characters to avoid unnecessary parsing.
- Added a FreeBSD CI workflow and continued fixing multi-platform workflow and package retrieval issues.

### Fixed

- Fixed an issue where `OS::exec()` failed to exit after the command had already timed out.
- Fixed compilation errors on Unix-like systems other than Linux (e.g., FreeBSD, macOS).
- Fixed a program crash that occurred when pressing a key on FreeBSD.
- Fixed a segmentation fault caused by frequent event capturing on Unix.
- Fixed compilation issues on macOS.
- Fixed several known issues in the `ping` program.


