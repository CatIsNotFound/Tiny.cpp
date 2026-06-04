# TUI Module

Namespace: `Tiny::TUI`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Enum Definitions](#4-enum-definitions)
5. [Color Class](#5-color-class)
6. [Style Class](#6-style-class)
7. [Canvas Class](#7-canvas-class)
8. [Usage Examples](#8-usage-examples)
9. [Notes](#9-notes)

---

## 1. Module Overview

The `TUI` module provides basic terminal user interface rendering functionality, containing three core classes:

- **Color Class**: Color representation and ANSI escape code generation
- **Style Class**: Text style (color, background, attributes) management
- **Canvas Class**: Terminal canvas, supports drawing characters, strings, borders, etc.

---

## 2. Header File

```cpp
#include "TUI/TUI.hpp"
```

---

## 3. Type Definitions

```cpp
using ColorValue = uint8_t;
```

**Description**: Color value type, 8-bit unsigned integer (0-255).

---

## 4. Enum Definitions

### 4.1 ColorType Enum

```cpp
enum class ColorType : uint8_t {
    Foreground = 38,  // Foreground color
    Background = 48   // Background color
};
```

| Enum Value | Value | Description |
|------------|-------|-------------|
| `Foreground` | 38 | Foreground color (text color) |
| `Background` | 48 | Background color |

### 4.2 TextStyle Enum

```cpp
enum class TextStyle : uint8_t {
    Reset      = 0,   // Reset all styles
    Bold       = 1,   // Bold
    Dim        = 2,   // Dim
    Italic     = 3,   // Italic
    Underline  = 4,   // Underline
    Blink      = 5,   // Blink
    Reverse    = 7,   // Reverse (swap foreground/background)
    Hidden     = 8,   // Hidden
    Strikethrough = 9  // Strikethrough
};
```

### 4.3 BorderStyle Enum

```cpp
enum class BorderStyle : uint8_t {
    None,     // No border
    Single,   // Single line border
    Double,   // Double line border
    Rounded,  // Rounded corners border
    Bold,     // Bold border
    Block     // Block border
};
```

| Enum Value | Description |
|------------|-------------|
| `None` | No border |
| `Single` | Single line border (┌─┐) |
| `Double` | Double line border (╔═╗) |
| `Rounded` | Rounded corners border (╭─╮) |
| `Bold` | Bold border (┏━┓) |
| `Block` | Block border (▛▀▜) |

---

## 5. Color Class

### 5.1 Class Overview

Color representation class, supports ANSI 256 colors and RGB colors.

### 5.2 Constructors

```cpp
Color(ColorValue value);                    // Construct from 256-color value
Color(uint8_t r, uint8_t g, uint8_t b);     // Construct from RGB values
Color(const Color& color);                  // Copy constructor
Color(Color&& color) noexcept;              // Move constructor
```

| Parameter | Type | Description | Range |
|-----------|------|-------------|-------|
| `value` | `ColorValue` | 256-color index | 0-255 |
| `r` | `uint8_t` | Red component | 0-255 |
| `g` | `uint8_t` | Green component | 0-255 |
| `b` | `uint8_t` | Blue component | 0-255 |

### 5.3 Static Member Functions

#### getColorCode

```cpp
static std::string getColorCode(ColorType color_type, ColorValue value);
static std::string getColorCode(ColorType color_type, uint8_t r, uint8_t g, uint8_t b);
```
- **Function**: Generate ANSI color escape code
- **Parameter**: 
  - `color_type` - Color type (Foreground/Background)
  - `value` / `r, g, b` - Color value
- **Return Value**: ANSI escape code string

### 5.4 Member Functions

#### setColor (overloads)

```cpp
void setColor(ColorValue value);
void setColor(uint8_t r, uint8_t g, uint8_t b);
```
- **Function**: Set color value
- **Parameter**: Color index or RGB components
- **Return Value**: None

#### colorCode

```cpp
[[nodiscard]] std::string colorCode(ColorType color_type) const;
```
- **Function**: Get ANSI escape code for specified color type
- **Parameter**: `color_type` - Foreground or background
- **Return Value**: Escape code string

#### value

```cpp
[[nodiscard]] ColorValue value() const;
```
- **Function**: Get color value
- **Return Value**: Color index (0-255)

#### r / g / b

```cpp
[[nodiscard]] uint8_t r() const;
[[nodiscard]] uint8_t g() const;
[[nodiscard]] uint8_t b() const;
```
- **Function**: Get RGB component values
- **Return Value**: Component value (0-255)

### 5.5 Static Color Constants

```cpp
static const Color Black;        // Black (0)
static const Color Red;          // Red (1)
static const Color Green;        // Green (2)
static const Color Yellow;       // Yellow (3)
static const Color Blue;         // Blue (4)
static const Color Magenta;      // Magenta (5)
static const Color Cyan;         // Cyan (6)
static const Color White;        // White (7)
static const Color Default;      // Default color
static const Color BrightBlack;  // Bright black (8)
static const Color BrightRed;    // Bright red (9)
static const Color BrightGreen;  // Bright green (10)
static const Color BrightYellow; // Bright yellow (11)
static const Color BrightBlue;   // Bright blue (12)
static const Color BrightMagenta;// Bright magenta (13)
static const Color BrightCyan;   // Bright cyan (14)
static const Color BrightWhite;  // Bright white (15)
```

---

## 6. Style Class

### 6.1 Class Overview

Text style management class, combines foreground color, background color, and text attributes.

### 6.2 Constructors

```cpp
Style();                                    // Default style
Style(const Color& foreground);             // Style with foreground color
Style(const Color& foreground, const Color& background);  // Style with foreground and background
Style(const Color& foreground, const Color& background, 
      const std::vector<TextStyle>& styles);  // Complete style
Style(const Style& style);                  // Copy constructor
Style(Style&& style) noexcept;              // Move constructor
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `foreground` | `Color` | Foreground color |
| `background` | `Color` | Background color |
| `styles` | `std::vector<TextStyle>` | Text style list |

### 6.3 Destructor

```cpp
~Style();
```

### 6.4 Member Functions

#### setForeground

```cpp
void setForeground(const Color& color);
```
- **Function**: Set foreground color
- **Parameter**: `color` - Foreground color
- **Return Value**: None

#### setBackground

```cpp
void setBackground(const Color& color);
```
- **Function**: Set background color
- **Parameter**: `color` - Background color
- **Return Value**: None

#### setStyles

```cpp
void setStyles(const std::vector<TextStyle>& styles);
```
- **Function**: Set text style list
- **Parameter**: `styles` - Style list
- **Return Value**: None

#### addStyle

```cpp
void addStyle(TextStyle style);
```
- **Function**: Add text style
- **Parameter**: `style` - Style to add
- **Return Value**: None

#### removeStyle

```cpp
void removeStyle(TextStyle style);
```
- **Function**: Remove text style
- **Parameter**: `style` - Style to remove
- **Return Value**: None

#### foreground / background

```cpp
[[nodiscard]] const Color& foreground() const;
[[nodiscard]] const Color& background() const;
```
- **Function**: Get foreground/background color
- **Return Value**: Constant reference to color

#### styles

```cpp
[[nodiscard]] const std::vector<TextStyle>& styles() const;
```
- **Function**: Get style list
- **Return Value**: Constant reference to style list

#### ansiCode

```cpp
[[nodiscard]] std::string ansiCode() const;
```
- **Function**: Generate complete ANSI escape code
- **Return Value**: Escape code string

### 6.5 Static Style Constants

```cpp
static const Style Default;       // Default style
static const Style Bold;          // Bold style
static const Style Underline;     // Underline style
static const Style Italic;        // Italic style
static const Style Strikethrough; // Strikethrough style
static const Style Blink;         // Blink style
static const Style Reverse;       // Reverse style
static const Style Hidden;        // Hidden style
static const Style Dim;           // Dim style
```

---

## 7. Canvas Class

### 7.1 Class Overview

Terminal canvas class, provides drawing characters, strings, borders, and other functionality.

### 7.2 Constructors

```cpp
Canvas();
Canvas(uint16_t width, uint16_t height);
Canvas(const Canvas& canvas);
Canvas(Canvas&& canvas) noexcept;
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `width` | `uint16_t` | Canvas width (characters) |
| `height` | `uint16_t` | Canvas height (characters) |

### 7.3 Destructor

```cpp
~Canvas();
```

### 7.4 Member Functions

#### setSize

```cpp
void setSize(uint16_t width, uint16_t height);
```
- **Function**: Set canvas size
- **Parameter**: `width` - Width, `height` - Height
- **Return Value**: None
- **Constraint**: Clears canvas content

#### resize

```cpp
void resize(uint16_t width, uint16_t height);
```
- **Function**: Resize canvas (preserves existing content)
- **Parameter**: `width` - New width, `height` - New height
- **Return Value**: None

#### width / height

```cpp
[[nodiscard]] uint16_t width() const;
[[nodiscard]] uint16_t height() const;
```
- **Function**: Get canvas width/height
- **Return Value**: Size value

#### clear

```cpp
void clear();
```
- **Function**: Clear canvas content
- **Return Value**: None

#### fill

```cpp
void fill(char c);
void fill(char c, const Style& style);
```
- **Function**: Fill entire canvas with specified character
- **Parameter**: 
  - `c` - Fill character
  - `style` - Fill style (optional)
- **Return Value**: None

#### drawChar

```cpp
void drawChar(uint16_t x, uint16_t y, char c);
void drawChar(uint16_t x, uint16_t y, char c, const Style& style);
```
- **Function**: Draw character at specified position
- **Parameter**: 
  - `x, y` - Coordinates (0-based)
  - `c` - Character
  - `style` - Style (optional)
- **Return Value**: None
- **Constraint**: Does nothing if coordinates out of bounds

#### drawString

```cpp
void drawString(uint16_t x, uint16_t y, const std::string& str);
void drawString(uint16_t x, uint16_t y, const std::string& str, const Style& style);
```
- **Function**: Draw string at specified position
- **Parameter**: 
  - `x, y` - Starting coordinates
  - `str` - String content
  - `style` - Style (optional)
- **Return Value**: None
- **Constraint**: Truncates if string exceeds canvas boundary

#### drawHorizontalLine

```cpp
void drawHorizontalLine(uint16_t x, uint16_t y, uint16_t length);
void drawHorizontalLine(uint16_t x, uint16_t y, uint16_t length, const Style& style);
```
- **Function**: Draw horizontal line
- **Parameter**: 
  - `x, y` - Starting coordinates
  - `length` - Line length
  - `style` - Style (optional)
- **Return Value**: None

#### drawVerticalLine

```cpp
void drawVerticalLine(uint16_t x, uint16_t y, uint16_t length);
void drawVerticalLine(uint16_t x, uint16_t y, uint16_t length, const Style& style);
```
- **Function**: Draw vertical line
- **Parameter**: 
  - `x, y` - Starting coordinates
  - `length` - Line length
  - `style` - Style (optional)
- **Return Value**: None

#### drawBox

```cpp
void drawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
             BorderStyle border_style);
void drawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
             BorderStyle border_style, const Style& style);
```
- **Function**: Draw rectangular box
- **Parameter**: 
  - `x, y` - Top-left corner coordinates
  - `width, height` - Box dimensions
  - `border_style` - Border style (default Single)
  - `style` - Style (optional)
- **Return Value**: None

#### drawFilledBox

```cpp
void drawFilledBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawFilledBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
                   char fill_char);
void drawFilledBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
                   char fill_char, const Style& style);
```
- **Function**: Draw filled rectangular box
- **Parameter**: 
  - `x, y` - Top-left corner coordinates
  - `width, height` - Box dimensions
  - `fill_char` - Fill character (default space)
  - `style` - Style (optional)
- **Return Value**: None

#### render

```cpp
void render() const;
```
- **Function**: Render canvas content to terminal
- **Return Value**: None
- **Output**: ANSI escape code formatted text

### 7.5 Static Member Functions

#### moveCursor

```cpp
static std::string moveCursor(uint16_t x, uint16_t y);
```
- **Function**: Generate cursor movement escape code
- **Parameter**: `x, y` - Target coordinates (1-based)
- **Return Value**: Escape code string

#### clearScreen

```cpp
static std::string clearScreen();
```
- **Function**: Generate screen clear escape code
- **Return Value**: Escape code string

#### clearLine

```cpp
static std::string clearLine();
```
- **Function**: Generate line clear escape code
- **Return Value**: Escape code string

#### hideCursor / showCursor

```cpp
static std::string hideCursor();
static std::string showCursor();
```
- **Function**: Generate cursor hide/show escape codes
- **Return Value**: Escape code string

#### setWindowTitle

```cpp
static std::string setWindowTitle(const std::string& title);
```
- **Function**: Generate window title setting escape code
- **Parameter**: `title` - Window title
- **Return Value**: Escape code string

---

## 8. Usage Examples

### 8.1 Basic Color Usage Example

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // Use predefined colors
    std::cout << Color::Red.colorCode(ColorType::Foreground) 
              << "Red text" << Style::Default.ansiCode() << std::endl;
    
    // Use RGB colors
    Color custom(255, 128, 0);  // Orange
    std::cout << custom.colorCode(ColorType::Foreground) 
              << "Orange text" << Style::Default.ansiCode() << std::endl;
    
    // Use 256 colors
    Color purple(93);  // Purple
    std::cout << purple.colorCode(ColorType::Foreground) 
              << "Purple text" << Style::Default.ansiCode() << std::endl;
    
    return 0;
}
```

### 8.2 Style Usage Example

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // Create custom style
    Style myStyle(Color::White, Color::Blue, {TextStyle::Bold});
    std::cout << myStyle.ansiCode() << "Bold white text on blue background" 
              << Style::Default.ansiCode() << std::endl;
    
    // Chain styles
    std::cout << Style(Color::Green).ansiCode() << "Green text"
              << Style::Bold.ansiCode() << " and bold"
              << Style::Underline.ansiCode() << " and underlined"
              << Style::Default.ansiCode() << std::endl;
    
    // Use static styles
    std::cout << Style::Bold.ansiCode() << "Bold only"
              << Style::Default.ansiCode() << std::endl;
    
    return 0;
}
```

### 8.3 Canvas Basic Usage Example

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // Create canvas
    Canvas canvas(40, 10);
    
    // Fill background
    canvas.fill(' ', Style(Color::Black, Color::Black));
    
    // Draw border
    canvas.drawBox(0, 0, 40, 10, BorderStyle::Single);
    
    // Draw title
    canvas.drawString(12, 1, "Terminal UI Demo", 
                      Style(Color::Yellow, Color::Black, {TextStyle::Bold}));
    
    // Draw content
    canvas.drawString(2, 3, "This is a simple TUI example.");
    canvas.drawString(2, 5, "Features:");
    canvas.drawString(4, 6, "- Color support");
    canvas.drawString(4, 7, "- Border styles");
    canvas.drawString(4, 8, "- Text styling");
    
    // Draw button
    canvas.drawFilledBox(15, 8, 10, 1, ' ', 
                         Style(Color::White, Color::Blue));
    canvas.drawString(17, 8, "[ OK ]", 
                      Style(Color::White, Color::Blue, {TextStyle::Bold}));
    
    // Render
    canvas.render();
    
    return 0;
}
```

### 8.4 Complex UI Example

```cpp
#include "TUI/TUI.hpp"
#include <iostream>
#include <vector>

int main() {
    using namespace Tiny::TUI;
    
    Canvas canvas(60, 15);
    
    // Clear screen
    std::cout << Canvas::clearScreen();
    
    // Draw main window
    canvas.drawBox(0, 0, 60, 15, BorderStyle::Double);
    canvas.drawString(20, 1, "System Monitor", 
                      Style(Color::Cyan, Color::Black, {TextStyle::Bold}));
    
    // Draw separator line
    canvas.drawHorizontalLine(1, 2, 58, Style(Color::Gray));
    
    // Draw info panel
    canvas.drawBox(2, 3, 25, 5, BorderStyle::Single);
    canvas.drawString(4, 4, "CPU: 45%", Style(Color::Green));
    canvas.drawString(4, 5, "RAM: 2.1GB / 8GB", Style(Color::Yellow));
    canvas.drawString(4, 6, "Disk: 120GB / 500GB", Style(Color::Blue));
    
    // Draw progress bar
    canvas.drawString(30, 4, "CPU Usage:");
    canvas.drawBox(30, 5, 20, 1, BorderStyle::Single);
    canvas.drawFilledBox(31, 5, 9, 1, '#', Style(Color::Green, Color::Green));
    
    // Draw menu
    std::vector<std::string> menu = {"Process", "Network", "Disk", "Settings"};
    for (size_t i = 0; i < menu.size(); ++i) {
        if (i == 0) {
            canvas.drawFilledBox(30, 7 + i, 15, 1, ' ', 
                                 Style(Color::Black, Color::White));
            canvas.drawString(32, 7 + i, menu[i], 
                              Style(Color::Black, Color::White, {TextStyle::Bold}));
        } else {
            canvas.drawString(32, 7 + i, menu[i]);
        }
    }
    
    // Draw status bar
    canvas.drawFilledBox(1, 13, 58, 1, ' ', 
                         Style(Color::White, Color::Blue));
    canvas.drawString(2, 13, " F1:Help  F2:Refresh  F10:Exit", 
                      Style(Color::White, Color::Blue));
    
    // Render
    canvas.render();
    
    return 0;
}
```

### 8.5 Direct Escape Code Usage Example

```cpp
#include "TUI/TUI.hpp"
#include <iostream>

int main() {
    using namespace Tiny::TUI;
    
    // Hide cursor
    std::cout << Canvas::hideCursor();
    
    // Clear screen
    std::cout << Canvas::clearScreen();
    
    // Set window title
    std::cout << Canvas::setWindowTitle("My TUI App");
    
    // Move cursor and output
    std::cout << Canvas::moveCursor(10, 5) << "Position (10, 5)";
    std::cout << Canvas::moveCursor(10, 6) << "Position (10, 6)";
    
    // Show cursor
    std::cout << Canvas::showCursor();
    
    return 0;
}
```

---

## 9. Notes

### 9.1 Terminal Support

- Modern terminals (Windows Terminal, iTerm2, GNOME Terminal) fully support
- Windows cmd.exe has limited support (requires Windows 10+)
- Some terminals may not support certain styles (e.g., Blink)

### 9.2 Coordinate System

- Canvas uses 0-based coordinates (top-left is 0, 0)
- `moveCursor` uses 1-based coordinates (compatibility with ANSI standard)
- Coordinates out of bounds are automatically ignored

### 9.3 Performance Considerations

- Canvas `render()` generates complete escape code string
- Frequent rendering may cause screen flicker
- Recommended to render at 30fps or below

### 9.4 Color Limitations

- 256 colors: index 0-15 are standard colors, 16-231 are color cube, 232-255 are grayscale
- RGB colors: requires terminal supporting true color (24-bit)
- If terminal doesn't support, colors automatically degrade

### 9.5 Escape Code Reference

| Code | Function |
|------|----------|
| `\033[0m` | Reset all styles |
| `\033[38;5;Nm` | Set foreground to 256-color N |
| `\033[48;5;Nm` | Set background to 256-color N |
| `\033[38;2;R;G;Bm` | Set foreground to RGB color |
| `\033[48;2;R;G;Bm` | Set background to RGB color |
| `\033[H` | Move cursor to top-left |
| `\033[2J` | Clear screen |
| `\033[K` | Clear to end of line |
| `\033[?25l` | Hide cursor |
| `\033[?25h` | Show cursor |
| `\033]0;Title\007` | Set window title |

### 9.6 Best Practices

- Always reset styles after colored output
- Use Canvas for complex UI, direct escape codes for simple output
- Hide cursor during animation to avoid flicker
- Test terminal support before using advanced features
