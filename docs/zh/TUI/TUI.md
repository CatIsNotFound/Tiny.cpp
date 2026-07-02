# TUI 模块

命名空间: `Tiny::TUI`

---

## 目录

1. [模块简介](#1-模块简介)
2. [头文件](#2-头文件)
3. [辅助函数](#3-辅助函数)
4. [数据结构](#4-数据结构)
5. [Terminal 类](#5-terminal-类)
6. [Renderer 类](#6-renderer-类)
7. [AbstractWidget 类](#7-abstractwidget-类)
8. [使用示例](#8-使用示例)
9. [注意事项](#9-注意事项)
10. [如何在 Linux 控制台下使用 GPM 库](#10-如何在-Linux-控制台下使用-GPM-库)

---

## 1. 模块简介

`TUI` 模块提供终端用户界面功能，包括：

- **终端控制**: 原始模式切换、屏幕控制、光标操作
- **输入处理**: 按键读取、鼠标事件
- **颜色样式**: 前景色、背景色、粗体、下划线等
- **双缓冲渲染**: 高效的屏幕渲染
- **控件基类**: 可扩展的控件系统

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/TUI/TUI.hpp>
// 直接复制源代码方式
#include "TUI/TUI.hpp"
```

---

## 3. 辅助函数

### 3.1 splitFront

```cpp
std::string splitFront(const char* data);
```
- **功能**: 提取 UTF-8 字符串的第一个字符
- **参数**: `data` - UTF-8 字符串
- **返回值**: 第一个字符（可能多字节）

### 3.2 splitUTF8

```cpp
std::vector<std::string> splitUTF8(const char* data);
```
- **功能**: 将 UTF-8 字符串分割为字符数组
- **参数**: `data` - UTF-8 字符串
- **返回值**: 字符数组

### 3.3 getKeyName

```cpp
const char* getKeyName(const uint8_t& KEY, const SP_Keys& SP);
```
- **功能**: 获取按键名称
- **参数**: 
  - `KEY` - 按键码
  - `SP` - 特殊键类型
- **返回值**: 按键名称字符串

### 3.4 getMouseName

```cpp
const char* getMouseName(const SP_Mouse& SP);
```
- **功能**: 获取鼠标事件名称
- **参数**: `SP` - 鼠标事件类型
- **返回值**: 事件名称字符串

### 3.5 comparePosition

```cpp
int8_t comparePosition(const Position& pos1, const Position& pos2);
```
- **功能**: 比较两个位置
- **参数**: 
  - `pos1` - 第一个位置
  - `pos2` - 第二个位置
- **返回值**: 
  - `-1` - pos1 在 pos2 之前
  - `0` - 位置相同
  - `1` - pos1 在 pos2 之后

### 3.6 compareSize

```cpp
int8_t compareSize(const Size& size1, const Size& size2);
```
- **功能**: 比较两个尺寸
- **参数**: 
  - `size1` - 第一个尺寸
  - `size2` - 第二个尺寸
- **返回值**: 
  - `-1` - size1 小于 size2
  - `0` - 尺寸相同
  - `1` - size1 大于 size2

### 3.7 isPointInRect

```cpp
bool isPointInRect(const Position& point, Position& start_pos, Position& end_pos);
```
- **功能**: 检查点是否在矩形内
- **参数**: 
  - `point` - 要检查的点
  - `start_pos` - 矩形左上角
  - `end_pos` - 矩形右下角
- **返回值**: `true` 表示点在矩形内

---

## 4. 数据结构

### 4.1 Size 结构体

```cpp
struct Size {
    uint32_t width;   // 宽度（列数）
    uint32_t height;  // 高度（行数）
};
```

### 4.2 Position 结构体

```cpp
struct Position {
    uint32_t row;     // 行号（0-based）
    uint32_t column;  // 列号（0-based）
};
```

### 4.3 Color 枚举

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

### 4.4 Keys 枚举

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
    KEY_BACKSPACE   = 8,
    KEY_TAB         = 9,
    KEY_LF          = 10,
    KEY_VT          = 11,
    KEY_FF          = 12,
    KEY_CR          = 13,
    KEY_ENTER       = 13,
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

**控制键**:
- `KEY_CTRL_A` 到 `KEY_CTRL_Z`: 对应 Ctrl+A 到 Ctrl+Z

**按键别名**:
- `KEY_BK` = `KEY_BACKSPACE` = `8`
- `KEY_CR` = `KEY_ENTER` = `13`
- `KEY_DEL` = `127`

### 4.5 SP_Keys 枚举（特殊键）

```cpp
enum SP_Keys : uint8_t {
    SP_KEY_UNKNOWN,
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

### 4.6 SP_Mouse 枚举（鼠标事件）

```cpp
enum SP_Mouse : uint8_t {
    SP_MOUSE_UNKNOWN,
    SP_MOUSE_LEFT_BUTTON,    // 左键
    SP_MOUSE_MIDDLE_BUTTON,  // 中键
    SP_MOUSE_RIGHT_BUTTON,   // 右键
    SP_MOUSE_WHEEL_UP,       // 滚轮上
    SP_MOUSE_WHEEL_DOWN,     // 滚轮下
    SP_MOUSE_MOVED,          // 鼠标移动
    SP_MOUSE_RELEASE         // 鼠标释放
};
```

### 4.7 InputEvent 结构体

```cpp
struct InputEvent {
    enum Type : uint8_t {
        None,
        Keyboard,
        Mouse
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
| `type` | `Type` | 事件类型：None、Keyboard 或 Mouse |
| `input.keyboard.key` | `uint8_t` | 按键码 |
| `input.keyboard.sp_key` | `SP_Keys` | 特殊键类型 |
| `input.keyboard.is_pressed` | `bool` | 按键是否按下（仅 Windows） |
| `input.mouse.position` | `Position` | 鼠标位置 |
| `input.mouse.button` | `SP_Mouse` | 鼠标按钮/事件 |
| `input.mouse.is_pressed` | `bool` | 鼠标按钮是否按下 |

### 4.8 类型别名

```cpp
using KeyEvent = InputEvent::Input::Keyboard;
using MouseEvent = InputEvent::Input::Mouse;
```

### 4.9 RGBColor 结构体

```cpp
struct RGBColor {
    uint8_t r, g, b;

    RGBColor();
    RGBColor(uint8_t r, uint8_t g, uint8_t b);
    bool operator==(const RGBColor& other) const;
    bool operator!=(const RGBColor& other) const;
};
```

| 成员 | 类型 | 说明 |
|------|------|------|
| `r` | `uint8_t` | 红色分量（0-255） |
| `g` | `uint8_t` | 绿色分量（0-255） |
| `b` | `uint8_t` | 蓝色分量（0-255） |

---

## 5. Terminal 类

### 5.1 类简介

终端控制类，提供原始模式切换、屏幕控制、光标操作、颜色设置、输入读取等功能。

所有成员函数均为静态函数。

### 5.2 原始模式控制

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

### 5.3 屏幕信息

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

### 5.4 输出函数

#### print

```cpp
static bool print(char ch);
static bool print(const std::string& text);
```
- **功能**: 输出单个字符或文本（不换行）
- **参数**: 
  - `ch` - 单个字符
  - `text` - 要输出的文本
- **返回值**: `true` 表示成功

#### printLine

```cpp
static bool printLine(const std::string& text = {});
```
- **功能**: 输出文本并换行
- **参数**: `text` - 要输出的文本（可选，默认为空行）
- **返回值**: `true` 表示成功

#### printFormat

```cpp
template<typename ... Args>
static bool printFormat(const char* format, Args... args);
```
- **功能**: 格式化输出（类似 Python f-string）
- **参数**: 
  - `format` - 格式字符串，使用 `{}` 作为占位符
  - `args` - 可变参数
- **返回值**: `true` 表示成功
- **示例**: `printFormat("Hello, {}!", "World")`

#### formatString

```cpp
template<typename ... Args>
static std::string formatString(const char* format, Args... args);
```
- **功能**: 格式化字符串
- **参数**: 同 `printFormat`
- **返回值**: 格式化后的字符串

### 5.5 屏幕控制

#### clearScreen

```cpp
static bool clearScreen();
```
- **功能**: 清屏
- **返回值**: `true` 表示成功

#### clearInRow

```cpp
static bool clearInRow(uint8_t row);
```
- **功能**: 清除指定行
- **参数**: `row` - 行号（0-based）
- **返回值**: `true` 表示成功

#### moveCursor

```cpp
static bool moveCursor(Position position);
static bool moveCursor(uint32_t row, uint32_t column);
```
- **功能**: 移动光标到指定位置
- **参数**: 目标位置
- **返回值**: `true` 表示成功

#### setScrollRegion

```cpp
static bool setScrollRegion(uint32_t row_start, uint32_t row_end);
```
- **功能**: 设置滚动区域
- **参数**: 
  - `row_start` - 起始行
  - `row_end` - 结束行
- **返回值**: `true` 表示成功

#### resetScrollRegion

```cpp
static bool resetScrollRegion();
```
- **功能**: 重置滚动区域为全屏
- **返回值**: `true` 表示成功

#### flushScreen

```cpp
static bool flushScreen();
```
- **功能**: 刷新屏幕输出
- **返回值**: `true` 表示成功

### 5.6 输入函数

#### readLine

```cpp
static std::string readLine();
```
- **功能**: 读取一行输入
- **返回值**: 输入的字符串（不含换行符）

#### readLineW

```cpp
static std::wstring readLineW();
```
- **功能**: 读取一行宽字符输入
- **返回值**: 宽字符串

#### getKey

```cpp
static uint8_t getKey(SP_Keys* sp_key = nullptr);
```
- **功能**: 读取按键
- **参数**: 
  - `sp_key` - 输出特殊键类型（可选）
- **返回值**: 按键码

### 5.7 鼠标控制

#### setMouseEnabled

```cpp
static bool setMouseEnabled(bool enabled);
```
- **功能**: 启用/禁用鼠标事件
- **参数**: `enabled` - 是否启用
- **返回值**: `true` 表示成功

#### getMouseButton

```cpp
static uint8_t getMouseButton(Position* mouse_pos = nullptr, bool* is_pressed = nullptr);
```
- **功能**: 获取鼠标事件
- **参数**: 
  - `mouse_pos` - 输出鼠标位置（可选）
  - `is_pressed` - 输出是否按下（可选）
- **返回值**: 鼠标事件码

#### getInput

```cpp
static InputEvent getInput();
```
- **功能**: 获取统一的输入事件（键盘或鼠标）
- **返回值**: `InputEvent` 结构体，包含事件类型和数据

### 5.8 颜色与样式函数

#### 颜色设置

```cpp
static void setBackgroundColor(Color color, bool intensity = true);
static void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
static void setForegroundColor(Color color, bool intensity = false);
static void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
```

| 函数 | 功能 | 参数 |
|------|------|------|
| `setBackgroundColor(Color, bool)` | 设置背景色（ANSI 16色） | `color` - 颜色，`intensity` - 是否高亮 |
| `setBackgroundColor(uint8_t, uint8_t, uint8_t)` | 设置背景色（RGB） | `r, g, b` - 红绿蓝分量（0-255） |
| `setForegroundColor(Color, bool)` | 设置前景色（ANSI 16色） | `color` - 颜色，`intensity` - 是否高亮 |
| `setForegroundColor(uint8_t, uint8_t, uint8_t)` | 设置前景色（RGB） | `r, g, b` - 红绿蓝分量（0-255） |

#### 样式设置

```cpp
static void setBolder(bool enable);        // 粗体
static void setDark(bool enable);          // 暗色
static void setItalic(bool enable);        // 斜体
static void setUnderline(bool enable);     // 下划线
static void setBlinking(bool enable);      // 闪烁
static void setReverseColor(bool enable);  // 反色
static void setCursorVisible(bool enable); // 光标可见性
static void setStrikethrough(bool enable); // 删除线
static void reset();                       // 重置所有样式
```

---

## 6. Renderer 类

### 6.1 类简介

双缓冲终端渲染器，支持字符绘制、矩形填充、边框绘制等功能。

### 6.2 嵌套结构体

#### Style

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
        Bolder            = 1,    // 粗体
        Dark              = 2,    // 暗色
        Italic            = 4,    // 斜体
        Underline         = 8,    // 下划线
        Blinking          = 16,   // 闪烁
        Reverse           = 32,   // 反色
        Strikethrough     = 64,   // 删除线
    };

    Style();
    void reset();
    bool isDefault() const;
    bool operator==(const Style& other) const;
    bool operator!=(const Style& other) const;
};
```

| 成员 | 类型 | 说明 |
|------|------|------|
| `property` | `uint8_t` | 样式属性位掩码 |
| `bg_color` | `Color` | ANSI 背景色（默认值：`Color::Black`） |
| `fg_color` | `Color` | ANSI 前景色（默认值：`Color::Default`） |
| `intensity` | `uint8_t` | 颜色强度：0=无, 1=仅背景, 2=仅前景, 3=全部（默认值：2） |
| `used_rgb_color` | `bool` | 是否使用 RGB 颜色（true 时忽略 ANSI 颜色） |
| `bg_rgb_color` | `RGBColor` | RGB 背景色（r, g, b 各 0-255） |
| `fg_rgb_color` | `RGBColor` | RGB 前景色（r, g, b 各 0-255） |

**Property 枚举**:
- 使用位运算组合多个属性：`Style::Bolder | Style::Underline`
- 或使用 `property` 字段直接设置

#### Cell

```cpp
struct Cell {
    Char data;        // 字符数据
    bool is_dirty;    // 是否已修改
    Style style;      // 样式

    Cell();
    void reset();
    void set(const char* ch, Style st);
};
```

| 成员 | 类型 | 说明 |
|------|------|------|
| `data` | `Char` | 字符数据（支持多字节 UTF-8） |
| `is_dirty` | `bool` | 该单元格是否已被修改 |
| `style` | `Style` | 单元格样式 |

**Char 类**:

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

| 成员 | 类型 | 说明 |
|------|------|------|
| `data` | `std::string` | 字符字符串数据 |
| `length` | `uint8_t` | 字符数据的字节长度 |

#### Corner

```cpp
struct Corner {
    Char left_top;     // 左上角
    Char left;         // 左边
    Char left_bottom;  // 左下角
    Char right_top;    // 右上角
    Char right;        // 右边
    Char right_bottom; // 右下角
    Char top;          // 上边
    Char bottom;       // 下边
};
```

### 6.3 构造函数与静态成员函数

#### self

```cpp
static Renderer& self();
```
- **功能**: 获取渲染器单例
- **返回值**: 渲染器引用

#### 析构函数

```cpp
virtual ~Renderer();
```
- **功能**: 释放渲染器资源并停止后台尺寸监控线程

### 6.4 成员函数

#### set (重载)

```cpp
void set(const Position& pos, uint8_t ch, Style style = {});
void set(uint32_t x, uint32_t y, uint8_t ch, Style style = {});
void set(const Position& pos, const std::string& str, Style style = {});
void set(uint32_t x, uint32_t y, const std::string& str, Style style = {});
```
- **功能**: 在指定位置设置字符
- **参数**: 
  - `pos` / `x, y` - 位置
  - `ch` / `str` - 字符或字符串
  - `style` - 样式（可选）

#### setStrF

```cpp
template<typename ... Args>
void setStrF(const Position& pos, const char* format, Args... args);
```
- **功能**: 格式化设置字符串
- **参数**: 
  - `pos` - 位置
  - `format` - 格式字符串
  - `args` - 可变参数

#### setSSF

```cpp
template<typename ... Args>
void setSSF(const Position& pos, const char* format, const Style& style, Args... args);
```
- **功能**: 带样式的格式化设置
- **参数**: 
  - `pos` - 位置
  - `format` - 格式字符串
  - `style` - 样式
  - `args` - 可变参数

#### fillScreen

```cpp
void fillScreen(const Style& style = {});
```
- **功能**: 使用指定样式填充整个屏幕
- **参数**: `style` - 填充样式

#### fillRows

```cpp
void fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch = ' ', Style style = {});
void fillRows(uint32_t start_row, uint32_t end_row, const std::string& ch, Style style = {});
```
- **功能**: 填充指定行范围
- **参数**: 
  - `start_row` - 起始行
  - `end_row` - 结束行
  - `ch` / `str` - 填充字符或字符串
  - `style` - 样式

#### fillCols

```cpp
void fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch = ' ', Style style = {});
void fillCols(uint32_t start_col, uint32_t end_col, const std::string& ch, Style style = {});
```
- **功能**: 填充指定列范围
- **参数**: 
  - `start_col` - 起始列
  - `end_col` - 结束列
  - `ch` / `str` - 填充字符或字符串
  - `style` - 样式

#### fillRect

```cpp
void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch = ' ', Style style = {});
void fillRect(const Position& start_pos, const Position& end_pos, const std::string& str, Style style = {});
```
- **功能**: 填充矩形区域
- **参数**: 
  - `start_pos` - 起始位置
  - `end_pos` - 结束位置
  - `ch` / `str` - 填充字符或字符串
  - `style` - 样式

#### drawBorder

```cpp
void drawBorder(const Position& start_pos, const Position& end_pos, Corner corner, Style style = {});
```
- **功能**: 绘制边框
- **参数**: 
  - `start_pos` - 左上角位置
  - `end_pos` - 右下角位置
  - `corner` - 边角字符定义
  - `style` - 样式

#### unset

```cpp
void unset(const Position& pos);
void unset(uint32_t x, uint32_t y);
```
- **功能**: 清除指定位置
- **参数**: 目标位置

#### unsetRow

```cpp
void unsetRow(uint32_t row);
```
- **功能**: 清除整行
- **参数**: `row` - 行号

#### unsetCol

```cpp
void unsetCol(uint32_t col);
```
- **功能**: 清除整列
- **参数**: `col` - 列号

#### unsetRect

```cpp
void unsetRect(const Position& start_pos, const Position& end_pos);
```
- **功能**: 清除矩形区域
- **参数**: 
  - `start_pos` - 起始位置
  - `end_pos` - 结束位置

#### setResizeEvent

```cpp
void setResizeEvent(const std::function<void(Renderer&)>& event);
```
- **功能**: 设置终端尺寸变化事件回调
- **参数**: `event` - 回调函数，接收 Renderer 引用
- **说明**: 当终端窗口大小变化时触发

#### clear

```cpp
void clear();
```
- **功能**: 清空前缓冲区

#### present

```cpp
void present();
```
- **功能**: 将前缓冲区内容呈现到屏幕

### 6.5 受保护虚函数

#### renderEvent

```cpp
virtual void renderEvent();
```
- **功能**: 内部渲染事件处理函数，渲染器需要重绘时调用
- **说明**: 子类可重写以自定义渲染行为

#### resizeEvent

```cpp
virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
```
- **功能**: 内部尺寸变化事件处理函数，终端尺寸变化时调用
- **参数**: 
  - `use_default_size` - 是否使用当前终端尺寸（默认值：`true`）
  - `size` - 当 `use_default_size` 为 `false` 时使用的自定义尺寸
- **说明**: 子类可重写以自定义尺寸变化行为

---

## 7. AbstractWidget 类

### 7.1 类简介

抽象控件基类，所有 TUI 控件的基类。

### 7.2 构造函数

```cpp
explicit AbstractWidget(const std::string& name, const Position& position, const Size& size);
```
- **参数**: 
  - `name` - 控件名称
  - `position` - 位置
  - `size` - 大小

### 7.3 析构函数

```cpp
virtual ~AbstractWidget();
```

### 7.4 成员函数

#### rename

```cpp
void rename(const std::string& name);
```
- **功能**: 重命名控件
- **参数**: `name` - 新名称

#### move

```cpp
void move(const Position& position);
void move(uint32_t x, uint32_t y);
```
- **功能**: 移动控件位置
- **参数**: 新位置

#### resize

```cpp
void resize(const Size& size);
void resize(uint32_t w, uint32_t h);
```
- **功能**: 调整控件大小
- **参数**: 新大小

#### name

```cpp
[[nodiscard]] const std::string& name() const;
```
- **功能**: 获取控件名称
- **返回值**: 名称引用

#### position

```cpp
[[nodiscard]] const Position& position() const;
```
- **功能**: 获取控件位置
- **返回值**: 位置引用

#### size

```cpp
[[nodiscard]] const Size& size() const;
```
- **功能**: 获取控件大小
- **返回值**: 大小引用

### 7.5 受保护虚函数

```cpp
virtual void renderEvent();
```
- **功能**: 渲染事件处理，子类可重写以自定义控件渲染
- **说明**: 基类提供默认空实现

---

## 8. 使用示例

### 8.1 基本终端控制

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // 进入原始模式
    Terminal::enterRawMode();
    
    // 清屏
    Terminal::clearScreen();
    
    // 设置颜色并输出
    Terminal::setForegroundColor(Color::Green);
    Terminal::printLine("Hello, TUI!");
    Terminal::reset();
    
    // 格式化输出
    Terminal::printFormat("Screen size: {}x{}\n", 
        Terminal::screenSize().width,
        Terminal::screenSize().height);
    
    // 移动光标
    Terminal::moveCursor(5, 10);
    Terminal::print("Position (5, 10)");
    
    // 读取按键
    Terminal::printLine("\nPress any key...");
    SP_Keys sp_key;
    uint8_t key = Terminal::getKey(&sp_key);
    
    Terminal::printFormat("Key: {} ({})", 
        getKeyName(key, sp_key),
        (int)key);
    
    // 退出原始模式
    Terminal::leaveRawMode();
    
    return 0;
}
```

### 8.2 渲染器使用

```cpp
#include "TUI/TUI.hpp"

int main() {
    using namespace Tiny::TUI;
    
    // 获取渲染器实例
    auto& renderer = Renderer::self();
    
    // 设置字符
    renderer.set(0, 0, 'H');
    renderer.set(1, 0, "Hello");
    
    // 设置样式
    Renderer::Style style;
    style.fg_color = Color::Green;
    renderer.set(2, 0, "Green Text", style);
    
    // 绘制边框
    Renderer::Corner corner;
    corner.left_top = "+";
    corner.left = "|";
    corner.top = "-";
    corner.right_top = "+";
    corner.right = "|";
    corner.right_bottom = "+";
    corner.left_bottom = "+";
    corner.bottom = "-";
    renderer.drawBorder({5, 5}, {15, 25}, corner);
    
    // 填充矩形
    renderer.fillRect({6, 6}, {14, 24}, ' ');
    
    // 格式化输出
    renderer.setStrF({7, 7}, "Count: {}", 42);
    
    // 呈现到屏幕
    renderer.present();
    
    // 等待输入
    Terminal::getKey();
    
    return 0;
}
```

### 8.3 鼠标事件处理

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    Terminal::enterRawMode();
    Terminal::setMouseEnabled(true);
    Terminal::clearScreen();
    
    Terminal::printLine("Click anywhere or press 'q' to quit...");
    
    while (true) {
        uint8_t key;
        SP_Keys sp_key;
        Terminal::getKey(key, sp_key);
        
        if (key == 'q' || key == 'Q') {
            break;
        }
        
        if (key == KEY_SPECIAL) {
            Position pos;
            bool pressed;
            uint8_t mouse_btn = Terminal::getMouseButton(&pos, &pressed);
            
            Terminal::moveCursor(2, 0);
            Terminal::clearInRow(2);
            Terminal::printFormat("Mouse: {} at ({}, {}), Pressed: {}",
                getMouseName(static_cast<SP_Mouse>(mouse_btn)),
                pos.row, pos.column, pressed);
        }
    }
    
    Terminal::setMouseEnabled(false);
    Terminal::leaveRawMode();
    
    return 0;
}
```

### 8.4 颜色样式示例

```cpp
#include "TUI/TUI.hpp"

int main() {
    using namespace Tiny::TUI;
    
    Terminal::enterRawMode();
    Terminal::clearScreen();
    
    // ANSI 16 色
    Terminal::setForegroundColor(Color::Red);
    Terminal::printLine("Red Text");
    
    Terminal::setForegroundColor(Color::Blue, true);  // 高亮
    Terminal::printLine("Bright Blue Text");
    
    // RGB 颜色
    Terminal::setForegroundColor(255, 128, 0);  // 橙色
    Terminal::setBackgroundColor(0, 0, 128);    // 深蓝背景
    Terminal::printLine("Orange on Dark Blue");
    
    Terminal::reset();
    
    // 样式组合
    Terminal::setBolder(true);
    Terminal::setUnderline(true);
    Terminal::printLine("Bold and Underlined");
    
    Terminal::reset();
    Terminal::getKey();
    Terminal::leaveRawMode();
    
    return 0;
}
```

---

## 9. 注意事项

### 9.1 原始模式

- 进入原始模式后，终端不会自动处理输入输出
- 必须手动处理回车、退格等按键
- 程序退出前必须调用 `leaveRawMode()`
- 建议使用 RAII 模式确保恢复终端状态

### 9.2 终端兼容性

- 需要支持 ANSI 转义序列的终端
- Windows 10+、现代 Linux 终端、macOS Terminal 均支持
- Windows 7/8 可能需要启用虚拟终端处理

### 9.3 鼠标支持

- 需要终端支持鼠标事件
- 启用后鼠标事件通过 `getKey()` 返回 `KEY_SPECIAL`
- 然后调用 `getMouseButton()` 获取详细信息

### 9.4 渲染器使用

- 使用双缓冲机制，先绘制到缓冲区
- 调用 `present()` 才实际输出到屏幕
- 调用 `clear()` 清空前缓冲区以便重新绘制
- 终端尺寸变化由 `resizeEvent()` 内部处理；可使用 `setResizeEvent()` 注册自定义回调

### 9.5 UTF-8 支持

- 支持多字节字符显示
- 使用 `splitUTF8()` 处理字符串
- 注意中文字符等宽字符的宽度计算

### 9.6 性能考虑

- 避免频繁调用 `present()`
- 批量绘制后统一呈现
- 使用脏标记减少不必要的重绘

## 10. 如何在 Linux 控制台下使用 GPM 库

见文章 [GPM_In_Linux.md](GPM_In_Linux.md)，介绍了如何在 Linux 无桌面环境下使用 GPM 库以解决 TTY 模式下的鼠标事件处理问题。
