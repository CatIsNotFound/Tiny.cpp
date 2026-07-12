# TUI Module

Namespace: `Tiny::TUI`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Helper Functions](#3-helper-functions)
4. [Data Structures](#4-data-structures)
5. [Terminal Class](#5-terminal-class)
6. [Renderer Class](#6-renderer-class)
7. [AbstractWidget Class](#7-abstractwidget-class)
8. [Usage Examples](#8-usage-examples)
9. [Notes](#9-notes)

---

## 1. Module Overview

The `TUI` module provides terminal user interface functionality, including:

- **Terminal Control**: Raw mode switching, screen control, cursor operations
- **Input Handling**: Key reading, mouse events
- **Color & Style**: Foreground/background colors, bold, underline, etc.
- **Double Buffering**: Efficient screen rendering
- **Widget Base Class**: Extensible widget system

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/TUI/TUI.hpp>
// Direct source copy method
#include "TUI/TUI.hpp"
```

---

## 3. Helper Functions

### 3.1 splitFront

```cpp
std::string splitFront(const char* data);
```
- **Function**: Extract the first character from a UTF-8 string
- **Parameter**: `data` - UTF-8 string
- **Return Value**: First character (may be multi-byte)

### 3.2 splitUTF8

```cpp
std::vector<std::string> splitUTF8(const char* data);
```
- **Function**: Split UTF-8 string into character array
- **Parameter**: `data` - UTF-8 string
- **Return Value**: Character array

### 3.3 getKeyName

```cpp
const char* getKeyName(const uint8_t& KEY, const SP_Keys& SP);
```
- **Function**: Get key name
- **Parameters**: 
  - `KEY` - Key code
  - `SP` - Special key type
- **Return Value**: Key name string

### 3.4 getMouseName

```cpp
const char* getMouseName(const SP_Mouse& SP);
```
- **Function**: Get mouse event name
- **Parameter**: `SP` - Mouse event type
- **Return Value**: Event name string

### 3.5 comparePosition

```cpp
int8_t comparePosition(const Position& pos1, const Position& pos2);
```
- **Function**: Compare two positions
- **Parameters**: 
  - `pos1` - First position
  - `pos2` - Second position
- **Return Value**: 
  - `-1` - pos1 is before pos2
  - `0` - Same position
  - `1` - pos1 is after pos2

### 3.6 compareSize

```cpp
int8_t compareSize(const Size& size1, const Size& size2);
```
- **Function**: Compare two sizes
- **Parameters**: 
  - `size1` - First size
  - `size2` - Second size
- **Return Value**: 
  - `-1` - size1 is smaller than size2
  - `0` - Same size
  - `1` - size1 is larger than size2

### 3.7 isPointInRect

```cpp
bool isPointInRect(const Position& point, Position& start_pos, Position& end_pos);
```
- **Function**: Check if a point is inside a rectangle
- **Parameters**: 
  - `point` - Point to check
  - `start_pos` - Top-left corner of rectangle
  - `end_pos` - Bottom-right corner of rectangle
- **Return Value**: `true` if point is inside the rectangle

---

## 4. Data Structures

### 4.1 Size Structure

```cpp
struct Size {
    uint32_t width;   // Width (columns)
    uint32_t height;  // Height (rows)
};
```

### 4.2 Position Structure

```cpp
struct Position {
    uint32_t row;     // Row number (0-based)
    uint32_t column;  // Column number (0-based)
};
```

### 4.3 Color Enum

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

### 4.4 Keys Enum

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

**Control Keys**:
- `KEY_CTRL_A` to `KEY_CTRL_Z`: Correspond to Ctrl+A to Ctrl+Z

**Key Aliases**:
- `KEY_BK` = `KEY_BACKSPACE` = `8`
- `KEY_CR` = `KEY_ENTER` = `13`
- `KEY_DEL` = `127`

### 4.5 SP_Keys Enum (Special Keys)

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

### 4.6 SP_Mouse Enum (Mouse Events)

```cpp
enum SP_Mouse : uint8_t {
    SP_MOUSE_UNKNOWN,
    SP_MOUSE_LEFT_BUTTON,    // Left button
    SP_MOUSE_MIDDLE_BUTTON,  // Middle button
    SP_MOUSE_RIGHT_BUTTON,   // Right button
    SP_MOUSE_WHEEL_UP,       // Wheel up
    SP_MOUSE_WHEEL_DOWN,     // Wheel down
    SP_MOUSE_MOVED,          // Mouse moved
    SP_MOUSE_RELEASE         // Mouse button released
};
```

### 4.7 InputEvent Structure

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
| `type` | `Type` | Event type: None, Keyboard, or Mouse |
| `input.keyboard.key` | `uint8_t` | Key code |
| `input.keyboard.sp_key` | `SP_Keys` | Special key type |
| `input.keyboard.is_pressed` | `bool` | Whether key is pressed (Windows only) |
| `input.mouse.position` | `Position` | Mouse position |
| `input.mouse.button` | `SP_Mouse` | Mouse button/event |
| `input.mouse.is_pressed` | `bool` | Whether mouse button is pressed |

### 4.8 Type Aliases

```cpp
using KeyEvent = InputEvent::Input::Keyboard;
using MouseEvent = InputEvent::Input::Mouse;
```

### 4.9 RGBColor Structure

```cpp
struct RGBColor {
    uint8_t r, g, b;

    RGBColor();
    RGBColor(uint8_t r, uint8_t g, uint8_t b);
    bool operator==(const RGBColor& other) const;
    bool operator!=(const RGBColor& other) const;
};
```

| Member | Type | Description |
|--------|------|-------------|
| `r` | `uint8_t` | Red component (0-255) |
| `g` | `uint8_t` | Green component (0-255) |
| `b` | `uint8_t` | Blue component (0-255) |

### 4.10 Char Class

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

| Member | Description |
|--------|-------------|
| `Char()` | Default constructor, initializes with a space character |
| `Char(const char* data)` | Construct from a C-string (extracts the first UTF-8 character) |
| `Char(const std::string& data)` | Construct from a `std::string` (extracts the first UTF-8 character) |
| `operator=` | Assignment operators (from string, C-string, or another Char) |
| `operator==` / `operator!=` | Comparison operators |
| `data()` | Get the underlying string representation of the character |
| `length()` | Get the byte length of the character |

---

## 5. Terminal Class

### 5.1 Class Overview

Terminal control class providing raw mode switching, screen control, cursor operations, color settings, input reading, and other functions.

All member functions are static.

### 5.2 Raw Mode Control

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

### 5.3 Screen Information

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

### 5.4 Output Functions

#### print

```cpp
static bool print(char ch);
static bool print(const std::string& text);
```
- **Function**: Output a single character or text (no newline)
- **Parameters**: 
  - `ch` - Single character
  - `text` - Text to output
- **Return Value**: `true` means success

#### printLine

```cpp
static bool printLine(const std::string& text = {});
```
- **Function**: Output text with newline
- **Parameter**: `text` - Text to output (optional, defaults to empty line)
- **Return Value**: `true` means success

#### printFormat

```cpp
template<typename ... Args>
static bool printFormat(const char* format, Args... args);
```
- **Function**: Formatted output (similar to Python f-string)
- **Parameters**: 
  - `format` - Format string using `{}` as placeholders
  - `args` - Variable arguments
- **Return Value**: `true` means success
- **Example**: `printFormat("Hello, {}!", "World")`

#### formatString

```cpp
template<typename ... Args>
static std::string formatString(const char* format, Args... args);
```
- **Function**: Format string
- **Parameters**: Same as `printFormat`
- **Return Value**: Formatted string

### 5.5 Screen Control

#### clearScreen

```cpp
static bool clearScreen();
```
- **Function**: Clear screen
- **Return Value**: `true` means success

#### clearInRow

```cpp
static bool clearInRow(uint8_t row);
```
- **Function**: Clear specified row
- **Parameter**: `row` - Row number (0-based)
- **Return Value**: `true` means success

#### moveCursor

```cpp
static bool moveCursor(Position position);
static bool moveCursor(uint32_t row, uint32_t column);
```
- **Function**: Move cursor to specified position
- **Parameters**: Target position
- **Return Value**: `true` means success

#### setScrollRegion

```cpp
static bool setScrollRegion(uint32_t row_start, uint32_t row_end);
```
- **Function**: Set scroll region
- **Parameters**: 
  - `row_start` - Start row
  - `row_end` - End row
- **Return Value**: `true` means success

#### resetScrollRegion

```cpp
static bool resetScrollRegion();
```
- **Function**: Reset scroll region to full screen
- **Return Value**: `true` means success

#### flushScreen

```cpp
static bool flushScreen();
```
- **Function**: Flush screen output
- **Return Value**: `true` means success

### 5.6 Input Functions

#### readLine

```cpp
static std::string readLine();
```
- **Function**: Read a line of input
- **Return Value**: Input string (without newline)

#### readLineW

```cpp
static std::wstring readLineW();
```
- **Function**: Read a line of wide character input
- **Return Value**: Wide string

#### getKey

```cpp
static uint8_t getKey(SP_Keys* sp_key = nullptr);
```
- **Function**: Read key press
- **Parameter**: 
  - `sp_key` - Output special key type (optional)
- **Return Value**: Key code

### 5.7 Mouse Control

#### setMouseEnabled

```cpp
static bool setMouseEnabled(bool enabled);
```
- **Function**: Enable/disable mouse events
- **Parameter**: `enabled` - Whether to enable
- **Return Value**: `true` means success

#### getMouseButton

```cpp
static uint8_t getMouseButton(Position* mouse_pos = nullptr, bool* is_pressed = nullptr);
```
- **Function**: Get mouse event
- **Parameters**: 
  - `mouse_pos` - Output mouse position (optional)
  - `is_pressed` - Output whether pressed (optional)
- **Return Value**: Mouse event code

#### getInput

```cpp
static InputEvent getInput();
```
- **Function**: Get unified input event (keyboard or mouse)
- **Return Value**: `InputEvent` structure containing event type and data

### 5.8 Color and Style Functions

#### Color Settings

```cpp
static void setBackgroundColor(Color color, bool intensity = true);
static void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
static void setForegroundColor(Color color, bool intensity = false);
static void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
```

| Function | Description | Parameters |
|----------|-------------|------------|
| `setBackgroundColor(Color, bool)` | Set background color (ANSI 16 colors) | `color` - Color, `intensity` - Whether bright |
| `setBackgroundColor(uint8_t, uint8_t, uint8_t)` | Set background color (RGB) | `r, g, b` - Red, green, blue components (0-255) |
| `setForegroundColor(Color, bool)` | Set foreground color (ANSI 16 colors) | `color` - Color, `intensity` - Whether bright |
| `setForegroundColor(uint8_t, uint8_t, uint8_t)` | Set foreground color (RGB) | `r, g, b` - Red, green, blue components (0-255) |

#### Style Settings

```cpp
static void setBolder(bool enable);        // Bold
static void setDark(bool enable);          // Dim
static void setItalic(bool enable);        // Italic
static void setUnderline(bool enable);     // Underline
static void setBlinking(bool enable);      // Blink
static void setReverseColor(bool enable);  // Reverse
static void setCursorVisible(bool enable); // Cursor visibility
static void setStrikethrough(bool enable); // Strikethrough
static void reset();                       // Reset all styles
```

---

## 6. Renderer Class

### 6.1 Class Overview

Double-buffered terminal renderer supporting character drawing, rectangle filling, border drawing, and other functions.

### 6.2 Nested Structures

#### Style

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
        Bolder            = 1,    // Bold
        Dark              = 2,    // Dim
        Italic            = 4,    // Italic
        Underline         = 8,    // Underline
        Blinking          = 16,   // Blink
        Reverse           = 32,   // Reverse
        Strikethrough     = 64,   // Strikethrough
    };

    Style();
    void reset();
    bool isDefault() const;
    bool operator==(const Style& other) const;
    bool operator!=(const Style& other) const;
};
```

| Member | Type | Description |
|--------|------|-------------|
| `property` | `uint8_t` | Style property bitmask |
| `bg_color` | `Color` | ANSI background color (default: `Color::Black`) |
| `fg_color` | `Color` | ANSI foreground color (default: `Color::Default`) |
| `intensity` | `uint8_t` | Color intensity: 0=None, 1=Background only, 2=Foreground only, 3=All (default: 2) |
| `used_rgb_color` | `bool` | Whether to use RGB colors (when true, ignores ANSI colors) |
| `bg_rgb_color` | `RGBColor` | RGB background color (r, g, b each 0-255) |
| `fg_rgb_color` | `RGBColor` | RGB foreground color (r, g, b each 0-255) |

**Property Enum**:
- Use bitwise operations to combine multiple properties: `Style::Bolder | Style::Underline`
- Or set directly via the `property` field

#### Cell

```cpp
struct Cell {
    Char data;        // Character data
    bool is_dirty;    // Whether modified
    Style style;      // Style

    Cell();
    void reset();
    void set(const char* ch, Style st);
};
```

| Member | Type | Description |
|--------|------|-------------|
| `data` | `Char` | Character data (supports multi-byte UTF-8) |
| `is_dirty` | `bool` | Whether the cell has been modified |
| `style` | `Style` | Cell style |

**Char Class**:

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

| Member | Type | Description |
|--------|------|-------------|
| `data` | `std::string` | Character string data |
| `length` | `uint8_t` | Byte length of the character data |

#### Corner

```cpp
struct Corner {
    Char left_top;     // Top-left corner
    Char left;         // Left side
    Char left_bottom;  // Bottom-left corner
    Char right_top;    // Top-right corner
    Char right;        // Right side
    Char right_bottom; // Bottom-right corner
    Char top;          // Top side
    Char bottom;       // Bottom side
};
```

### 6.3 Constructors and Static Member Functions

#### self

```cpp
static Renderer& self();
```
- **Function**: Get renderer singleton
- **Return Value**: Renderer reference

#### Destructor

```cpp
virtual ~Renderer();
```
- **Function**: Release renderer resources and stop background resize monitoring thread

### 6.4 Member Functions

#### set (overloads)

```cpp
void set(const Position& pos, uint8_t ch, Style style = {});
void set(uint32_t x, uint32_t y, uint8_t ch, Style style = {});
void set(const Position& pos, const std::string& str, Style style = {});
void set(uint32_t x, uint32_t y, const std::string& str, Style style = {});
```
- **Function**: Set character at specified position
- **Parameters**: 
  - `pos` / `x, y` - Position
  - `ch` / `str` - Character or string
  - `style` - Style (optional)

#### setStrF

```cpp
template<typename ... Args>
void setStrF(const Position& pos, const char* format, Args... args);
```
- **Function**: Set formatted string
- **Parameters**: 
  - `pos` - Position
  - `format` - Format string
  - `args` - Variable arguments

#### setSSF

```cpp
template<typename ... Args>
void setSSF(const Position& pos, const char* format, const Style& style, Args... args);
```
- **Function**: Set formatted string with style
- **Parameters**: 
  - `pos` - Position
  - `format` - Format string
  - `style` - Style
  - `args` - Variable arguments

#### fillScreen

```cpp
void fillScreen(const Style& style = {});
```
- **Function**: Fill entire screen with specified style
- **Parameter**: `style` - Fill style

#### fillRows

```cpp
void fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch = ' ', Style style = {});
void fillRows(uint32_t start_row, uint32_t end_row, const std::string& ch, Style style = {});
```
- **Function**: Fill specified row range
- **Parameters**: 
  - `start_row` - Start row
  - `end_row` - End row
  - `ch` / `str` - Fill character or string
  - `style` - Style

#### fillCols

```cpp
void fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch = ' ', Style style = {});
void fillCols(uint32_t start_col, uint32_t end_col, const std::string& ch, Style style = {});
```
- **Function**: Fill specified column range
- **Parameters**: 
  - `start_col` - Start column
  - `end_col` - End column
  - `ch` / `str` - Fill character or string
  - `style` - Style

#### fillRect

```cpp
void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch = ' ', Style style = {});
void fillRect(const Position& start_pos, const Position& end_pos, const std::string& str, Style style = {});
```
- **Function**: Fill rectangular region
- **Parameters**: 
  - `start_pos` - Start position
  - `end_pos` - End position
  - `ch` / `str` - Fill character or string
  - `style` - Style

#### drawBorder

```cpp
void drawBorder(const Position& start_pos, const Position& end_pos, Corner corner, Style style = {});
```
- **Function**: Draw border
- **Parameters**: 
  - `start_pos` - Top-left position
  - `end_pos` - Bottom-right position
  - `corner` - Corner character definitions
  - `style` - Style

#### unset

```cpp
void unset(const Position& pos);
void unset(uint32_t x, uint32_t y);
```
- **Function**: Clear specified position
- **Parameters**: Target position

#### unsetRow

```cpp
void unsetRow(uint32_t row);
```
- **Function**: Clear entire row
- **Parameter**: `row` - Row number

#### unsetCol

```cpp
void unsetCol(uint32_t col);
```
- **Function**: Clear entire column
- **Parameter**: `col` - Column number

#### unsetRect

```cpp
void unsetRect(const Position& start_pos, const Position& end_pos);
```
- **Function**: Clear rectangular region
- **Parameters**: 
  - `start_pos` - Start position
  - `end_pos` - End position

#### setResizeEvent

```cpp
void setResizeEvent(const std::function<void(Renderer&)>& event);
```
- **Function**: Set terminal resize event callback
- **Parameter**: `event` - Callback function, receives Renderer reference
- **Note**: Triggered when terminal window size changes

#### clear

```cpp
void clear();
```
- **Function**: Clear front buffer

#### present

```cpp
void present();
```
- **Function**: Present front buffer content to screen

### 6.5 Protected Virtual Functions

#### renderEvent

```cpp
virtual void renderEvent();
```
- **Function**: Internal render event handler, called when the renderer needs to redraw
- **Note**: Can be overridden in subclasses to customize rendering behavior

#### resizeEvent

```cpp
virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
```
- **Function**: Internal resize event handler, called when the terminal size changes
- **Parameters**: 
  - `use_default_size` - Whether to use the current terminal size (default: `true`)
  - `size` - Custom size to use when `use_default_size` is `false`
- **Note**: Can be overridden in subclasses to customize resize behavior

---

## 7. AbstractWidget Class

### 7.1 Class Overview

Abstract widget base class, base class for all TUI widgets.

### 7.2 Constructor

```cpp
explicit AbstractWidget(const std::string& name, const Position& position, const Size& size);
```
- **Parameters**: 
  - `name` - Widget name
  - `position` - Position
  - `size` - Size

### 7.3 Destructor

```cpp
virtual ~AbstractWidget();
```

### 7.4 Member Functions

#### rename

```cpp
void rename(const std::string& name);
```
- **Function**: Rename widget
- **Parameter**: `name` - New name

#### move

```cpp
void move(const Position& position);
void move(uint32_t x, uint32_t y);
```
- **Function**: Move widget position
- **Parameters**: New position

#### resize

```cpp
void resize(const Size& size);
void resize(uint32_t w, uint32_t h);
```
- **Function**: Resize widget
- **Parameters**: New size

#### name

```cpp
[[nodiscard]] const std::string& name() const;
```
- **Function**: Get widget name
- **Return Value**: Name reference

#### position

```cpp
[[nodiscard]] const Position& position() const;
```
- **Function**: Get widget position
- **Return Value**: Position reference

#### size

```cpp
[[nodiscard]] const Size& size() const;
```
- **Function**: Get widget size
- **Return Value**: Size reference

### 7.5 Protected Virtual Functions

```cpp
virtual void renderEvent();
```
- **Function**: Render event handling, can be overridden by subclasses to customize widget rendering
- **Note**: The base class provides a default empty implementation

---

## 8. Usage Examples

### 8.1 Basic Terminal Control

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // Enter raw mode
    Terminal::enterRawMode();
    
    // Clear screen
    Terminal::clearScreen();
    
    // Set color and output
    Terminal::setForegroundColor(Color::Green);
    Terminal::printLine("Hello, TUI!");
    Terminal::reset();
    
    // Formatted output
    Terminal::printFormat("Screen size: {}x{}\n", 
        Terminal::screenSize().width,
        Terminal::screenSize().height);
    
    // Move cursor
    Terminal::moveCursor(5, 10);
    Terminal::print("Position (5, 10)");
    
    // Read key
    Terminal::printLine("\nPress any key...");
    SP_Keys sp_key;
    uint8_t key = Terminal::getKey(&sp_key);
    
    Terminal::printFormat("Key: {} ({})", 
        getKeyName(key, sp_key),
        (int)key);
    
    // Exit raw mode
    Terminal::leaveRawMode();
    
    return 0;
}
```

### 8.2 Renderer Usage

```cpp
#include "TUI/TUI.hpp"

int main() {
    using namespace Tiny::TUI;
    
    // Get renderer instance
    auto& renderer = Renderer::self();
    
    // Set character
    renderer.set(0, 0, 'H');
    renderer.set(1, 0, "Hello");
    
    // Set style
    Renderer::Style style;
    style.fg_color = Color::Green;
    renderer.set(2, 0, "Green Text", style);
    
    // Draw border
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
    
    // Fill rectangle
    renderer.fillRect({6, 6}, {14, 24}, ' ');
    
    // Formatted output
    renderer.setStrF({7, 7}, "Count: {}", 42);
    
    // Present to screen
    renderer.present();
    
    // Wait for input
    Terminal::getKey();
    
    return 0;
}
```

### 8.3 Mouse Event Handling

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

### 8.4 Color and Style Example

```cpp
#include "TUI/TUI.hpp"

int main() {
    using namespace Tiny::TUI;
    
    Terminal::enterRawMode();
    Terminal::clearScreen();
    
    // ANSI 16 colors
    Terminal::setForegroundColor(Color::Red);
    Terminal::printLine("Red Text");
    
    Terminal::setForegroundColor(Color::Blue, true);  // Bright
    Terminal::printLine("Bright Blue Text");
    
    // RGB colors
    Terminal::setForegroundColor(255, 128, 0);  // Orange
    Terminal::setBackgroundColor(0, 0, 128);    // Dark blue background
    Terminal::printLine("Orange on Dark Blue");
    
    Terminal::reset();
    
    // Style combination
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

## 9. Notes

### 9.1 Raw Mode

- After entering raw mode, terminal will not automatically handle input/output
- Must manually handle Enter, Backspace, and other keys
- Must call `leaveRawMode()` before program exit
- Recommended to use RAII pattern to ensure terminal state restoration

### 9.2 Terminal Compatibility

- Requires terminal supporting ANSI escape sequences
- Windows 10+, modern Linux terminals, macOS Terminal are all supported
- Windows 7/8 may need to enable virtual terminal processing

### 9.3 Mouse Support

- Requires terminal supporting mouse events
- After enabling, mouse events are returned via `getKey()` as `KEY_SPECIAL`
- Then call `getMouseButton()` to get detailed information

### 9.4 Renderer Usage

- Uses double buffering mechanism, draws to buffer first
- Call `present()` to actually output to screen
- Call `clear()` to clear the front buffer before redrawing
- Terminal size changes are handled internally by `resizeEvent()`; use `setResizeEvent()` to register custom callbacks

### 9.5 UTF-8 Support

- Supports multi-byte character display
- Use `splitUTF8()` to process strings
- Note width calculation for full-width characters like Chinese

### 9.6 Performance Considerations

- Avoid frequent `present()` calls
- Batch drawing and present uniformly
- Use dirty marking to reduce unnecessary redraws
