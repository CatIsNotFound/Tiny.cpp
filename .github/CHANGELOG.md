# Changelog

全部版本变更日志，格式遵循 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

---

## [0.3.0] - Unreleased

### Added

- **TUI**：新增 `Object` 基类，为所有 TUI 控件和布局提供统一的对象层级关系（父子管理、查找、类型标识）。
- **TUI**：新增 `Button` 按钮控件（继承自 `Label`），支持点击事件和默认键位绑定。
- **TUI**：新增 `LineEdit` 文本输入控件，支持 `EchoMode`（正常/密码/不回显）、占位符文本、最小/最大长度限制、内容对齐。
- **TUI**：新增 `Slider` 滑动条控件，支持水平/垂直方向、值域范围、单步/页步、反转模式、值变更事件。
- **TUI**：新增 `ProgressBar` 进度条控件（非交互式），支持方向、填充颜色、反转模式。
- **TUI**：新增 `ListView` 列表视图控件，支持动态插入/删除/交换条目、当前索引追踪、选中/激活颜色、索引变更事件。
- **TUI**：新增 `CurBlock` 光标块控件。
- **TUI**：新增 `U8Code` 命名空间（`string2Wide`、`wide2String`、`splitUTF8`、`calcStrDisplayWidth`、`subUTF8` 等），提供跨平台 UTF-8 宽字符工具函数；Windows 版本额外支持 Codepage 参数。
- **TUI**：新增 `StyleStatus` 枚举（`S_Disabled`、`S_Active`、`S_Checked`、`S_Normal`）与 `AbstractWidget::setStyle(uint8_t, Style)` 状态式样式设置接口。
- **TUI**：新增 `Orientation` 枚举（`Horizontal` / `Vertical`）。
- **TUI**：新增 `EchoMode` 枚举（`NoEcho`、`Normal`、`Password`）。
- **TUI**：新增事件总线交互宏 `REG_META_EVENT`、`CALL_META_EVENT`、`LISTEN_EVENT`，用于简化控件与 EventBus 之间的事件注册与调用。
- **TUI**：`Application` 新增 `exit()`、`setEnabledExitByKey` / `isEnabledExitByKey`、`setRefreshEnabled` / `isRefreshEnabled`、`setZOrder`（三个重载）、`zOrder()`、`zOrderOf()`、`count()` 等方法。
- **TUI**：`AbstractWidget` 新增 `setMinMaxSize`、`setMouseTracingEnabled` / `mouseTracingEnabled`、`checkable` / `checked` / `setCheckable` / `setChecked`、`setObjectName` / `objectName` 方法。
- **TUI**：`Label` 新增 `setAutoSizeEnabled` / `autoSizeEnabled`、`setAlignment` / `alignment`、`text()` 方法，支持显式指定 `Size` 或 `Object* parent` 的构造函数重载。
- **TUI**：`AbstractLayout` 新增 `replaceWidget`（按索引和迭代器两个重载）方法。
- **TUI**：`Terminal` 新增输出宽字符的 `printW`、`printLineW`、`printErrorW` 及流式 `print()` / `perror()` 函数；新增 `moveUp/Down/Left/RightCursor` 系列方法。
- **OS/File**：`File::readLine()` 新增 `size_t limit_length = 0` 参数，支持限制单行最大读取字节数。
- **Parser**：`CommandParser` 新增 `generateHelpInfo()` 方法，可根据已添加的命令自动生成格式化帮助文本，支持描述宽度、排序与精简模式。
- 新增示例程序 `read`，可将指定文件内容输出到终端。

### Changed

> ⚠️ 以下条目包含 **破坏性变更 (Breaking Changes)**，升级时请仔细对照。

