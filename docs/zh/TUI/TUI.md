# TUI 模块

命名空间: `Tiny::TUI`

---

## 目录

1. [模块简介](#1-模块简介)
2. [头文件](#2-头文件)
3. [U8Code 命名空间](#3-u8code-命名空间)
4. [辅助函数](#4-辅助函数)
5. [数据结构](#5-数据结构)
6. [Terminal 类](#6-terminal-类)
7. [Renderer 类](#7-renderer-类)
8. [Object 类](#8-object-类)
9. [AbstractWidget 类](#9-abstractwidget-类)
10. [EventBus 类](#10-eventbus-类)
11. [Application 类](#11-application-类)
12. [AbstractLayout 类](#12-abstractlayout-类)
13. [CurBlock 类](#13-curblock-类)
14. [Label 类](#14-label-类)
15. [Button 类](#15-button-类)
16. [LineEdit 类](#16-lineedit-类)
17. [Slider 类](#17-slider-类)
18. [ProgressBar 类](#18-progressbar-类)
19. [ListView 类](#19-listview-类)
20. [使用示例](#20-使用示例)
21. [注意事项](#21-注意事项)
22. [如何在 Linux 控制台下使用 GPM 库](#22-如何在-linux-控制台下使用-gpm-库)

---

## 1. 模块简介

`TUI` 模块提供终端用户界面功能，包括：

- **终端控制**: 原始模式切换、屏幕控制、光标操作
- **输入处理**: 按键读取、鼠标事件
- **颜色样式**: 前景色、背景色、粗体、下划线等
- **双缓冲渲染**: 高效的屏幕渲染
- **Object 层级**: `Object` 基类提供父子关系管理
- **控件系统**: 可扩展的控件层级，以 `AbstractWidget` 为基类
- **事件总线**: 通过 `EventBus` 实现解耦的事件订阅和分发
- **应用框架**: 顶层 `Application` 管理事件循环和控件渲染

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/TUI/TUI.hpp>
// 直接复制源代码方式
#include "TUI/TUI.hpp"
```

---

## 3. U8Code 命名空间

`Tiny::U8Code` 命名空间提供 UTF-8 工具函数，定义在 `Terminal.hpp` 中。

> **已弃用别名**: `Tiny::Code` 是 `Tiny::U8Code` 的别名。请使用 `Tiny::U8Code`；`Tiny::Code` 将从 v0.3.0 起移除。

### 3.1 宽字符串转换

```cpp
// Windows 版本（带 codepage 参数，默认 65001 = UTF-8）
std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001);
std::string wide2String(const std::wstring& str, uint32_t codepage = 65001);

// Unix 版本
std::wstring string2Wide(const std::string& str);
std::string wide2String(const std::wstring& str);
```

### 3.2 UTF-8 字符串工具

#### splitFront

```cpp
std::string splitFront(const char* data);
```
- **功能**: 提取 UTF-8 字符串的第一个字符
- **参数**: `data` - UTF-8 字符串
- **返回值**: 第一个字符（可能多字节）

#### splitUTF8

```cpp
std::vector<std::string> splitUTF8(const char* data, size_t *display_size = nullptr);
```
- **功能**: 将 UTF-8 字符串分割为字符数组
- **参数**:
  - `data` - UTF-8 字符串
  - `display_size` - 可选输出参数，用于获取显示宽度（默认值：`nullptr`）
- **返回值**: 字符数组

#### calcStrDisplayWidth

```cpp
size_t calcStrDisplayWidth(const std::string& str);
```
- **功能**: 计算 UTF-8 字符串的显示宽度（终端列数）
- **参数**: `str` - UTF-8 字符串
- **返回值**: 显示宽度（CJK 字符计为 2）

#### calcDisplaySize

```cpp
size_t calcDisplaySize(const std::string& str);
```
- **功能**: 计算 UTF-8 字符串的显示大小
- **参数**: `str` - UTF-8 字符串
- **返回值**: 显示大小

#### subUTF8

```cpp
std::string subUTF8(const char* data, size_t display_count, size_t offset = 0,
                    size_t *result_display_count = nullptr);
```
- **功能**: 按显示宽度计数和偏移量截取子串
- **参数**:
  - `data` - UTF-8 字符串
  - `display_count` - 要截取的显示列数
  - `offset` - 显示宽度偏移量（默认：`0`）
  - `result_display_count` - 可选输出：实际截取的显示宽度
- **返回值**: UTF-8 子串

#### lastCharCount

```cpp
size_t lastCharCount(const std::string& buf);
```
- **功能**: 获取缓冲区中最后一个 UTF-8 字符的字节长度
- **参数**: `buf` - UTF-8 字符串
- **返回值**: 最后一个字符的字节长度

---

## 4. 辅助函数

### 4.1 getKeyName

```cpp
const char* getKeyName(const uint8_t& KEY, const SP_Keys& SP);
```
- **功能**: 获取按键名称
- **参数**:
  - `KEY` - 按键码
  - `SP` - 特殊键类型
- **返回值**: 按键名称字符串

### 4.2 getMouseName

```cpp
const char* getMouseName(const SP_Mouse& SP);
```
- **功能**: 获取鼠标事件名称
- **参数**: `SP` - 鼠标事件类型
- **返回值**: 事件名称字符串

### 4.3 isPointInRect

```cpp
bool isPointInRect(const Position& point, const Position& start_pos, const Position& end_pos);
bool isPointInRect(const Position& point, const Position& start_pos, const Size& size);
```
- **功能**: 检查点是否在矩形内
- **参数**:
  - `point` - 要检查的点
  - `start_pos` - 矩形左上角
  - `end_pos` - 矩形右下角（第一个重载）
  - `size` - 矩形大小（第二个重载）
- **返回值**: `true` 表示点在矩形内

### 4.4 KEY_BACKSPACE

```cpp
constexpr bool KEY_BACKSPACE(uint8_t key);
```
- **功能**: 检查按键是否为退格键
- **参数**: `key` - 按键码
- **返回值**: `true` 表示是退格键（`KEY_BK` (8) 或 `KEY_DEL` (127)）
- **说明**: constexpr 辅助函数，用于按键匹配

### 4.5 KEY_ENTER

```cpp
constexpr bool KEY_ENTER(uint8_t key);
```
- **功能**: 检查按键是否为回车键
- **参数**: `key` - 按键码
- **返回值**: `true` 表示是回车键（`KEY_CR` (13) 或 `KEY_LF` (10)）
- **说明**: constexpr 辅助函数，用于按键匹配

### 4.6 KEY_RETURN

```cpp
constexpr bool KEY_RETURN(uint8_t key);
```
- **功能**: `KEY_ENTER` 的别名

### 4.7 KEY_CONFIRM

```cpp
constexpr bool KEY_CONFIRM(uint8_t key);
```
- **功能**: 检查按键是否为确认键（Space、CR 或 LF）

### 4.8 KEY_CANCEL

```cpp
constexpr bool KEY_CANCEL(uint8_t key);
```
- **功能**: 检查按键是否为取消键（BK、DEL 或 ESC）

---

## 5. 数据结构

### 5.1 Size 结构体

```cpp
struct Size {
    uint32_t width;   // 宽度（列数）
    uint32_t height;  // 高度（行数）

    Size();
    Size(uint32_t width, uint32_t height);

    bool operator==(const Size& other) const;
    bool operator!=(const Size& other) const;
    bool isEqual(const Size& other) const;
    int8_t compare(const Size& other) const;

    Size& operator+(const Size& other);
    Size& operator+=(const Size& other);
    Size& operator-(const Size& other);
    Size& operator-=(const Size& other);
    Size& operator*(uint32_t value);
    Size& operator*=(uint32_t value);
};
```

| 成员 | 说明 |
|------|------|
| `Size()` | 默认构造，`{0, 0}` |
| `Size(w, h)` | 显式宽高构造 |
| `isEqual(other)` | `width * height == other.width * other.height`（总面积相同） |
| `compare(other)` | 较小返回 `1`，较大返回 `-1`，相等返回 `0`（先比较高度再比较宽度） |
| `operator+/-/*` | 算术运算符（原地修改，返回 `*this`） |

### 5.2 Position 结构体

```cpp
struct Position {
    uint32_t row;     // 行号（0-based）
    uint32_t column;  // 列号（0-based）

    Position();
    Position(uint32_t row, uint32_t column);
    Position(const Size& size);  // 从 Size 构造：row=height, column=width

    Position calcEndPos(const Size& size) const;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    int8_t compare(const Position& other) const;
    void swap(Position& other) noexcept;

    Position& operator+(const Position& other);
    Position& operator+=(const Position& other);
    Position& operator-(const Position& other);
    Position& operator-=(const Position& other);
};
```

| 成员 | 说明 |
|------|------|
| `Position()` | 默认构造，`{0, 0}` |
| `Position(r, c)` | 显式行列构造 |
| `Position(const Size&)` | 从 `Size` 构造 `{height, width}` |
| `calcEndPos(size)` | 返回 `{row + size.height - 1, column + size.width - 1}` |
| `compare(other)` | 较小返回 `1`，较大返回 `-1`，相等返回 `0`（先行后列） |
| `swap(other)` | 交换位置，noexcept |
| `operator+/-` | 算术运算符（原地修改，返回 `*this`） |

### 5.3 Color 枚举

```cpp
enum class Color : uint8_t {
    Black   = 0,
    Red     = 1,
    Green   = 2,
    Yellow  = 3,
    Blue    = 4,
    Magenta = 5,
    Cyan    = 6,
    White   = 7,
    Default = 9
};
```

### 5.4 Keys 枚举

```cpp
enum Keys : uint8_t {
    KEY_NONE        = 0,
    KEY_NULL        = 0,
    KEY_SOH         = 1,
    KEY_STX         = 2,
    KEY_ETX         = 3,
    KEY_EOT         = 4,
    KEY_ENQ         = 5,
    KEY_ACK         = 6,
    KEY_BELL        = 7,
    KEY_BK          = 8,
    KEY_TAB         = 9,
    KEY_LF          = 10,
    KEY_VT          = 11,
    KEY_FF          = 12,
    KEY_CR          = 13,
    KEY_SO          = 14,
    KEY_SI          = 15,
    KEY_DLE         = 16,
    KEY_DC1         = 17,
    KEY_DC2         = 18,
    KEY_DC3         = 19,
    KEY_DC4         = 20,
    KEY_NAK         = 21,
    KEY_SYN         = 22,
    KEY_ETB         = 23,
    KEY_CAN         = 24,
    KEY_ESC         = 27,
    KEY_FS          = 28,
    KEY_GS          = 29,
    KEY_RS          = 30,
    KEY_US          = 31,
    KEY_SPACE       = 32,

    KEY_DEL         = 127,

    KEY_CTRL_A      = 1,
    KEY_CTRL_B      = 2,
    KEY_CTRL_C      = 3,
    KEY_CTRL_D      = 4,
    KEY_CTRL_E      = 5,
    KEY_CTRL_F      = 6,
    KEY_CTRL_G      = 7,
    KEY_CTRL_H      = 8,
    KEY_CTRL_I      = 9,
    KEY_CTRL_J      = 10,
    KEY_CTRL_K      = 11,
    KEY_CTRL_L      = 12,
    KEY_CTRL_M      = 13,
    KEY_CTRL_N      = 14,
    KEY_CTRL_O      = 15,
    KEY_CTRL_P      = 16,
    KEY_CTRL_Q      = 17,
    KEY_CTRL_R      = 18,
    KEY_CTRL_S      = 19,
    KEY_CTRL_T      = 20,
    KEY_CTRL_U      = 21,
    KEY_CTRL_V      = 22,
    KEY_CTRL_W      = 23,
    KEY_CTRL_X      = 24,
    KEY_CTRL_Y      = 25,
    KEY_CTRL_Z      = 26,

    KEY_SPECIAL     = 254,
    KEY_UNKNOWN     = 255
};
```

**按键别名**: constexpr 辅助函数提供 `KEY_BACKSPACE`、`KEY_ENTER` 等（参见 [辅助函数](#4-辅助函数)）。

### 5.5 SP_Keys 枚举（特殊键）

```cpp
enum SP_Keys : uint8_t {
    SP_KEY_UNKNOWN,
    SP_KEY_NONE = 0,
    SP_KEY_F1, SP_KEY_F2, SP_KEY_F3, SP_KEY_F4,
    SP_KEY_F5, SP_KEY_F6, SP_KEY_F7, SP_KEY_F8,
    SP_KEY_F9, SP_KEY_F10, SP_KEY_F11, SP_KEY_F12,
    SP_KEY_INSERT,
    SP_KEY_DELETE,
    SP_KEY_HOME,
    SP_KEY_END,
    SP_KEY_PAGE_UP,
    SP_KEY_PAGE_DOWN,
    SP_KEY_CENTER,
    SP_KEY_UP,
    SP_KEY_LEFT,
    SP_KEY_DOWN,
    SP_KEY_RIGHT,
    SP_KEY_PRINTSCR,
    SP_KEY_CTRL,
    SP_KEY_SHIFT,
    SP_KEY_ALT,
    SP_KEY_CAPSLOCK,
    SP_KEY_NUMLOCK,
    SP_KEY_SCROLLLOCK
};
```

### 5.6 SP_Mouse 枚举（鼠标事件）

```cpp
enum SP_Mouse : uint8_t {
    SP_MOUSE_UNKNOWN,
    SP_MOUSE_LEFT_BUTTON,
    SP_MOUSE_MIDDLE_BUTTON,
    SP_MOUSE_RIGHT_BUTTON,
    SP_MOUSE_WHEEL_UP,
    SP_MOUSE_WHEEL_DOWN,
    SP_MOUSE_MOVED,
    SP_MOUSE_RELEASE,
    // 别名（相同值，0-7）:
    MOUSE_UNKNOWN       = 0,
    MOUSE_LEFT_BUTTON   = 1,
    MOUSE_MIDDLE_BUTTON = 2,
    MOUSE_RIGHT_BUTTON  = 3,
    MOUSE_WHEEL_UP      = 4,
    MOUSE_WHEEL_DOWN    = 5,
    MOUSE_MOVED         = 6,
    MOUSE_RELEASE       = 7
};
```

### 5.7 InputEvent 结构体

```cpp
struct InputEvent {
    enum Type : uint8_t {
        None, N = 0,
        Keyboard, Key = 1, K = 1,
        Mouse, M = 2
    } type;
    union Input {
        struct Keyboard {
            uint8_t key;
            SP_Keys sp_key;
            bool is_pressed;    // P.s: 仅 Windows 可捕获，其他平台始终为 `true`!
        } keyboard;
        struct Mouse {
            Position position;
            SP_Mouse button;
            bool is_pressed;
        } mouse;
    } input;
};
```

| 成员 | 类型 | 说明 |
|------|------|------|
| `type` | `Type` | 事件类型 |
| `input.keyboard.key` | `uint8_t` | 按键码 |
| `input.keyboard.sp_key` | `SP_Keys` | 特殊键类型 |
| `input.keyboard.is_pressed` | `bool` | 按键是否按下（仅 Windows） |
| `input.mouse.position` | `Position` | 鼠标位置 |
| `input.mouse.button` | `SP_Mouse` | 鼠标按钮/事件 |
| `input.mouse.is_pressed` | `bool` | 鼠标按钮是否按下 |

**Type 别名**:
- `None = N = 0`
- `Keyboard = Key = K = 1`
- `Mouse = M = 2`

### 5.8 类型别名

```cpp
using KeyEvent = InputEvent::Input::Keyboard;
using MouseEvent = InputEvent::Input::Mouse;
```

### 5.9 RGBColor 结构体

```cpp
struct RGBColor {
    uint8_t r, g, b;

    RGBColor();
    RGBColor(uint8_t r, uint8_t g, uint8_t b);
    bool operator==(const RGBColor& other) const;
    bool operator!=(const RGBColor& other) const;
};
```

### 5.10 Char 类

```cpp
class Char {
public:
    Char();
    Char(const char* data);
    Char(const std::string& data);
    Char& operator=(const std::string& ch);
    Char& operator=(const char* ch);
    Char& operator=(const Char& ch);
    bool operator==(const Char& other) const;
    bool operator!=(const Char& other) const;
    const std::string& data() const;
    uint8_t length() const;
};
```

单个字符的轻量级封装（支持 UTF-8 多字节字符）。

### 5.11 Alignment 枚举

```cpp
enum class Alignment : uint8_t {
    LeftTop,
    CenterTop,
    RightTop,
    Left,
    Center,
    Right,
    LeftBottom,
    CenterBottom,
    RightBottom
};
```

控件在布局中的对齐方式。

### 5.12 TextAlignment 枚举

```cpp
enum class TextAlignment : uint8_t {
    Left,
    Center,
    Right
};
```

控件内文本的对齐方式。

### 5.13 SizePolicy 枚举

```cpp
enum class SizePolicy : uint8_t {
    Fixed,
    Maximized,
    Minimized,
    Ignored
};
```

控件在布局管理中的尺寸策略。

### 5.14 Orientation 枚举

```cpp
enum class Orientation : uint8_t {
    Horizontal, H = 0,
    Vertical, V = 1
};
```

用于 `Slider` 和 `ProgressBar` 的方向。

---

## 6. Terminal 类

### 6.1 类简介

终端控制类，提供原始模式切换、屏幕控制、光标操作、颜色设置、输入读取等功能。

#### self

```cpp
static Terminal& self();
```
- **功能**: 获取终端单例实例
- **返回值**: Terminal 引用
- **说明**: 支持流式输出链式调用

### 6.2 原始模式控制

#### enterRawMode

```cpp
static bool enterRawMode();
```
- **功能**: 进入原始模式（禁用行缓冲、回显等）
- **返回值**: `true` 表示成功
- **注意事项**:
  - Windows: 创建新的屏幕缓冲区
  - Unix: 使用 termios 设置原始模式

#### leaveRawMode

```cpp
static bool leaveRawMode();
```
- **功能**: 退出原始模式，恢复终端设置
- **返回值**: `true` 表示成功

#### isInRawMode

```cpp
static bool isInRawMode();
```
- **功能**: 检查当前是否在原始模式
- **返回值**: `true` 表示在原始模式

### 6.3 屏幕信息

#### screenSize

```cpp
static Size screenSize();
```
- **功能**: 获取终端屏幕尺寸
- **返回值**: `Size` 结构体（宽度和高度）

#### cursorPosition

```cpp
static Position cursorPosition();
```
- **功能**: 获取光标当前位置
- **返回值**: `Position` 结构体（行和列）

### 6.4 输出函数

#### print

```cpp
static bool print(char ch);
static bool print(const std::string& text);
```
- **功能**: 输出单个字符或文本（不换行）

#### printW

```cpp
static bool printW(wchar_t ch);
static bool printW(const std::wstring& text);
```
- **功能**: 输出宽字符或宽文本（不换行）

#### printLine

```cpp
static bool printLine(const std::string& text = {});
```
- **功能**: 输出文本并换行

#### printLineW

```cpp
static bool printLineW(const std::wstring& text = {});
```
- **功能**: 输出宽文本并换行

#### printFormat

```cpp
template<typename ... Args>
static bool printFormat(const char* format, Args... args);
```
- **功能**: 格式化输出（使用 `{}` 作为占位符）

#### formatString

```cpp
template<typename ... Args>
static std::string formatString(const char* format, Args... args);
```
- **功能**: 格式化字符串（不输出）
- **返回值**: 格式化后的字符串

#### printError

```cpp
template<typename ... Args>
static bool printError(const char* format, Args... args);
static bool printError(const std::string& text);
static bool printErrorW(const std::wstring& text);
```
- **功能**: 向 stderr 输出错误文本
- **重载**:
  - 带可变参数的模板版本
  - 直接的 `std::string` 版本
  - 宽字符版本

#### 流式 print

```cpp
static Terminal& print();      // 流式风格，无需参数
static Terminal& perror();     // 流式风格错误输出
Terminal& operator<<(const std::string& text);
Terminal& operator<<(char ch);
Terminal& operator<<(int value);
Terminal& operator<<(bool expr);
Terminal& operator<<(const wchar_t* expr);
```
- **说明**: 支持流畅 API 风格：`Terminal::self() << "Hello " << 42`

### 6.5 屏幕控制

#### clearScreen

```cpp
static bool clearScreen();
```

#### clearInRow

```cpp
static bool clearInRow(uint8_t row);
```

#### moveCursor

```cpp
static bool moveCursor(Position position);
static bool moveCursor(uint32_t row, uint32_t column);
```

#### moveUpCursor / moveDownCursor / moveLeftCursor / moveRightCursor

```cpp
static bool moveUpCursor(uint32_t rows = 1);
static bool moveDownCursor(uint32_t rows = 1);
static bool moveLeftCursor(uint32_t cols = 1);
static bool moveRightCursor(uint32_t cols = 1);
```

#### setScrollRegion / resetScrollRegion

```cpp
static bool setScrollRegion(uint32_t row_start, uint32_t row_end);
static bool resetScrollRegion();
```

#### flushScreen

```cpp
static bool flushScreen();
```

### 6.6 输入函数

#### readLine / readLineW

```cpp
static std::string readLine();
static std::wstring readLineW();
```

#### getKey

```cpp
static uint8_t getKey(SP_Keys* sp_key = nullptr);
```

### 6.7 鼠标控制

#### setMouseEnabled

```cpp
static bool setMouseEnabled(bool enabled);
```

#### getMouseButton

```cpp
static uint8_t getMouseButton(Position* mouse_pos = nullptr, bool* is_pressed = nullptr);
```

#### getInput

```cpp
static InputEvent getInput();
```

### 6.8 颜色与样式函数

#### 颜色设置

```cpp
static void setBackgroundColor(Color color, bool intensity = true);
static void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
static void setForegroundColor(Color color, bool intensity = false);
static void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
```

#### 样式设置

```cpp
static void setBolder(bool enable);
static void setDark(bool enable);
static void setItalic(bool enable);
static void setUnderline(bool enable);
static void setBlinking(bool enable);
static void setReverseColor(bool enable);
static void setCursorVisible(bool enable);
static void setStrikethrough(bool enable);
static void reset();
```

### 6.9 TStyle 命名空间（v1.2.0）

流式风格的终端颜色和样式设置接口。所有函数返回 `Terminal&` 以支持方法链式调用。

```cpp
namespace TStyle {
    Terminal& bg(Color color, bool intense = false);
    Terminal& bg(uint8_t r, uint8_t g, uint8_t b);
    Terminal& fg(Color color, bool intense = true);
    Terminal& fg(uint8_t r, uint8_t g, uint8_t b);
    Terminal& bold(bool enabled = true);
    Terminal& italic(bool enabled = true);
    Terminal& underline(bool enabled = true);
    Terminal& blink(bool enabled = true);
    Terminal& reverse(bool enabled = true);
    Terminal& showcur();
    Terminal& hidecur();
    Terminal& striketh(bool enabled = true);
    Terminal& reset();
}
```

**示例**:
```cpp
Terminal::self() << TStyle::fg(Color::Green) << TStyle::bold() << "绿色粗体文本";
Terminal::self() << TStyle::bg(255, 0, 0) << "红色背景";
Terminal::self() << TStyle::reset();
```

---

## 7. Renderer 类

### 7.1 类简介

双缓冲终端渲染器，支持字符绘制、矩形填充、边框绘制等功能。

### 7.2 嵌套结构体

#### Style（也可作为 `Tiny::TUI::Style` 使用）

```cpp
struct Style {
    uint8_t property;       // 样式属性（使用 Property 枚举）
    Color bg_color;         // 背景色（ANSI 16色）
    Color fg_color;         // 前景色（ANSI 16色）
    uint8_t intensity;      // 颜色强度：0=无, 1=仅背景, 2=仅前景, 3=全部
    bool used_rgb_color;    // 是否使用 RGB 颜色
    RGBColor bg_rgb_color;  // RGB 背景色
    RGBColor fg_rgb_color;  // RGB 前景色

    enum Property : uint8_t {
        Bolder            = 1,
        Dark              = 2,
        Italic            = 4,
        Underline         = 8,
        Blinking          = 16,
        Reverse           = 32,
        Strikethrough     = 64,
    };

    Style();
    void reset();
    bool isDefault() const;
    bool operator==(const Style& other) const;
    bool operator!=(const Style& other) const;
};
```

> **注意**: `Renderer::Style` 和 `Renderer::Corner` 是已弃用的 typedef。请直接使用 `Tiny::TUI::Style` 和 `Tiny::TUI::Corner` — 它们将从 v0.3.0 起移除。

| 成员 | 类型 | 说明 |
|------|------|------|
| `property` | `uint8_t` | 样式属性位掩码 |
| `bg_color` | `Color` | ANSI 背景色（默认值：`Color::Default`） |
| `fg_color` | `Color` | ANSI 前景色（默认值：`Color::Default`） |
| `intensity` | `uint8_t` | 颜色强度（默认值：2 = 仅前景） |
| `used_rgb_color` | `bool` | 是否使用 RGB 颜色 |
| `bg_rgb_color` | `RGBColor` | RGB 背景色 |
| `fg_rgb_color` | `RGBColor` | RGB 前景色 |

#### Cell

```cpp
struct Cell {
    Char data;
    bool is_dirty;
    Style style;

    Cell();
    void reset();
    void set(const char* ch, Style st);
};
```

#### Corner（也可作为 `Tiny::TUI::Corner` 使用）

```cpp
struct Corner {
    Char left_top{"+"};
    Char left{"|"};
    Char left_bottom{"+"};
    Char right_top{"+"};
    Char right{"|"};
    Char right_bottom{"+"};
    Char top{"-"};
    Char bottom{"-"};
};
```

### 7.3 静态成员函数

#### self

```cpp
static Renderer& self();
```

### 7.4 成员函数

#### set（重载）

```cpp
void set(const Position& pos, uint8_t ch, Style style = {});
void set(uint32_t x, uint32_t y, uint8_t ch, Style style = {});
void set(const Position& pos, const std::string& str, Style style = {});
void set(uint32_t x, uint32_t y, const std::string& str, Style style = {});
```

#### setStrF / setSSF / setSSFX

```cpp
template<typename ... Args>
void setStrF(const Position& pos, const char* format, Args... args);

template<typename ... Args>
void setSSF(const Position& pos, const char* format, const Style& style, Args... args);

template<typename ... Args>
void setSSFX(const Position& pos, const char* format, const StyleList& styles, Args... args);
```

#### fillScreen / fillRows / fillCols / fillRect

```cpp
void fillScreen(const Style& style = {});

void fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch = ' ', Style style = {});
void fillRows(uint32_t start_row, uint32_t end_row, const std::string& ch, Style style = {});

void fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch = ' ', Style style = {});
void fillCols(uint32_t start_col, uint32_t end_col, const std::string& ch, Style style = {});

void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch = ' ', Style style = {});
void fillRect(const Position& start_pos, const Position& end_pos, const std::string& str, Style style = {});
```

#### drawBorder

```cpp
void drawBorder(const Position& start_pos, const Position& end_pos, Corner corner, Style style = {});
```

#### unset / unsetRow / unsetCol / unsetRect

```cpp
void unset(const Position& pos);
void unset(uint32_t x, uint32_t y);
void unsetRow(uint32_t row);
void unsetCol(uint32_t col);
void unsetRect(const Position& start_pos, const Position& end_pos);
```

#### setStyle

```cpp
void setStyle(const Position& pos, Style style);
void setStyle(uint32_t x, uint32_t y, Style style);
```

#### charAt / styleAt

```cpp
const Char& charAt(const Position& position);
const Style& styleAt(const Position& position);
```

#### clear / present

```cpp
void clear();
void present();
```

#### setResizeEvent（已弃用）

```cpp
void setResizeEvent(const std::function<void(Renderer&)>& event);
```
- **已弃用**: 已被 `EventBus` 替代，将在 v1.4.0 移除。

### 7.5 受保护虚函数

```cpp
virtual void renderEvent();
virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
```

---

## 8. Object 类

### 8.1 类简介

TUI 层级中所有对象的基类。提供对象命名、父子关系、类型信息和事件分发。`AbstractWidget` 和 `AbstractLayout` 都（间接）继承自 `Object`。

### 8.2 构造函数

```cpp
explicit Object(const std::string& name, std::type_index type_id, Object* parent = nullptr);
explicit Object(const std::string& name, std::type_index type_id, std::type_index parent_type_id, Object* parent = nullptr);
```

| 参数 | 说明 |
|------|------|
| `name` | 对象名称 |
| `type_id` | 标识实际派生类型的 `std::type_index` |
| `parent_type_id` | 期望父类型的 `std::type_index`（仅第二个构造函数） |
| `parent` | 父对象指针（可选，默认：`nullptr`） |

### 8.3 析构函数

```cpp
virtual ~Object() = default;
```

### 8.4 成员函数

#### renameObject / setObjectName / objectName

```cpp
void renameObject(const std::string& name);
void setObjectName(const std::string& name);
[[nodiscard]] const std::string& objectName() const;
```

重命名或查询对象名称。`renameObject` 和 `setObjectName` 效果相同。

#### setParent / parent

```cpp
void setParent(Object* parent);
Object* parent() const;
```

#### hash / phash

```cpp
size_t hash() const;
size_t phash() const;
```

- `hash()` — 本对象的类型哈希
- `phash()` — 父对象的类型哈希

#### className

```cpp
const char* className() const;
```

从存储的 `type_index` 返回人类可读的类名。

#### isChild

```cpp
bool isChild(Object* child) const;
```

检查 `child` 是否为该对象的直接子对象。

#### findChild

```cpp
Object* findChild(const std::string& name) const;
Object* findChild(std::type_index type_id, const std::string& name) const;
```

按名称（第一个重载）或类型 + 名称（第二个重载）查找子对象。找不到返回 `nullptr`。

#### children

```cpp
[[nodiscard]] const std::vector<Object*>& children() const;
```

返回只读的直接子对象向量。

### 8.5 受保护虚函数

子类**必须**重写：

```cpp
virtual void onEvent(const AbstractEvent& event) = 0;
virtual void onResizedTermSize(const Size& size) = 0;
virtual void onObjectNameChanged() = 0;
virtual void onParentChanged() = 0;
```

框架在事件到达、终端尺寸变化或父/名称改变时调用这些函数。

---

## 9. AbstractWidget 类

### 9.1 类简介

抽象控件基类，继承自 `Object`。提供位置/大小管理、样式状态、可检查状态、鼠标追踪以及子类必须实现的渲染/事件接口。

### 9.2 StyleStatus 枚举

```cpp
enum StyleStatus : uint8_t {
    S_Disabled,
    S_Active,
    S_Checked,
    S_Normal
};
```

与 `setStyle()` 一起使用，将 `Style` 关联到控件状态。

### 9.3 构造函数

```cpp
explicit AbstractWidget(const std::string& name, const Position& position, const Size& size,
                        std::type_index type_id, Object* parent = nullptr);
explicit AbstractWidget(const std::string& name, std::type_index type_id, Object* parent = nullptr);
```

| 参数 | 说明 |
|------|------|
| `name` | 控件名称（传给 `Object` 基类） |
| `position` | 控件位置 |
| `size` | 控件大小 |
| `type_id` | 具体子类的 `std::type_index` |
| `parent` | 父对象（可选） |

### 9.4 析构函数

```cpp
virtual ~AbstractWidget() = default;
```

### 9.5 成员函数

#### move

```cpp
void move(const Position& position);
void move(uint32_t x, uint32_t y);
```

#### resize

```cpp
void resize(const Size& size);
void resize(uint32_t w, uint32_t h);
```

#### setMinimumSize / setMaximumSize / setMinMaxSize

```cpp
void setMinimumSize(const Size& size);
void setMinimumSize(uint32_t w, uint32_t h);

void setMaximumSize(const Size& size);
void setMaximumSize(uint32_t w, uint32_t h);

void setMinMaxSize(const Size& size);
void setMinMaxSize(uint32_t w, uint32_t h);
```

`setMinMaxSize` 同时设置最小和最大尺寸为相同值（用于锁定尺寸）。

#### setEnabled / setVisible / setFocus

```cpp
void setEnabled(bool enabled);
void setVisible(bool visible);
void setFocus(bool focus);
```

#### setSizePolicy

```cpp
void setSizePolicy(SizePolicy policy);
```

#### setMouseTracingEnabled / mouseTracingEnabled

```cpp
void setMouseTracingEnabled(bool enabled);
[[nodiscard]] bool mouseTracingEnabled() const;
```

启用后，即使未按下也能持续接收鼠标移动事件。

#### setStyle / style

```cpp
void setStyle(uint8_t status, const Style& style);
[[nodiscard]] Style style(uint8_t status) const;
```

将 `Style` 关联到控件状态（`S_Disabled`、`S_Active`、`S_Checked`、`S_Normal`）。

#### draw（已弃用）

```cpp
API_DEPRECATED("The function will be removed since ver.0.3.0!")
void draw();
```

> 请使用 `callDrawEvent()`（受保护）或让 `Application` 处理重绘。

#### Position / Size 获取器

```cpp
[[nodiscard]] const Position& position() const;
[[nodiscard]] const Size& size() const;
[[nodiscard]] const Size& minimumSize() const;
[[nodiscard]] const Size& maximumSize() const;
```

#### 状态获取器

```cpp
[[nodiscard]] bool enabled() const;
[[nodiscard]] bool visible() const;
[[nodiscard]] bool focus() const;
[[nodiscard]] SizePolicy sizePolicy() const;
```

#### 可检查状态

```cpp
[[nodiscard]] bool checkable() const;
[[nodiscard]] bool checked() const;
```

子类通过受保护方法控制可检查状态：

```cpp
protected:
    void setCheckable(bool checkable);
    void setChecked(bool checked);
```

### 9.6 受保护虚函数

所有子类**必须**实现：

```cpp
virtual void renderEvent(Renderer& renderer) = 0;
virtual void resizeEvent(uint32_t width, uint32_t height) = 0;
virtual void moveEvent(uint32_t x, uint32_t y) = 0;
virtual void keyEvent(KeyEvent keyboard) = 0;
virtual void mouseEvent(MouseEvent mouse) = 0;
virtual void focusEvent(bool focus) = 0;
virtual void enableEvent(bool enable) = 0;
virtual void clickedEvent() = 0;
```

子类可用的额外受保护辅助函数：

```cpp
void callDrawEvent();
void resizeWithoutCalledEvent(uint32_t width, uint32_t height);
const Style& currentStyle(uint8_t* status = nullptr) const;
```

---

## 10. EventBus 类

### 10.1 类简介

事件总线，用于管理和分发 TUI 系统中的事件。

### 10.2 事件类型

#### AbstractEvent

```cpp
class AbstractEvent {
public:
    AbstractEvent(std::type_index type);
    virtual ~AbstractEvent() = default;
    size_t hash() const;
};
```

#### UserInputEvent

```cpp
class UserInputEvent : public AbstractEvent {
public:
    UserInputEvent(InputEvent input_event);
    virtual ~UserInputEvent() = default;
    const InputEvent& inputEvent() const;
};
```

#### RefreshRenderEvent

```cpp
class RefreshRenderEvent : public AbstractEvent {
public:
    RefreshRenderEvent();
    virtual ~RefreshRenderEvent() = default;
};
```

#### ResizeTermEvent

```cpp
class ResizeTermEvent : public AbstractEvent {
public:
    ResizeTermEvent(const Size& old_size, const Size& new_size);
    virtual ~ResizeTermEvent() = default;
    const Size& oldSize() const;
    const Size& newSize() const;
};
```

### 10.3 类型别名

```cpp
using Subscriber = std::function<void(const AbstractEvent&)>;
using SubscriberMap = std::unordered_map<size_t, Subscriber>;
using SubscriberID = size_t;
```

### 10.4 成员函数

#### self

```cpp
static EventBus& self();
```

#### subscribe

```cpp
template <typename T>
SubscriberID subscribe(const Subscriber& subscriber);
template <typename T>
SubscriberID subscribe(Subscriber&& subscriber);
```
- **返回值**: 用于取消订阅的订阅者 ID

#### unsubscribe

```cpp
template <typename T>
void unsubscribe(SubscriberID id);
```

#### publish

```cpp
template <typename T>
void publish(SubscriberID id, AbstractEvent *event, size_t priority = 0);
template <typename T>
void publish(AbstractEvent *event, size_t priority = 0);
```

向特定订阅者（第一个重载）或所有类型为 `T` 的订阅者（第二个重载）发布事件。较高的 `priority` 值先执行。

#### pollEvents / clear

```cpp
void pollEvents();
void clear();
```

---

## 11. Application 类

### 11.1 类简介

TUI 程序的主应用类。拥有事件循环，管理所有注册的 `Object` 实例（控件和布局），并协调渲染。

### 11.2 构造函数

```cpp
explicit Application();
```

默认构造函数。无 `argc` / `argv` — 参数处理由用户负责。

### 11.3 析构函数

```cpp
virtual ~Application() = default;
```

### 11.4 成员函数

#### run

```cpp
int run();
```
- **功能**: 运行应用程序主循环
- **返回值**: 退出码（由 `exit()` 设置）

#### exit

```cpp
void exit(int8_t exit_code = 0);
```
- **功能**: 请求应用程序以指定退出码终止
- **参数**: `exit_code` — 退出状态（默认：`0`）

#### setEnabledExitByKey / isEnabledExitByKey

```cpp
void setEnabledExitByKey(bool enabled);
bool isEnabledExitByKey() const;
```

启用后，按下 `Ctrl+C` 或 `ESC` 将退出主循环。默认启用。

#### setRefreshEnabled / isRefreshEnabled

```cpp
void setRefreshEnabled(bool enabled);
bool isRefreshEnabled() const;
```

控制渲染器是否在每次循环迭代时刷新。

#### Z-order 控制

```cpp
void setZOrder(const Object* object, uint32_t z_order);
void setZOrder(uint32_t dst_order, uint32_t src_order);
void setZOrder(const Object* dst_object, const Object* src_object);
```

| 重载 | 说明 |
|------|------|
| `(object, z_order)` | 将对象移动到指定 Z-order 位置 |
| `(dst_order, src_order)` | 通过索引交换两个 Z-order 位置 |
| `(dst_object, src_object)` | 交换两个对象的 Z-order |

```cpp
uint32_t zOrder() const;
const Object* zOrderOf(uint32_t dst_order) const;
```

- `zOrder()` — 当前总 Z-order 数量
- `zOrderOf(dst_order)` — 给定 Z-order 位置的对象（或 `nullptr`）

#### count

```cpp
uint32_t count() const;
```

注册到应用程序的顶层 `Object` 实例数量。

---

## 12. AbstractLayout 类

### 12.1 类简介

抽象布局基类，用于管理控件排列。继承自 `Object`。

### 12.2 类型别名

```cpp
using WidgetIter = std::vector<AbstractWidget*>::iterator;
using CWidgetIter = std::vector<AbstractWidget*>::const_iterator;
```

### 12.3 构造函数

```cpp
AbstractLayout(const std::string& name, std::type_index type_id, Object* parent = nullptr);
```

| 参数 | 说明 |
|------|------|
| `name` | 布局名称 |
| `type_id` | 具体布局子类的 `std::type_index` |
| `parent` | 父对象（可选） |

### 12.4 析构函数

```cpp
virtual ~AbstractLayout() = default;
```

### 12.5 成员函数

#### move

```cpp
void move(const Position& position);
void move(uint32_t x, uint32_t y);
```

#### resize

```cpp
void resize(const Size& size);
void resize(uint32_t w, uint32_t h);
```

#### setEnabled / setVisible

```cpp
void setEnabled(bool enabled);
void setVisible(bool visible);
```

#### 控件管理

```cpp
bool appendWidget(AbstractWidget* widget);
bool insertWidget(uint64_t index, AbstractWidget* widget);
bool removeWidget(AbstractWidget* widget);
bool removeWidget(uint64_t index);
bool replaceWidget(uint64_t index, AbstractWidget* new_widget);
bool replaceWidget(WidgetIter pos, AbstractWidget* new_widget);
bool swapWidget(uint64_t index_1, uint64_t index_2);
bool swapWidget(AbstractWidget* widget_1, AbstractWidget* widget_2);
void clear();
```

所有控件修改函数成功返回 `true`。`remove`/`replace`/`swap` 同时提供基于指针和基于索引的重载。

#### Position / Size / State 获取器

```cpp
[[nodiscard]] const Position& position() const;
[[nodiscard]] const Size& size() const;
[[nodiscard]] bool enabled() const;
[[nodiscard]] bool visible() const;
```

#### 迭代器

```cpp
[[nodiscard]] WidgetIter begin();
[[nodiscard]] WidgetIter end();
[[nodiscard]] CWidgetIter cbegin() const;
[[nodiscard]] CWidgetIter cend() const;
```

#### count / widget / indexOf

```cpp
[[nodiscard]] size_t count() const;
[[nodiscard]] AbstractWidget* widget(size_t index) const;
[[nodiscard]] uint64_t indexOf(const AbstractWidget* widget) const;
```

### 12.6 受保护虚函数

```cpp
virtual void renderEvent(Renderer& renderer) = 0;
virtual void moveEvent(uint32_t x, uint32_t y) = 0;
virtual void resizeEvent(uint32_t width, uint32_t height) = 0;
```

---

## 13. CurBlock 类

### 13.1 类简介

光标块控件。渲染为实心块，可作为光标指示器。继承自 `AbstractWidget`。

### 13.2 构造函数

```cpp
explicit CurBlock(const std::string& name, Object* parent = nullptr);
```

| 参数 | 说明 |
|------|------|
| `name` | 控件名称 |
| `parent` | 父对象（可选） |

### 13.3 析构函数

```cpp
virtual ~CurBlock() = default;
```

该类故意保持极简 — 渲染行为完全由 `AbstractWidget` 中定义的受保护虚函数处理。

---

## 14. Label 类

### 14.1 类简介

简单文本标签控件。继承自 `AbstractWidget`。

### 14.2 构造函数

```cpp
explicit Label(const std::string& name, const Position& position, Object* parent = nullptr);
explicit Label(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

| 参数 | 说明 |
|------|------|
| `name` | 控件名称 |
| `position` | 控件位置 |
| `size` | 控件大小（第二个构造函数；否则默认自动尺寸） |
| `parent` | 父对象（可选） |

### 14.3 析构函数

```cpp
virtual ~Label() = default;
```

### 14.4 成员函数

#### setText / text

```cpp
void setText(const std::string& text);
[[nodiscard]] const std::string& text() const;
```

#### setAutoSizeEnabled / autoSizeEnabled

```cpp
void setAutoSizeEnabled(bool enabled);
[[nodiscard]] bool autoSizeEnabled() const;
```

启用时（默认，且未提供显式 size 时），标签自动调整大小以适应文本。

#### setAlignment / alignment

```cpp
void setAlignment(Alignment alignment);
[[nodiscard]] Alignment alignment() const;
```

在控件分配区域内的对齐方式。

---

## 15. Button 类

### 15.1 类简介

可点击按钮控件。继承自 `Label`。支持点击回调和可配置的默认激活键。

### 15.2 构造函数

```cpp
Button(const std::string& name, const Position& position, Object* parent = nullptr);
Button(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

### 15.3 析构函数

```cpp
virtual ~Button() = default;
```

### 15.4 成员函数

#### setEvent / unsetEvent

```cpp
void setEvent(const std::function<void(Button&)>& event);
void unsetEvent();
```

设置或清除点击回调。回调接收被点击的 `Button` 的引用。

#### setDefaultKeyEvent

```cpp
void setDefaultKeyEvent(const std::array<KeyEvent, 2>& key_events);
```

设置两个激活按钮的键盘快捷键。

#### setDefaultKeys

```cpp
void setDefaultKeys(uint8_t key1, uint8_t key2 = KEY_NONE,
                    SP_Keys sp_key1 = SP_KEY_NONE, SP_Keys sp_key2 = SP_KEY_NONE);
```

便捷重载 — 直接指定按键码和特殊键，无需构造 `KeyEvent` 对象。

---

## 16. LineEdit 类

### 16.1 类简介

单行文本输入控件。继承自 `AbstractWidget`。

### 16.2 EchoMode 枚举

```cpp
enum class EchoMode : uint8_t {
    NoEcho,
    Normal,
    Password
};
```

### 16.3 构造函数

```cpp
explicit LineEdit(const std::string& name, const Position& position, uint32_t width, Object* parent = nullptr);
```

### 16.4 析构函数

```cpp
virtual ~LineEdit() = default;
```

### 16.5 成员函数

#### 文本操作

```cpp
void setText(const std::string& text);
void setText(const char* text);
void appendText(const char* text);
void appendText(const std::string& text);
void clear();
```

#### 长度约束

```cpp
void setMinimumLength(uint16_t size);
void setMaximumLength(uint16_t size);
[[nodiscard]] uint16_t minimumLength() const;
[[nodiscard]] uint16_t maximumLength() const;
```

#### 占位符

```cpp
void setPlaceHolderText(const std::string& text);
void setPlaceHolderText(const char* text);
```

#### 回显模式

```cpp
void setEchoMode(EchoMode mode);
[[nodiscard]] EchoMode echoMode() const;

void setEchoPassChar(const Char& ch);
[[nodiscard]] const Char& echoPassChar() const;
```

设置 `EchoMode::Password` 时，`echoPassChar`（默认 `*`）替换每个字符用于显示。

#### 文本对齐

```cpp
void setTextAlignment(TextAlignment alignment);
[[nodiscard]] TextAlignment textAlignment() const;
```

#### text

```cpp
[[nodiscard]] const std::string& text() const;
```

当前输入值。

---

## 17. Slider 类

### 17.1 类简介

交互式滑块控件。允许用户拖动或步进数值范围。继承自 `AbstractWidget`。

### 17.2 构造函数

```cpp
explicit Slider(const std::string& name, const Position& position, uint8_t width, Object* parent = nullptr);
```

`width` 是显示列数（或行数，取决于方向）。

### 17.3 析构函数

```cpp
virtual ~Slider() = default;
```

### 17.4 成员函数

#### 方向 / 宽度

```cpp
void setOrientation(Orientation mode);
[[nodiscard]] Orientation orientation() const;

void setWidth(uint8_t width);
[[nodiscard]] uint8_t width() const;
```

#### 范围 / 值

```cpp
void setMinimumValue(int value);
void setMaximumValue(int value);
void setValue(int value);
void appendValue(int value);

[[nodiscard]] int minimumValue() const;
[[nodiscard]] int maximumValue() const;
[[nodiscard]] int value() const;
```

#### 步长

```cpp
void setSingleStep(int value);
void setPageStep(int value);

[[nodiscard]] int singleStep() const;
[[nodiscard]] int pageStep() const;
```

#### 反转

```cpp
void setInvertedEnabled(bool enable);
[[nodiscard]] bool invertedEnabled() const;
```

启用时，滑块从相反方向填充。

#### 事件回调

```cpp
void setEvent(const std::function<void(int)>& event);
void unsetEvent();
```

值改变时回调接收当前整数值。

#### 填充颜色

```cpp
void setFilledColor(const Color& fg_color, const Color& bg_color);
[[nodiscard]] Color fgFilledColor() const;
[[nodiscard]] Color bgFilledColor() const;
```

---

## 18. ProgressBar 类

### 18.1 类简介

非交互式进度指示器。继承自 `AbstractWidget`。与 `Slider` 不同，它不响应用户输入。

### 18.2 构造函数

```cpp
explicit ProgressBar(const std::string& name, const Position& position, uint32_t width,
                     Object* parent = nullptr);
```

### 18.3 析构函数

```cpp
virtual ~ProgressBar() = default;
```

### 18.4 成员函数

#### 方向 / 宽度

```cpp
void setOrientation(Orientation mode);
[[nodiscard]] Orientation orientation() const;

void setWidth(uint8_t width);
[[nodiscard]] uint8_t width() const;
```

#### 值

```cpp
void setValue(int value);
void appendValue(int value);
[[nodiscard]] int value() const;
```

#### 反转

```cpp
void setInvertedEnabled(bool enable);
[[nodiscard]] bool invertedEnabled() const;
```

#### 填充颜色

```cpp
void setFilledColor(const Color& fg_color, const Color& bg_color);
[[nodiscard]] Color fgFilledColor() const;
[[nodiscard]] Color bgFilledColor() const;
```

---

## 19. ListView 类

### 19.1 类简介

可滚动列表控件。显示一组字符串，同时选中一项。继承自 `AbstractWidget`。

### 19.2 构造函数

```cpp
explicit ListView(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

### 19.3 析构函数

```cpp
virtual ~ListView() = default;
```

### 19.4 成员函数

#### 项目操作

```cpp
void appendItem(const std::string& text);
void appendItems(const std::vector<std::string>& items);
void insertItem(int32_t index, const std::string& text);
void popItem();
void removeItems(int32_t index, int32_t count = 1);
void clear();
void setItem(int32_t index, const std::string& new_text);
void swapItems(int32_t index1, int32_t index2);
```

#### 选择

```cpp
void setCurrentIndex(int32_t index);
[[nodiscard]] int32_t currentIndex() const;
```

#### 查询

```cpp
[[nodiscard]] int32_t count() const;
[[nodiscard]] std::string currentItem() const;
[[nodiscard]] std::string itemAt(int32_t index) const;
```

#### 颜色

```cpp
void setSelectionColor(const Color& fg_color, const Color& bg_color);
void setActiveColor(const Color& fg_color, const Color& bg_color);

[[nodiscard]] Color bgSelectionColor() const;
[[nodiscard]] Color fgSelectionColor() const;
[[nodiscard]] Color bgActiveColor() const;
[[nodiscard]] Color fgActiveColor() const;
```

---

## 20. 使用示例

### 20.1 基本终端控制

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;

    Terminal::enterRawMode();
    Terminal::clearScreen();

    Terminal::setForegroundColor(Color::Green);
    Terminal::printLine("Hello, TUI!");
    Terminal::reset();

    Terminal::printFormat("Screen size: {}x{}\n",
        Terminal::screenSize().width,
        Terminal::screenSize().height);

    Terminal::moveCursor(5, 10);
    Terminal::print("Position (5, 10)");

    Terminal::printLine("\nPress any key...");
    SP_Keys sp_key;
    uint8_t key = Terminal::getKey(&sp_key);

    Terminal::printFormat("Key: {} ({})",
        getKeyName(key, sp_key),
        (int)key);

    Terminal::leaveRawMode();
    return 0;
}
```

### 20.2 Slider 控件

```cpp
#include "TUI/TUI.hpp"

int main() {
    using namespace Tiny::TUI;

    Application app;

    auto* slider = new Slider("volume", {2, 2}, 40);
    slider->setMinimumValue(0);
    slider->setMaximumValue(100);
    slider->setValue(50);
    slider->setEvent([](int v) {
        Terminal::printError("Value changed: {}\n", v);
    });

    return app.run();
}
```

### 20.3 Button 与快捷键

```cpp
using namespace Tiny::TUI;

auto* ok_btn = new Button("ok", {4, 4}, Size{10, 1});
ok_btn->setText(" OK ");
ok_btn->setDefaultKeys(KEY_CR, KEY_NONE, SP_KEY_NONE, SP_KEY_NONE);
ok_btn->setEvent([](Button& b) {
    Terminal::printError("Clicked: {}\n", b.objectName());
});
```

### 20.4 LineEdit 密码模式

```cpp
using namespace Tiny::TUI;

auto* password = new LineEdit("pwd", {6, 4}, 30);
password->setEchoMode(LineEdit::EchoMode::Password);
password->setPlaceHolderText("Enter password");
password->setMaximumLength(32);
```

---

## 21. 注意事项

### 21.1 原始模式

- 进入原始模式后，终端不会自动处理输入输出
- 必须手动处理回车、退格等按键
- 程序退出前必须调用 `leaveRawMode()`
- 建议使用 RAII 模式确保恢复终端状态

### 21.2 终端兼容性

- 需要支持 ANSI 转义序列的终端
- Windows 10+、现代 Linux 终端、macOS Terminal 均支持
- Windows 7/8 可能需要启用虚拟终端处理

### 21.3 渲染器使用

- 使用双缓冲机制，先绘制到缓冲区
- 调用 `present()` 才实际输出到屏幕
- 调用 `clear()` 清空前缓冲区以便重新绘制

### 21.4 `Application` 与直接使用 `Renderer`

- 基于控件的 UI 应使用 `Application::run()` — 它处理主循环、输入分发和渲染
- 简单的非控件程序或自定义渲染器适合直接使用 `Renderer::self()`

### 21.5 Object 层级

```
Object                          ← name, parent/children, type info
├── AbstractWidget              ← position, size, styles, checkable, mouse tracing
│   ├── CurBlock
│   ├── Label
│   │   └── Button
│   ├── LineEdit
│   ├── Slider
│   ├── ProgressBar
│   └── ListView
└── AbstractLayout              ← manages a list of AbstractWidget children
```

### 21.6 已弃用符号

| 符号 | 替代方案 | 移除版本 |
|------|----------|----------|
| `Renderer::Style` typedef | `Tiny::TUI::Style` | v0.3.0 |
| `Renderer::Corner` typedef | `Tiny::TUI::Corner` | v0.3.0 |
| `Tiny::Code` 命名空间 | `Tiny::U8Code` | v0.3.0 |
| `AbstractWidget::draw()` | 受保护的 `callDrawEvent()` | v0.3.0 |
| `Renderer::setResizeEvent()` | `EventBus::subscribe<ResizeTermEvent>()` | v1.4.0 |

---

## 22. 如何在 Linux 控制台下使用 GPM 库

见文章 [GPM_In_Linux.md](GPM_In_Linux.md)，介绍了如何在 Linux 无桌面环境下使用 GPM 库以解决 TTY 模式下的鼠标事件处理问题。
