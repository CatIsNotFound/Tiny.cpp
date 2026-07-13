# Changelog

## [1.1.1] - 2026-07-13

### Fixed

- **DateTime**: Fixed incorrect weekday calculation in the constructor `DateTime(year, month, day, ...)`. The weekday is now correctly derived from the computed timestamp instead of relying on the caller-provided value.

### Changed

- **Demos**: Removed the `Edit` (DemoTextEditor) demo module from the build.

### Added

- **CI/CD**: Added GitHub Actions workflow for automated builds and tests on Linux (Ubuntu) , macOS, and Windows.

---

## [1.1.1] - 2026-07-13

### 修复

- **DateTime**: 修复了构造函数 `DateTime(year, month, day, ...)` 中星期计算不正确的问题。现在星期值根据计算出的时间戳正确推导，而不再依赖调用者传入的值。

### 变更

- **Demos**: 从构建中移除了 `Edit`（DemoTextEditor）示例模块。

### 新增

- **CI/CD**: 新增 GitHub Actions 工作流，支持在 Linux (Ubuntu)、 macOS 和 Windows 上进行自动构建与测试。