- **[Breaking] TUI**：`Application` 构造函数从 `Application(int argc, char* argv[])` 变更为 `explicit Application()`，参数处理改由调用者自行负责。
- **[Breaking] TUI**：`AbstractWidget` 构造函数签名变更，新增必需的 `std::type_index type_id` 参数以及可选的 `Object* parent` 参数；`Position`/`Size` 构造形式新增第二重载。
- **[Breaking] TUI**：`AbstractLayout` 构造函数从 `AbstractLayout(const std::string& name)` 变更为 `AbstractLayout(const std::string& name, std::type_index type_id, Object* parent = nullptr)`。
- **[Breaking] TUI**：`AbstractWidget::rename()` 重命名为 `setObjectName()`（同时保留 `renameObject()` 兼容别名）；原 `name()` 方法统一为 `objectName()`。
- **[Breaking] TUI**：`Style` 与 `Corner` 从 `Renderer` 命名空间上移至 `TUI` 命名空间；`Renderer::Style` 和 `Renderer::Corner` 保留为已弃用的 typedef 别名，将在后续版本中移除。
- **[Breaking] TUI**：`AbstractWidget::draw()` 标记为弃用，将在后续版本中移除。
- **[Breaking] TUI**：输入事件类型别名从 `KEY_BACKSPACE` / `KEY_ENTER` 等常量化 API 变更为 `KEY()` / `SP_KEY()` 工厂函数。
- **[Breaking] TUI**：`SizePolicy` 枚举成员顺序调整（`Ignored` 提前）。
- **TUI**：`Renderer::setSSFX` 新增格式化变参版本。
- **TUI**：`Position` / `Size` 结构体新增比较、交换、算术运算符及构造函数重载。
- **Terminal**：宽字符连续输出的渲染逻辑大幅优化。
- **DateTime**：时间戳格式化功能整体完善。
- **文档**：全项目文档（中英文）同步重写，覆盖新 TUI 控件、新增 API 与弃用说明。

### Deprecated

- **TUI**：`Tiny::Code` 作为 `Tiny::U8Code` 的别名，属于历史遗留，保留兼容；建议使用 `Tiny::U8Code`。
- **TUI**：`Renderer::Style` / `Renderer::Corner` 作为 `TUI::Style` / `TUI::Corner` 的 typedef 别名，属于历史遗留；建议直接使用 `TUI::Style` / `TUI::Corner`。
- **TUI**：`AbstractWidget::draw()` 被标记为弃用，建议改用新的渲染驱动方式。

### Fixed

- **TUI/LineEdit**：修复输入时的多项已知问题。
- **TUI/Button**：完善基本功能，修复部分已知缺陷。
- **TUI/Slider**：修正滑动块的方向和范围相关问题。
- **MSVC**：修复 MSVC 编译器下的多项编译错误。
- **UTF-8**：修复并优化 UTF-8 字符串分割与显示宽度计算逻辑。
- **API**：修复部分 API 签名与文档不一致的问题。
- **Terminal/FreeBSD**：修复 FreeBSD 系统环境下光标位置获取错误。
- **Net/文档**：修正文档中错误描述的 `Tiny::Net::parseFromHostname()` / `parseFirstHostname()` 自由函数（实际从未存在），正确入口为 `Address::parseFromHostname()` / `Address::parseFirstHostname()` 静态方法。

---

## [0.2.0]

### Added

- **OS**：新增 `Tiny::OS::exec()`，支持执行系统命令，可设置超时时间（毫秒）并分别捕获标准输出与标准错误。
- **OS**：新增 FreeBSD 系统下获取 CPU、内存、磁盘等系统信息的支持。
- **TUI**：新增 tty 终端环境下对鼠标点击与释放事件的捕获支持。
- **TUI**：新增 Windows 系统下对 `wcwidth` 函数的支持，用于计算宽字符宽度。
- **Terminal**：新增输出宽字符的能力。
- **DateTime**：新增输出标准时间格式的支持，完善格式化字符串与相关 API。
- **Net**：新增 `ping` 示例程序，用于测试网络连通性。
- 新增 `du` 示例程序，用于查看指定路径实际占用的数据大小。
- 添加项目相关文档（含模块导入指南等）。
- 新增 FreeBSD CI 工作流。

### Changed

- 强化 TUI 模块并进一步调整相关 API；完善 TUI 模块整体功能。
- 优化 Renderer 渲染机制以及 Terminal 连续输出宽字符的处理逻辑。
- **Net**：新增更多 Socket 选项，并修复部分已有 Socket 选项；完善 Net 相关功能使 `ping` 程序能够正常反馈。
- **DateTime**：格式化字符串功能新增对反转义符的支持，以避免不必要的解析。
- 持续修复多平台工作流与包获取问题。

### Fixed

