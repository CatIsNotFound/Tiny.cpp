# Changelog

## [1.2.0] - 2026-08-08

**🎉 The Tiny.cpp project now supports building with XMake.**

### Added

- **Net**: Added `Net` module for network communication, including:
  - `Net::Address` class for IPv4/IPv6 address parsing and management.
  - `Net::Socket` class supporting TCP/UDP/SCTP protocols with connect, bind, listen, accept, send, and recv operations.
  - `Net::parseFromHostname` / `Net::parseFirstHostname` for hostname resolution.
  - `Net::PortProtocol` enum listing common protocol ports (HTTP, HTTPS, SSH, DNS, etc.).
  - `Net::SocketOption` enum and `Net::OptionValue` for fine-grained socket option control (keepalive, non-blocking, buffer sizes, etc.).
  - `Net::Advanced` namespace for custom socket option registration.
- **Parser**: Added `IniParser` class for parsing and manipulating INI configuration files, with support for groups, key-value pairs, and dump/serialize.
- **OS/File**: Added `OS::autoConvertDataSize()` for automatic data size unit conversion, and `OS::dataUnitName()` for unit name lookup.
- **OS/File**: Added `OS::Permission` enum (P_None, P_Read, P_Write, P_Execute, P_All, etc.) for file permission representation.
- **OS/File**: Added `Path::lastAccessTime()`, `Path::lastWriteTime()`, `Path::lastCreateTime()` for file timestamp queries.
- **OS/File**: Added `Path::userPermission()`, `Path::groupPermission()`, `Path::otherPermission()` for file permission queries.
- **OS/System**: Added `FileSystem::listPathEx()` with layered directory traversal, returning `LayerMap` (per-depth path results) with stop/callback control.
- **TUI/Terminal**: Added `Terminal::self()` singleton, `Terminal::print()` / `Terminal::perror()` stream-style output, and `operator<<` for chained output.
- **TUI/Terminal**: Added `Terminal::printError()` for formatted error output (to stderr).
- **TUI/Terminal**: Added cursor movement helpers: `moveUpCursor()`, `moveDownCursor()`, `moveLeftCursor()`, `moveRightCursor()`.
- **TUI/Terminal**: Added `KEY_BACKSPACE()` and `KEY_ENTER()` constexpr helper functions for key matching.
- **TUI/Terminal**: Added `TStyle` namespace with stream-style terminal styling functions (`bg()`, `fg()`, `bold()`, `italic()`, `underline()`, `blink()`, `reverse()`, `showcur()`, `hidecur()`, `striketh()`, `reset()`).

### Changed

- **Core**: `Tiny.hpp` now includes `Net/Socket.hpp` and `Parser/IniParser.hpp`.
- **TUI/TUI**: `AbstractWidget::renderEvent()` is now pure virtual. Added pure virtual `resizeEvent()` and `moveEvent()`, and public `draw()` method.
- **CI/CD**: Upgraded build workflow (`beta.yml`) to support multi-architecture builds:
  - Linux: Added matrix strategy for x86_64 (`ubuntu-latest`) and arm64 (`ubuntu-24.04-arm`).
  - macOS: Added matrix strategy for x86_64 (`macos-15-intel`) and arm64 (`macos-latest`).
  - Windows: Split into MSVC (`env-windows-msvc`) and MinGW/MSYS2 (`env-windows-mingw`) jobs.

---

## [1.2.0] - 2026-08-08

**🎉 Tiny.cpp 项目现已支持使用 XMake 构建**

### 新增

- **Net**: 新增 `Net` 网络通信模块，包括：
  - `Net::Address` 类，用于 IPv4/IPv6 地址解析与管理。
  - `Net::Socket` 类，支持 TCP/UDP/SCTP 协议，提供 connect、bind、listen、accept、send、recv 等操作。
  - `Net::parseFromHostname` / `Net::parseFirstHostname` 用于主机名解析。
  - `Net::PortProtocol` 枚举，列出常见协议端口（HTTP、HTTPS、SSH、DNS 等）。
  - `Net::SocketOption` 枚举与 `Net::OptionValue`，用于精细控制套接字选项（keepalive、非阻塞、缓冲区大小等）。
  - `Net::Advanced` 命名空间，支持自定义套接字选项注册。
- **Parser**: 新增 `IniParser` 类，用于解析和操作 INI 配置文件，支持分组、键值对以及序列化输出。
- **OS/File**: 新增 `OS::autoConvertDataSize()` 用于自动转换数据大小单位，以及 `OS::dataUnitName()` 用于获取单位名称。
- **OS/File**: 新增 `OS::Permission` 枚举（P_None、P_Read、P_Write、P_Execute、P_All 等），用于表示文件权限。
- **OS/File**: 新增 `Path::lastAccessTime()`、`Path::lastWriteTime()`、`Path::lastCreateTime()` 用于查询文件时间戳。
- **OS/File**: 新增 `Path::userPermission()`、`Path::groupPermission()`、`Path::otherPermission()` 用于查询文件权限。
- **OS/System**: 新增 `FileSystem::listPathEx()` 分层目录遍历方法，返回 `LayerMap`（按深度分层的路径结果），支持停止/回调控制。
- **TUI/Terminal**: 新增 `Terminal::self()` 单例、`Terminal::print()` / `Terminal::perror()` 流式输出以及 `operator<<` 链式输出。
- **TUI/Terminal**: 新增 `Terminal::printError()` 用于格式化错误输出（输出到 stderr）。
- **TUI/Terminal**: 新增光标移动辅助函数：`moveUpCursor()`、`moveDownCursor()`、`moveLeftCursor()`、`moveRightCursor()`。
- **TUI/Terminal**: 新增 `KEY_BACKSPACE()` 和 `KEY_ENTER()` constexpr 辅助函数，用于按键匹配。
- **TUI/Terminal**: 新增 `TStyle` 命名空间，提供流式终端样式函数（`bg()`、`fg()`、`bold()`、`italic()`、`underline()`、`blink()`、`reverse()`、`showcur()`、`hidecur()`、`striketh()`、`reset()`）。

### 变更

- **Core**: `Tiny.hpp` 现在包含 `Net/Socket.hpp` 和 `Parser/IniParser.hpp`。
- **TUI/TUI**: `AbstractWidget::renderEvent()` 改为纯虚函数。新增纯虚函数 `resizeEvent()` 和 `moveEvent()`，以及公开方法 `draw()`。
- **CI/CD**: 升级构建工作流（`beta.yml`），支持多架构构建：
  - Linux：新增矩阵策略，支持 x86_64（`ubuntu-latest`）和 arm64（`ubuntu-24.04-arm`）。
  - macOS：新增矩阵策略，支持 x86_64（`macos-15-intel`）和 arm64（`macos-latest`）。
  - Windows：拆分为 MSVC（`env-windows-msvc`）和 MinGW/MSYS2（`env-windows-mingw`）两个任务。
