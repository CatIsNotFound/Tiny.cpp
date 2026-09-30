# TUI Module

Namespace: `Tiny::TUI`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [U8Code Namespace](#3-u8code-namespace)
4. [Helper Functions](#4-helper-functions)
5. [Data Structures](#5-data-structures)
6. [Terminal Class](#6-terminal-class)
7. [Renderer Class](#7-renderer-class)
8. [Object Class](#8-object-class)
9. [AbstractWidget Class](#9-abstractwidget-class)
10. [EventBus Class](#10-eventbus-class)
11. [Application Class](#11-application-class)
12. [AbstractLayout Class](#12-abstractlayout-class)
13. [CurBlock Class](#13-curblock-class)
14. [Label Class](#14-label-class)
15. [Button Class](#15-button-class)
16. [LineEdit Class](#16-lineedit-class)
17. [Slider Class](#17-slider-class)
18. [ProgressBar Class](#18-progressbar-class)
19. [ListView Class](#19-listview-class)
20. [Usage Examples](#20-usage-examples)
21. [Notes](#21-notes)

---

## 1. Module Overview

The `TUI` module provides terminal user interface functionality, including:

- **Terminal Control**: Raw mode switching, screen control, cursor operations
- **Input Handling**: Key reading, mouse events
- **Color & Style**: Foreground/background colors, bold, underline, etc.
- **Double Buffering**: Efficient screen rendering
- **Object Hierarchy**: `Object` base class providing parent-child relationships
- **Widget System**: Extensible widget hierarchy with `AbstractWidget` as the base
- **Event Bus**: Decoupled event subscription and dispatch via `EventBus`
- **Application Framework**: Top-level `Application` managing the event loop and widget rendering

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/TUI/TUI.hpp>
// Direct source copy method
#include "TUI/TUI.hpp"
```

---

## 3. U8Code Namespace

The `Tiny::U8Code` namespace provides UTF-8 utility functions. It is defined in `Terminal.hpp`.

> **Deprecated alias**: `Tiny::Code` is an alias of `Tiny::U8Code`. Use `Tiny::U8Code`; `Tiny::Code` will be removed since v0.3.0.

### 3.1 Wide String Conversion

```cpp
// Windows versions (with codepage parameter, default 65001 = UTF-8)
std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001);
std::string wide2String(const std::wstring& str, uint32_t codepage = 65001);

// Unix versions
std::wstring string2Wide(const std::string& str);
std::string wide2String(const std::wstring& str);
```

### 3.2 UTF-8 String Utilities

#### splitFront

```cpp
std::string splitFront(const char* data);
```
- **Function**: Extract the first character from a UTF-8 string
- **Parameter**: `data` - UTF-8 string
- **Return Value**: First character (may be multi-byte)

#### splitUTF8

```cpp
std::vector<std::string> splitUTF8(const char* data, size_t *display_size = nullptr);
```
- **Function**: Split UTF-8 string into character array
- **Parameters**:
  - `data` - UTF-8 string
  - `display_size` - Optional output parameter for display width (default: `nullptr`)
- **Return Value**: Character array

#### calcStrDisplayWidth

```cpp
size_t calcStrDisplayWidth(const std::string& str);
```
- **Function**: Calculate the display width (terminal columns) of a UTF-8 string
- **Parameter**: `str` - UTF-8 string
- **Return Value**: Display width in columns (CJK characters count as 2)

#### calcDisplaySize

```cpp
size_t calcDisplaySize(const std::string& str);
```
- **Function**: Calculate display size of a UTF-8 string
- **Parameter**: `str` - UTF-8 string
- **Return Value**: Display size

#### subUTF8

```cpp
std::string subUTF8(const char* data, size_t display_count, size_t offset = 0,
                    size_t *result_display_count = nullptr);
```
- **Function**: Extract a substring by display-width count and offset
- **Parameters**:
  - `data` - UTF-8 string
  - `display_count` - Number of display columns to extract
  - `offset` - Display-width offset (default: `0`)
  - `result_display_count` - Optional output: actual display count extracted
- **Return Value**: UTF-8 substring

#### lastCharCount

```cpp
size_t lastCharCount(const std::string& buf);
```
- **Function**: Get the byte length of the last UTF-8 character in a buffer
- **Parameter**: `buf` - UTF-8 string
- **Return Value**: Byte length of the last character

---

## 4. Helper Functions

### 4.1 getKeyName

```cpp
const char* getKeyName(const uint8_t& KEY, const SP_Keys& SP);
```
- **Function**: Get key name
- **Parameters**:
  - `KEY` - Key code
  - `SP` - Special key type
- **Return Value**: Key name string

### 4.2 getMouseName

```cpp
const char* getMouseName(const SP_Mouse& SP);
```
- **Function**: Get mouse event name
- **Parameter**: `SP` - Mouse event type
- **Return Value**: Event name string

### 4.3 isPointInRect

```cpp
bool isPointInRect(const Position& point, const Position& start_pos, const Position& end_pos);
bool isPointInRect(const Position& point, const Position& start_pos, const Size& size);
```
- **Function**: Check if a point is inside a rectangle
- **Parameters**:
  - `point` - Point to check
  - `start_pos` - Top-left corner of rectangle
  - `end_pos` - Bottom-right corner of rectangle (first overload)
  - `size` - Size of rectangle (second overload)
- **Return Value**: `true` if point is inside the rectangle

### 4.4 KEY_BACKSPACE

```cpp
constexpr bool KEY_BACKSPACE(uint8_t key);
```
- **Function**: Check if a key code represents a backspace key
- **Parameter**: `key` - Key code to check
- **Return Value**: `true` if key is `KEY_BK` (8) or `KEY_DEL` (127)
- **Note**: Helper function for convenient backspace key detection

### 4.5 KEY_ENTER

```cpp
constexpr bool KEY_ENTER(uint8_t key);
```
- **Function**: Check if a key code represents an enter key
- **Parameter**: `key` - Key code to check
- **Return Value**: `true` if key is `KEY_CR` (13) or `KEY_LF` (10)
- **Note**: Helper function for convenient enter key detection

### 4.6 KEY_RETURN

```cpp
constexpr bool KEY_RETURN(uint8_t key);
```
- **Function**: Alias of `KEY_ENTER`

### 4.7 KEY_CONFIRM

```cpp
constexpr bool KEY_CONFIRM(uint8_t key);
```
- **Function**: Check if a key code represents a confirm key (Space, CR, or LF)

### 4.8 KEY_CANCEL

```cpp
constexpr bool KEY_CANCEL(uint8_t key);
```
- **Function**: Check if a key code represents a cancel key (BK, DEL, or ESC)

---

## 5. Data Structures

### 5.1 Size Structure

```cpp
struct Size {
    uint32_t width;   // Width (columns)
    uint32_t height;  // Height (rows)

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

| Member | Type | Description |
|--------|------|-------------|
| `width` | `uint32_t` | Width in columns |
| `height` | `uint32_t` | Height in rows |
| `Size()` | - | Default constructor, `{0, 0}` |
| `Size(w, h)` | - | Construct with explicit width and height |
| `isEqual(other)` | - | `width * height == other.width * other.height` (same total area) |
| `compare(other)` | - | Returns `1` if smaller, `-1` if larger, `0` if equal (compared by height then width) |
| `operator+/-/*` | - | Arithmetic operators (modify in-place, return `*this`) |

### 5.2 Position Structure

```cpp
struct Position {
    uint32_t row;     // Row number (0-based)
    uint32_t column;  // Column number (0-based)

    Position();
    Position(uint32_t row, uint32_t column);
    Position(const Size& size);  // Constructs from size: row=height, column=width

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

| Member | Type | Description |
|--------|------|-------------|
| `row` | `uint32_t` | Row (0-based) |
| `column` | `uint32_t` | Column (0-based) |
| `Position()` | - | Default constructor, `{0, 0}` |
| `Position(r, c)` | - | Construct with explicit row and column |
| `Position(const Size&)` | - | Constructs `{height, width}` from a `Size` |
| `calcEndPos(size)` | - | Returns `{row + size.height - 1, column + size.width - 1}` |
| `compare(other)` | - | Returns `1` if smaller, `-1` if larger, `0` if equal (compared by row then column) |
| `swap(other)` | - | Swap positions noexcept |
| `operator+/-` | - | Arithmetic operators (modify in-place, return `*this`) |

### 5.3 Color Enum

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

### 5.4 Keys Enum

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

**Key Aliases** (helpers like `KEY_BACKSPACE`, `KEY_ENTER`, etc. are provided by constexpr functions — see [Helper Functions](#4-helper-functions)).

### 5.5 SP_Keys Enum (Special Keys)

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

### 5.6 SP_Mouse Enum (Mouse Events)

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
    // Aliases (same values, 0-7):
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

### 5.7 InputEvent Structure

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
            bool is_pressed;    // P.s: Only Windows can captured, otherwise it is always `true`!
        } keyboard;
        struct Mouse {
            Position position;
            SP_Mouse button;
            bool is_pressed;
        } mouse;
    } input;
};
```

| Member | Type | Description |
|--------|------|-------------|
| `type` | `Type` | Event type |
| `input.keyboard.key` | `uint8_t` | Key code |
| `input.keyboard.sp_key` | `SP_Keys` | Special key type |
| `input.keyboard.is_pressed` | `bool` | Whether key is pressed (Windows only) |
| `input.mouse.position` | `Position` | Mouse position |
| `input.mouse.button` | `SP_Mouse` | Mouse button/event |
| `input.mouse.is_pressed` | `bool` | Whether mouse button is pressed |

**Type Aliases**:
- `None = N = 0`
- `Keyboard = Key = K = 1`
- `Mouse = M = 2`

### 5.8 Type Aliases

```cpp
using KeyEvent = InputEvent::Input::Keyboard;
using MouseEvent = InputEvent::Input::Mouse;
```

### 5.9 RGBColor Structure

```cpp
struct RGBColor {
    uint8_t r, g, b;

    RGBColor();
    RGBColor(uint8_t r, uint8_t g, uint8_t b);
    bool operator==(const RGBColor& other) const;
    bool operator!=(const RGBColor& other) const;
};
```

### 5.10 Char Class

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

A lightweight wrapper for a single character (supports UTF-8 multi-byte characters).

### 5.11 Alignment Enum

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

### 5.12 TextAlignment Enum

```cpp
enum class TextAlignment : uint8_t {
    Left,
    Center,
    Right
};
```

### 5.13 SizePolicy Enum

```cpp
enum class SizePolicy : uint8_t {
    Fixed,
    Maximized,
    Minimized,
    Ignored
};
```

### 5.14 Orientation Enum

```cpp
enum class Orientation : uint8_t {
    Horizontal, H = 0,
    Vertical, V = 1
};
```

Orientation for `Slider` and `ProgressBar`.

---

## 6. Terminal Class

### 6.1 Class Overview

Terminal control class providing raw mode switching, screen control, cursor operations, color settings, input reading, and other functions.

All member functions are static.

#### self

```cpp
static Terminal& self();
```
- **Function**: Get terminal singleton instance
- **Return Value**: Terminal reference
- **Note**: Enables stream-style output chaining

### 6.2 Raw Mode Control

#### enterRawMode

```cpp
static bool enterRawMode();
```
- **Function**: Enter raw mode (disable line buffering, echo, etc.)
- **Return Value**: `true` means success
- **Notes**:
  - Windows: Creates new screen buffer
  - Unix: Uses termios to set raw mode

#### leaveRawMode

```cpp
static bool leaveRawMode();
```
- **Function**: Exit raw mode, restore terminal settings
- **Return Value**: `true` means success

#### isInRawMode

```cpp
static bool isInRawMode();
```
- **Function**: Check if currently in raw mode
- **Return Value**: `true` means in raw mode

### 6.3 Screen Information

#### screenSize

```cpp
static Size screenSize();
```
- **Function**: Get terminal screen size
- **Return Value**: `Size` structure (width and height)

#### cursorPosition

```cpp
static Position cursorPosition();
```
- **Function**: Get current cursor position
- **Return Value**: `Position` structure (row and column)

### 6.4 Output Functions

#### print

```cpp
static bool print(char ch);
static bool print(const std::string& text);
```
- **Function**: Output a single character or text (no newline)

#### printW

```cpp
static bool printW(wchar_t ch);
static bool printW(const std::wstring& text);
```
- **Function**: Output a wide character or wide text (no newline)

#### printLine

```cpp
static bool printLine(const std::string& text = {});
```
- **Function**: Output text with newline

#### printLineW

```cpp
static bool printLineW(const std::wstring& text = {});
```
- **Function**: Output wide text with newline

#### printFormat

```cpp
template<typename ... Args>
static bool printFormat(const char* format, Args... args);
```
- **Function**: Formatted output (uses `{}` placeholders)

#### formatString

```cpp
template<typename ... Args>
static std::string formatString(const char* format, Args... args);
```
- **Function**: Format string without printing
- **Return Value**: Formatted string

#### printError

```cpp
template<typename ... Args>
static bool printError(const char* format, Args... args);
static bool printError(const std::string& text);
static bool printErrorW(const std::wstring& text);
```
- **Function**: Output error text to stderr
- **Overloads**:
  - Template version with format string and variable args
  - Direct `std::string` version
  - Wide-character version

#### Stream-style print (v1.2.0)

```cpp
static Terminal& print();      // Stream-style, no arg needed
static Terminal& perror();     // Stream-style error output
Terminal& operator<<(const std::string& text);
Terminal& operator<<(char ch);
Terminal& operator<<(int value);
Terminal& operator<<(bool expr);
Terminal& operator<<(const wchar_t* expr);
```
- **Note**: Enables fluent API style: `Terminal::self() << "Hello " << 42`

### 6.5 Screen Control

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

### 6.6 Input Functions

#### readLine / readLineW

```cpp
static std::string readLine();
static std::wstring readLineW();
```

#### getKey

```cpp
static uint8_t getKey(SP_Keys* sp_key = nullptr);
```

### 6.7 Mouse Control

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

### 6.8 Color and Style Functions

#### Color Settings

```cpp
static void setBackgroundColor(Color color, bool intensity = true);
static void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
static void setForegroundColor(Color color, bool intensity = false);
static void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
```

#### Style Settings

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

### 6.9 TStyle Namespace (v1.2.0)

Stream-style interface for setting terminal colors and styles. All functions return `Terminal&` for method chaining.

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

**Example**:
```cpp
Terminal::self() << TStyle::fg(Color::Green) << TStyle::bold() << "Green bold text";
Terminal::self() << TStyle::bg(255, 0, 0) << "Red background";
Terminal::self() << TStyle::reset();
```

---

## 7. Renderer Class

### 7.1 Class Overview

Double-buffered terminal renderer supporting character drawing, rectangle filling, border drawing, and other functions.

### 7.2 Nested Structures

#### Style (also available as `Tiny::TUI::Style`)

```cpp
struct Style {
    uint8_t property;       // Style property (use Property enum)
    Color bg_color;         // Background color (ANSI 16 colors)
    Color fg_color;         // Foreground color (ANSI 16 colors)
    uint8_t intensity;      // Color intensity: 0=None, 1=Background only, 2=Foreground only, 3=All
    bool used_rgb_color;    // Whether to use RGB colors
    RGBColor bg_rgb_color;  // RGB background color
    RGBColor fg_rgb_color;  // RGB foreground color

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

> **Note**: `Renderer::Style` and `Renderer::Corner` are deprecated typedefs. Use `Tiny::TUI::Style` and `Tiny::TUI::Corner` directly — they will be removed since ver.0.3.0.

| Member | Type | Description |
|--------|------|-------------|
| `property` | `uint8_t` | Style property bitmask |
| `bg_color` | `Color` | ANSI background color (default: `Color::Default`) |
| `fg_color` | `Color` | ANSI foreground color (default: `Color::Default`) |
| `intensity` | `uint8_t` | Color intensity (default: 2 = Foreground only) |
| `used_rgb_color` | `bool` | Whether to use RGB colors |
| `bg_rgb_color` | `RGBColor` | RGB background color |
| `fg_rgb_color` | `RGBColor` | RGB foreground color |

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

#### Corner (also available as `Tiny::TUI::Corner`)

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

### 7.3 Static Member Functions

#### self

```cpp
static Renderer& self();
```

### 7.4 Member Functions

#### set (overloads)

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

#### setResizeEvent (Deprecated)

```cpp
void setResizeEvent(const std::function<void(Renderer&)>& event);
```
- **Deprecated**: Replaced by `EventBus`, will be removed in v1.4.0.

### 7.5 Protected Virtual Functions

```cpp
virtual void renderEvent();
virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
```

---

## 8. Object Class

### 8.1 Class Overview

Base class for all objects in the TUI hierarchy. Provides object naming, parent-child relationships, type information, and event dispatch. Both `AbstractWidget` and `AbstractLayout` inherit (indirectly) from `Object`.

### 8.2 Constructors

```cpp
explicit Object(const std::string& name, std::type_index type_id, Object* parent = nullptr);
explicit Object(const std::string& name, std::type_index type_id, std::type_index parent_type_id, Object* parent = nullptr);
```

| Parameter | Description |
|-----------|-------------|
| `name` | Object name |
| `type_id` | `std::type_index` identifying the actual derived type |
| `parent_type_id` | `std::type_index` of the expected parent type (second constructor only) |
| `parent` | Parent object pointer (optional, default: `nullptr`) |

### 8.3 Destructor

```cpp
virtual ~Object() = default;
```

### 8.4 Member Functions

#### renameObject / setObjectName / objectName

```cpp
void renameObject(const std::string& name);
void setObjectName(const std::string& name);
[[nodiscard]] const std::string& objectName() const;
```

Rename or query the object's name. `renameObject` and `setObjectName` have the same effect.

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

- `hash()` — type hash of this object
- `phash()` — type hash of the parent

#### className

```cpp
const char* className() const;
```

Returns a human-readable class name derived from the stored `type_index`.

#### isChild

```cpp
bool isChild(Object* child) const;
```

Check if `child` is a direct child of this object.

#### findChild

```cpp
Object* findChild(const std::string& name) const;
Object* findChild(std::type_index type_id, const std::string& name) const;
```

Find a child object by name (first overload) or by type + name (second overload). Returns `nullptr` if not found.

#### children

```cpp
[[nodiscard]] const std::vector<Object*>& children() const;
```

Returns a read-only vector of direct child objects.

### 8.5 Protected Virtual Functions

Subclasses **must** override:

```cpp
virtual void onEvent(const AbstractEvent& event) = 0;
virtual void onResizedTermSize(const Size& size) = 0;
virtual void onObjectNameChanged() = 0;
virtual void onParentChanged() = 0;
```

These are called by the framework when events arrive, the terminal is resized, or parent/name changes occur.

---

## 9. AbstractWidget Class

### 9.1 Class Overview

Abstract widget base class, inherits from `Object`. Provides position/size management, style states, checkable state, mouse tracing, and a render/event interface that subclasses must implement.

### 9.2 StyleStatus Enum

```cpp
enum StyleStatus : uint8_t {
    S_Disabled,
    S_Active,
    S_Checked,
    S_Normal
};
```

Used with `setStyle()` to associate a `Style` with a widget state.

### 9.3 Constructors

```cpp
explicit AbstractWidget(const std::string& name, const Position& position, const Size& size,
                        std::type_index type_id, Object* parent = nullptr);
explicit AbstractWidget(const std::string& name, std::type_index type_id, Object* parent = nullptr);
```

| Parameter | Description |
|-----------|-------------|
| `name` | Widget name (passed to `Object` base) |
| `position` | Widget position |
| `size` | Widget size |
| `type_id` | `std::type_index` of the concrete subclass |
| `parent` | Parent object (optional) |

### 9.4 Destructor

```cpp
virtual ~AbstractWidget() = default;
```

### 9.5 Member Functions

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

`setMinMaxSize` sets both the minimum and maximum size to the same value (useful for locking size).

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

When enabled, the widget receives continuous mouse move events even when not pressed.

#### setStyle / style

```cpp
void setStyle(uint8_t status, const Style& style);
[[nodiscard]] Style style(uint8_t status) const;
```

Associate a `Style` with a widget state (`S_Disabled`, `S_Active`, `S_Checked`, `S_Normal`).

#### draw (DEPRECATED)

```cpp
API_DEPRECATED("The function will be removed since ver.0.3.0!")
void draw();
```

> Use `callDrawEvent()` (protected) or let the `Application` handle redrawing.

#### Position / Size getters

```cpp
[[nodiscard]] const Position& position() const;
[[nodiscard]] const Size& size() const;
[[nodiscard]] const Size& minimumSize() const;
[[nodiscard]] const Size& maximumSize() const;
```

#### State getters

```cpp
[[nodiscard]] bool enabled() const;
[[nodiscard]] bool visible() const;
[[nodiscard]] bool focus() const;
[[nodiscard]] SizePolicy sizePolicy() const;
```

#### Checkable state

```cpp
[[nodiscard]] bool checkable() const;
[[nodiscard]] bool checked() const;
```

Subclasses control checkable state through protected methods:

```cpp
protected:
    void setCheckable(bool checkable);
    void setChecked(bool checked);
```

### 9.6 Protected Virtual Functions

All subclasses **must** implement:

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

Additional protected helpers available to subclasses:

```cpp
void callDrawEvent();
void resizeWithoutCalledEvent(uint32_t width, uint32_t height);
const Style& currentStyle(uint8_t* status = nullptr) const;
```

---

## 10. EventBus Class

### 10.1 Class Overview

Event bus for managing and dispatching events in the TUI system.

### 10.2 Event Types

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

### 10.3 Type Aliases

```cpp
using Subscriber = std::function<void(const AbstractEvent&)>;
using SubscriberMap = std::unordered_map<size_t, Subscriber>;
using SubscriberID = size_t;
```

### 10.4 Member Functions

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
- **Return Value**: Subscriber ID for unsubscribing

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

Publish to a specific subscriber (first overload) or all subscribers of type `T` (second overload). Higher `priority` values execute first.

#### pollEvents / clear

```cpp
void pollEvents();
void clear();
```

---

## 11. Application Class

### 11.1 Class Overview

Main application class for TUI programs. Owns the event loop, manages all registered `Object` instances (widgets and layouts), and coordinates rendering.

### 11.2 Constructor

```cpp
explicit Application();
```

Default constructor. No `argc` / `argv` — argument handling is the user's responsibility.

### 11.3 Destructor

```cpp
virtual ~Application() = default;
```

### 11.4 Member Functions

#### run

```cpp
int run();
```
- **Function**: Run the application main loop
- **Return Value**: Exit code (set by `exit()`)

#### exit

```cpp
void exit(int8_t exit_code = 0);
```
- **Function**: Request application termination with the given exit code
- **Parameter**: `exit_code` — exit status (default: `0`)

#### setEnabledExitByKey / isEnabledExitByKey

```cpp
void setEnabledExitByKey(bool enabled);
bool isEnabledExitByKey() const;
```

When enabled, pressing `Ctrl+C` or `ESC` will exit the main loop. Enabled by default.

#### setRefreshEnabled / isRefreshEnabled

```cpp
void setRefreshEnabled(bool enabled);
bool isRefreshEnabled() const;
```

Control whether the renderer is refreshed on each loop iteration.

#### Z-order control

```cpp
void setZOrder(const Object* object, uint32_t z_order);
void setZOrder(uint32_t dst_order, uint32_t src_order);
void setZOrder(const Object* dst_object, const Object* src_object);
```

| Overload | Description |
|----------|-------------|
| `(object, z_order)` | Move an object to a specific Z-order position |
| `(dst_order, src_order)` | Swap two Z-order positions by index |
| `(dst_object, src_object)` | Swap Z-order of two objects |

```cpp
uint32_t zOrder() const;
const Object* zOrderOf(uint32_t dst_order) const;
```

- `zOrder()` — current total Z-order count
- `zOrderOf(dst_order)` — object at the given Z-order position (or `nullptr`)

#### count

```cpp
uint32_t count() const;
```

Number of top-level `Object` instances registered with the application.

---

## 12. AbstractLayout Class

### 12.1 Class Overview

Abstract layout base class for managing widget arrangements. Inherits from `Object`.

### 12.2 Type Aliases

```cpp
using WidgetIter = std::vector<AbstractWidget*>::iterator;
using CWidgetIter = std::vector<AbstractWidget*>::const_iterator;
```

### 12.3 Constructor

```cpp
AbstractLayout(const std::string& name, std::type_index type_id, Object* parent = nullptr);
```

| Parameter | Description |
|-----------|-------------|
| `name` | Layout name |
| `type_id` | `std::type_index` of the concrete layout subclass |
| `parent` | Parent object (optional) |

### 12.4 Destructor

```cpp
virtual ~AbstractLayout() = default;
```

### 12.5 Member Functions

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

#### Widget management

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

All widget-modification functions return `true` on success. Both pointer-based and index-based overloads are provided for remove/replace/swap.

#### Position / Size / State getters

```cpp
[[nodiscard]] const Position& position() const;
[[nodiscard]] const Size& size() const;
[[nodiscard]] bool enabled() const;
[[nodiscard]] bool visible() const;
```

#### Iterators

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

### 12.6 Protected Virtual Functions

```cpp
virtual void renderEvent(Renderer& renderer) = 0;
virtual void moveEvent(uint32_t x, uint32_t y) = 0;
virtual void resizeEvent(uint32_t width, uint32_t height) = 0;
```

---

## 13. CurBlock Class

### 13.1 Class Overview

A cursor-block widget. Renders as a solid block that can act as a cursor indicator.

Inherits from `AbstractWidget`.

### 13.2 Constructor

```cpp
explicit CurBlock(const std::string& name, Object* parent = nullptr);
```

| Parameter | Description |
|-----------|-------------|
| `name` | Widget name |
| `parent` | Parent object (optional) |

### 13.3 Destructor

```cpp
virtual ~CurBlock() = default;
```

This class is intentionally minimal — the rendering behavior is entirely handled by the protected virtual methods defined in `AbstractWidget`.

---

## 14. Label Class

### 14.1 Class Overview

Simple text label widget. Inherits from `AbstractWidget`.

### 14.2 Constructors

```cpp
explicit Label(const std::string& name, const Position& position, Object* parent = nullptr);
explicit Label(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

| Parameter | Description |
|-----------|-------------|
| `name` | Widget name |
| `position` | Widget position |
| `size` | Widget size (second constructor; auto-sized by default otherwise) |
| `parent` | Parent object (optional) |

### 14.3 Destructor

```cpp
virtual ~Label() = default;
```

### 14.4 Member Functions

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

When enabled (the default, when no explicit size is given), the label sizes itself to fit its text.

#### setAlignment / alignment

```cpp
void setAlignment(Alignment alignment);
[[nodiscard]] Alignment alignment() const;
```

Alignment within the widget's allocated area.

---

## 15. Button Class

### 15.1 Class Overview

Clickable button widget. Inherits from `Label`. Supports a click callback and configurable default activation keys.

### 15.2 Constructors

```cpp
Button(const std::string& name, const Position& position, Object* parent = nullptr);
Button(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

### 15.3 Destructor

```cpp
virtual ~Button() = default;
```

### 15.4 Member Functions

#### setEvent / unsetEvent

```cpp
void setEvent(const std::function<void(Button&)>& event);
void unsetEvent();
```

Set or clear the click callback. The callback receives a reference to the clicked `Button`.

#### setDefaultKeyEvent

```cpp
void setDefaultKeyEvent(const std::array<KeyEvent, 2>& key_events);
```

Set two keyboard shortcuts that activate the button.

#### setDefaultKeys

```cpp
void setDefaultKeys(uint8_t key1, uint8_t key2 = KEY_NONE,
                    SP_Keys sp_key1 = SP_KEY_NONE, SP_Keys sp_key2 = SP_KEY_NONE);
```

Convenience overload — specify key codes and special keys directly instead of constructing `KeyEvent` objects.

---

## 16. LineEdit Class

### 16.1 Class Overview

Single-line text input widget. Inherits from `AbstractWidget`.

### 16.2 EchoMode Enum

```cpp
enum class EchoMode : uint8_t {
    NoEcho,
    Normal,
    Password
};
```

### 16.3 Constructor

```cpp
explicit LineEdit(const std::string& name, const Position& position, uint32_t width, Object* parent = nullptr);
```

### 16.4 Destructor

```cpp
virtual ~LineEdit() = default;
```

### 16.5 Member Functions

#### Text manipulation

```cpp
void setText(const std::string& text);
void setText(const char* text);
void appendText(const char* text);
void appendText(const std::string& text);
void clear();
```

#### Length constraints

```cpp
void setMinimumLength(uint16_t size);
void setMaximumLength(uint16_t size);
[[nodiscard]] uint16_t minimumLength() const;
[[nodiscard]] uint16_t maximumLength() const;
```

#### Placeholder

```cpp
void setPlaceHolderText(const std::string& text);
void setPlaceHolderText(const char* text);
```

#### Echo mode

```cpp
void setEchoMode(EchoMode mode);
[[nodiscard]] EchoMode echoMode() const;

void setEchoPassChar(const Char& ch);
[[nodiscard]] const Char& echoPassChar() const;
```

When `EchoMode::Password` is set, `echoPassChar` (default `*`) replaces each character for display.

#### Text alignment

```cpp
void setTextAlignment(TextAlignment alignment);
[[nodiscard]] TextAlignment textAlignment() const;
```

#### text

```cpp
[[nodiscard]] const std::string& text() const;
```

Current input value.

---

## 17. Slider Class

### 17.1 Class Overview

Interactive slider widget. Allows the user to drag or step through a numeric range. Inherits from `AbstractWidget`.

### 17.2 Constructor

```cpp
explicit Slider(const std::string& name, const Position& position, uint8_t width, Object* parent = nullptr);
```

`width` is the number of display columns (or rows, depending on orientation).

### 17.3 Destructor

```cpp
virtual ~Slider() = default;
```

### 17.4 Member Functions

#### Orientation / width

```cpp
void setOrientation(Orientation mode);
[[nodiscard]] Orientation orientation() const;

void setWidth(uint8_t width);
[[nodiscard]] uint8_t width() const;
```

#### Range / value

```cpp
void setMinimumValue(int value);
void setMaximumValue(int value);
void setValue(int value);
void appendValue(int value);

[[nodiscard]] int minimumValue() const;
[[nodiscard]] int maximumValue() const;
[[nodiscard]] int value() const;
```

#### Step sizes

```cpp
void setSingleStep(int value);
void setPageStep(int value);

[[nodiscard]] int singleStep() const;
[[nodiscard]] int pageStep() const;
```

#### Inverted

```cpp
void setInvertedEnabled(bool enable);
[[nodiscard]] bool invertedEnabled() const;
```

When enabled, the slider fills from the opposite end.

#### Event callback

```cpp
void setEvent(const std::function<void(int)>& event);
void unsetEvent();
```

The callback receives the current integer value whenever it changes.

#### Fill color

```cpp
void setFilledColor(const Color& fg_color, const Color& bg_color);
[[nodiscard]] Color fgFilledColor() const;
[[nodiscard]] Color bgFilledColor() const;
```

---

## 18. ProgressBar Class

### 18.1 Class Overview

Non-interactive progress indicator. Inherits from `AbstractWidget`. Unlike `Slider`, it does not respond to user input.

### 18.2 Constructor

```cpp
explicit ProgressBar(const std::string& name, const Position& position, uint32_t width,
                     Object* parent = nullptr);
```

### 18.3 Destructor

```cpp
virtual ~ProgressBar() = default;
```

### 18.4 Member Functions

#### Orientation / width

```cpp
void setOrientation(Orientation mode);
[[nodiscard]] Orientation orientation() const;

void setWidth(uint8_t width);
[[nodiscard]] uint8_t width() const;
```

#### Value

```cpp
void setValue(int value);
void appendValue(int value);
[[nodiscard]] int value() const;
```

#### Inverted

```cpp
void setInvertedEnabled(bool enable);
[[nodiscard]] bool invertedEnabled() const;
```

#### Fill color

```cpp
void setFilledColor(const Color& fg_color, const Color& bg_color);
[[nodiscard]] Color fgFilledColor() const;
[[nodiscard]] Color bgFilledColor() const;
```

---

## 19. ListView Class

### 19.1 Class Overview

Scrollable list widget. Displays a set of strings with one selected item at a time. Inherits from `AbstractWidget`.

### 19.2 Constructor

```cpp
explicit ListView(const std::string& name, const Position& position, const Size& size, Object* parent = nullptr);
```

### 19.3 Destructor

```cpp
virtual ~ListView() = default;
```

### 19.4 Member Functions

#### Item manipulation

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

#### Selection

```cpp
void setCurrentIndex(int32_t index);
[[nodiscard]] int32_t currentIndex() const;
```

#### Queries

```cpp
[[nodiscard]] int32_t count() const;
[[nodiscard]] std::string currentItem() const;
[[nodiscard]] std::string itemAt(int32_t index) const;
```

#### Colors

```cpp
void setSelectionColor(const Color& fg_color, const Color& bg_color);
void setActiveColor(const Color& fg_color, const Color& bg_color);

[[nodiscard]] Color bgSelectionColor() const;
[[nodiscard]] Color fgSelectionColor() const;
[[nodiscard]] Color bgActiveColor() const;
[[nodiscard]] Color fgActiveColor() const;
```

---

## 20. Usage Examples

### 20.1 Basic Terminal Control

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

### 20.2 Slider Widget

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

### 20.3 Button with Keys

```cpp
using namespace Tiny::TUI;

auto* ok_btn = new Button("ok", {4, 4}, Size{10, 1});
ok_btn->setText(" OK ");
ok_btn->setDefaultKeys(KEY_CR, KEY_NONE, SP_KEY_NONE, SP_KEY_NONE);
ok_btn->setEvent([](Button& b) {
    Terminal::printError("Clicked: {}\n", b.objectName());
});
```

### 20.4 LineEdit with Password Mode

```cpp
using namespace Tiny::TUI;

auto* password = new LineEdit("pwd", {6, 4}, 30);
password->setEchoMode(LineEdit::EchoMode::Password);
password->setPlaceHolderText("Enter password");
password->setMaximumLength(32);
```

---

## 21. Notes

### 21.1 Raw Mode

- After entering raw mode, terminal will not automatically handle input/output
- Must manually handle Enter, Backspace, and other keys
- Must call `leaveRawMode()` before program exit
- Recommended to use RAII pattern to ensure terminal state restoration

### 21.2 Terminal Compatibility

- Requires terminal supporting ANSI escape sequences
- Windows 10+, modern Linux terminals, macOS Terminal are all supported
- Windows 7/8 may need to enable virtual terminal processing

### 21.3 Renderer Usage

- Uses double buffering mechanism, draws to buffer first
- Call `present()` to actually output to screen
- Call `clear()` to clear the front buffer before redrawing

### 21.4 `Application` vs. Direct `Renderer` Usage

- Widget-based UIs should use `Application::run()` — it handles the main loop, input dispatch, and rendering
- Direct `Renderer::self()` usage is appropriate for simple, non-widget programs or for custom renderers

### 21.5 Object Hierarchy

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

### 21.6 Deprecated Symbols

| Symbol | Replacement | Removes in |
|--------|-------------|------------|
| `Renderer::Style` typedef | `Tiny::TUI::Style` | v0.3.0 |
| `Renderer::Corner` typedef | `Tiny::TUI::Corner` | v0.3.0 |
| `Tiny::Code` namespace | `Tiny::U8Code` | v0.3.0 |
| `AbstractWidget::draw()` | Protected `callDrawEvent()` | v0.3.0 |
| `Renderer::setResizeEvent()` | `EventBus::subscribe<ResizeTermEvent>()` | v1.4.0 |

## 22. How to Use the GPM Library on Linux Console

See [GPM_In_Linux.md](GPM_In_Linux.md), which describes how to use the GPM library in a Linux non-desktop environment to solve mouse event handling issues in TTY mode.