- 修复 `OS::exec()` 在命令已超时的情况下程序无法退出的问题。
- 修复除 Linux 外其它 Unix 类系统（如 FreeBSD、macOS）环境下的编译错误。
- 修复 FreeBSD 系统环境下按下键盘时程序崩溃的问题。
- 修复 Unix 环境下频繁捕获事件时出现段错误的问题。
- 修复 macOS 系统下的编译问题。
- 修复 `ping` 程序的部分已知问题。
- 修复了读取文件时的逻辑，即仅文件内容中遇到 `'\n'` 字符时以进行换行。

---

# Changelog (English)

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/).

---

## [0.3.0] - Unreleased

### Added

- **TUI**: Added `Object` base class, providing a unified object hierarchy (parent-child management, lookup, type identity) for all TUI widgets and layouts.
- **TUI**: Added `Button` widget (inherits from `Label`) with click events and default key-bindings.
- **TUI**: Added `LineEdit` text-input widget with `EchoMode` (Normal / Password / NoEcho), placeholder text, min/max length limits, and text alignment.
- **TUI**: Added `Slider` widget supporting horizontal/vertical orientation, value range, single/page step, inverted mode, and value-change events.
- **TUI**: Added `ProgressBar` widget (non-interactive) supporting orientation, fill color, and inverted mode.
- **TUI**: Added `ListView` widget with dynamic item insertion/removal/swapping, current-index tracking, selection/activation colors, and index-change events.
- **TUI**: Added `CurBlock` cursor-block widget.
- **TUI**: Added `Tiny::U8Code` namespace (`string2Wide`, `wide2String`, `splitUTF8`, `calcStrDisplayWidth`, `subUTF8`, etc.) with cross-platform UTF-8 / wide-character utilities; Windows builds additionally accept a codepage parameter.
- **TUI**: Added `StyleStatus` enum (`S_Disabled`, `S_Active`, `S_Checked`, `S_Normal`) and `AbstractWidget::setStyle(uint8_t, Style)` state-based style API.
- **TUI**: Added `Orientation` enum (`Horizontal` / `Vertical`).
- **TUI**: Added `EchoMode` enum (`NoEcho`, `Normal`, `Password`).
- **TUI**: Added `REG_META_EVENT`, `CALL_META_EVENT`, `LISTEN_EVENT` macros to simplify event registration and dispatch between widgets and EventBus.
- **TUI**: `Application` now provides `exit()`, `setEnabledExitByKey` / `isEnabledExitByKey`, `setRefreshEnabled` / `isRefreshEnabled`, `setZOrder` (three overloads), `zOrder()`, `zOrderOf()`, and `count()`.
- **TUI**: `AbstractWidget` now provides `setMinMaxSize`, `setMouseTracingEnabled` / `mouseTracingEnabled`, `checkable` / `checked` / `setCheckable` / `setChecked`, `setObjectName` / `objectName`.
- **TUI**: `Label` now provides `setAutoSizeEnabled` / `autoSizeEnabled`, `setAlignment` / `alignment`, `text()`, and constructors accepting an explicit `Size` or `Object* parent`.
- **TUI**: `AbstractLayout` now provides `replaceWidget` (two overloads: by index and by iterator).
- **TUI**: `Terminal` now provides wide-character output (`printW`, `printLineW`, `printErrorW`, streamed `print()` / `perror()`) plus `moveUp/Down/Left/RightCursor` helpers.
- **OS/File**: `File::readLine()` now accepts an optional `size_t limit_length = 0` parameter to cap the number of bytes read per line.
- **Parser**: `CommandParser` now exposes `generateHelpInfo()` which auto-builds formatted help text (with configurable description width, sorting, and compact mode) from added commands.
- Added the `read` sample program to print file contents to the terminal.

### Changed

> ⚠️ Entries marked with **Breaking** are backwards-incompatible API changes — review carefully before upgrading.

- **[Breaking] TUI**: `Application` constructor changed from `Application(int argc, char* argv[])` to `explicit Application()`. Argument handling is now the caller's responsibility.
- **[Breaking] TUI**: `AbstractWidget` constructor signature changed — it now requires a `std::type_index type_id` parameter and accepts an optional `Object* parent`; a second constructor overload skips `Position`/`Size`.
- **[Breaking] TUI**: `AbstractLayout` constructor changed from `AbstractLayout(const std::string&)` to `AbstractLayout(const std::string&, std::type_index type_id, Object* parent = nullptr)`.
- **[Breaking] TUI**: `AbstractWidget::rename()` was renamed to `setObjectName()` (with `renameObject()` kept as a compatibility alias); `name()` is now `objectName()` uniformly.
- **[Breaking] TUI**: `Style` and `Corner` were promoted from `Renderer` namespace to the top-level `TUI` namespace. `Renderer::Style` and `Renderer::Corner` remain as deprecated typedef aliases and will be removed in a future version.
- **[Breaking] TUI**: `AbstractWidget::draw()` is marked deprecated and will be removed in a future version.
- **[Breaking] TUI**: Input-key aliases (`KEY_BACKSPACE`, `KEY_ENTER`, etc.) migrated from constant-style macros to `KEY()` / `SP_KEY()` factory functions.
- **[Breaking] TUI**: `SizePolicy` enum member ordering adjusted (`Ignored` is now listed first).
- **TUI**: `Renderer::setSSFX` now has a new variadic format-string overload.
- **TUI**: `Position` / `Size` structs gained comparison, swap, arithmetic operators, and additional constructor overloads.
- **Terminal**: Rendering logic for consecutive wide-character output was significantly optimized.
- **DateTime**: Timestamp formatting functionality was substantially improved.
- **Documentation**: All project docs (English and Chinese) were rewritten to cover new widgets, new APIs, and deprecation notices.

### Deprecated

- **TUI**: `Tiny::Code` — alias for `Tiny::U8Code`, kept for backwards compatibility. Use `Tiny::U8Code`.
- **TUI**: `Renderer::Style` / `Renderer::Corner` — typedef aliases of `TUI::Style` / `TUI::Corner`, kept for backwards compatibility. Use `TUI::Style` / `TUI::Corner` directly.
- **TUI**: `AbstractWidget::draw()` — marked deprecated; prefer the new renderer-driven drawing flow.

### Fixed

- **TUI/LineEdit**: Fixed multiple known issues during text input.
- **TUI/Button**: Completed basic functionality and fixed known defects.
- **TUI/Slider**: Fixed orientation and value-range issues.
- **MSVC**: Fixed multiple compilation errors under MSVC.
- **UTF-8**: Fixed and optimized UTF-8 string splitting and display-width calculation.
- **API**: Fixed several API signatures that had drifted from documentation.
- **Terminal/FreeBSD**: Fixed incorrect cursor-position retrieval on FreeBSD.
- **Net/Documentation**: Fixed docs that incorrectly referenced `Tiny::Net::parseFromHostname()` / `parseFirstHostname()` free functions (which never existed in the API). The correct entry points are `Address::parseFromHostname()` / `Address::parseFirstHostname()` static methods.

---

## [0.2.0]

### Added

- **OS**: Added `Tiny::OS::exec()`, which executes system commands with a configurable timeout (in milliseconds) and can capture stdout and stderr separately.
- **OS**: Added support for retrieving CPU, memory, disk and other system information on FreeBSD.
- **TUI**: Added support for capturing mouse press and release events in tty terminal environments.
- **TUI**: Added support for the `wcwidth` function on Windows, used to calculate the width of wide characters.
- **Terminal**: Added the ability to output wide characters.
- **DateTime**: Added support for outputting standard time formats and improved formatting-string and related APIs.
- **Net**: Added the `ping` sample program for testing network connectivity.
- Added the `du` sample program for viewing the actual data size occupied by a specified path.
- Added project-related documentation (including the module integration guide).
- Added a FreeBSD CI workflow.

### Changed

- Enhanced the TUI module and further adjusted related APIs; improved the overall functionality of the TUI module.
- Optimized the Renderer rendering mechanism and Terminal's handling of continuous wide-character output.
- **Net**: Added more Socket options and fixed some existing Socket options; improved Net-related functionality so the `ping` program can report results correctly.
- **DateTime**: The format-string feature now supports escape characters to avoid unnecessary parsing.
- Continued fixing multi-platform workflows and package retrieval issues.

### Fixed

- Fixed an issue where `OS::exec()` failed to exit after the command had already timed out.
- Fixed compilation errors on Unix-like systems other than Linux (e.g., FreeBSD, macOS).
- Fixed a program crash that occurred when pressing a key on FreeBSD.
- Fixed a segmentation fault caused by frequent event capturing on Unix.
- Fixed compilation issues on macOS.
- Fixed several known issues in the `ping` program.
- Fixed the logic for reading files so that it only breaks lines when it encounters the `'
'` character in the file content.