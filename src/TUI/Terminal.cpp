/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#include "Terminal.hpp"

#if defined(TINY_CPP_MY_OS_WINDOWS)
#include <windows.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <cstring>
#include <cwchar>
#include <clocale>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

#if defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_DEFINED_WIN)
#define TINY_CPP_DEFINED_WIN
namespace Tiny {
    std::string Win::convertPath(const std::string& path) {
        std::string _new_path;
        for (auto c : path) {
            _new_path.push_back(c == '/' ? '\\' : c);
        }
        return _new_path;
    }

    std::wstring Win::string2Wide(const std::string& str, uint32_t codepage) {
        auto len = MultiByteToWideChar(codepage, 0, str.data(),
        str.size(), nullptr, 0);
        std::wstring w_str(len, 0);
        MultiByteToWideChar(codepage, 0, str.data(), str.size(),
            &w_str[0], len);
        return w_str;
    }

    std::string Win::wide2String(const std::wstring& w_str, uint32_t codepage) {
        auto len = WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            &str[0], len, nullptr, nullptr);
        return str;
    }
}
#endif

#if defined(TINY_CPP_MY_OS_UNIX) && !defined(TINY_CPP_DEFINED_UNIX)
#define TINY_CPP_DEFINED_UNIX
namespace Tiny {
    std::wstring Win::string2Wide(const std::string& str) {
        std::mbstate_t state{};
        auto len = std::mbsrtowcs(nullptr, &str.data(), 0, &state);
        std::wstring w_str(len, 0);
        mbsrtowcs(&w_str[0], str.data(), w_str.size(), &state);
        return w_str;
    }

    std::string Win::wide2String(const std::wstring& w_str) {
        std::mbstate_t state{};
        auto len = std::wcsrtombs(nullptr, &w_str.data(), 0, &state);
        if (len == -1) return {};
        std::string str(len, 0);
        std::wcsrtombs(&str[0], &w_str.data(), len, &state);
        return str;
    }
}
#endif

namespace Tiny {
#ifdef TINY_CPP_MY_OS_UNIX
    struct termios TUI::Terminal::_old_terminal{};
    bool TUI::Terminal::_is_in_raw_mode{};
#elif defined(TINY_CPP_MY_OS_WINDOWS)
    void* TUI::Terminal::_old_console{};
    unsigned long TUI::Terminal::_old_console_handle{0};
#endif

    bool TUI::Terminal::enterRawMode() {
#ifdef TINY_CPP_MY_OS_UNIX
        struct termios raw;
        tcgetattr(STDIN_FILENO, &raw);
        struct termios new_raw = raw;
        _old_terminal = raw;
        new_raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
        new_raw.c_iflag &= ~(IXON | BRKINT | INPCK | PARMRK);
        new_raw.c_oflag &= ~(OPOST);
        new_raw.c_cflag |= CS8;
        new_raw.c_cc[VMIN] = 1;
        new_raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCIFLUSH, &new_raw);
        auto cmd = "\x1b[?1049h\x1b[2J\x1b[H";
        write(STDOUT_FILENO, cmd, strlen(cmd));
        _is_in_raw_mode = true;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        _old_console = console;
        auto new_console = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            CONSOLE_TEXTMODE_BUFFER,
            nullptr
        );
        if (new_console == INVALID_HANDLE_VALUE || !SetConsoleActiveScreenBuffer(new_console)) return false;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return false;
        _old_console_handle = mode;
        mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(console, mode)) return false;
        SetStdHandle(STD_OUTPUT_HANDLE, new_console);
        SetStdHandle(STD_ERROR_HANDLE, new_console);
#endif
        return true;
    }

    bool TUI::Terminal::leaveRawMode() {
#ifdef TINY_CPP_MY_OS_UNIX
        if (_is_in_raw_mode && tcsetattr(STDIN_FILENO, TCSAFLUSH, &_old_terminal) == -1) {
            return false;
        }
        _is_in_raw_mode = false;
        auto cmd = "\x1b[?1049l";
        write(STDOUT_FILENO, cmd, strlen(cmd));
        
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        if (_old_console != nullptr) {
            auto console = GetStdHandle(STD_OUTPUT_HANDLE);
            if (console == INVALID_HANDLE_VALUE) return false;
            if (!SetConsoleActiveScreenBuffer(_old_console)) return false;
            SetStdHandle(STD_OUTPUT_HANDLE, _old_console);
            SetStdHandle(STD_ERROR_HANDLE, _old_console);
            CloseHandle(console);
            SetConsoleMode(_old_console, _old_console_handle);
            _old_console = nullptr;
        }
#endif
        return true;
    }

    TUI::Size TUI::Terminal::screenSize() {
        Size size;
#ifdef TINY_CPP_MY_OS_UNIX
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return size;
        size.width = w.ws_col;
        size.height = w.ws_row;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return size;
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(console, &info)) return size;
        size.width = info.dwSize.X;
        size.height = info.dwSize.Y;
#endif
        return size;
    }

    TUI::Position TUI::Terminal::cursorPosition() {
        Position position;
#ifdef TINY_CPP_MY_OS_UNIX
        struct termios raw, original;

        if (tcgetattr(STDIN_FILENO, &original) == -1) return position;
        raw = original;
        raw.c_lflag &= ~(ECHO | ICANON);
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return position;
        if (write(STDOUT_FILENO, "\x1b[6n", 4) == -1) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
            return position;
        }
        char buf[32] = {};
        int i = 0;
        while (i < sizeof(buf) - 1) {
            if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
            if (buf[i] == 'R') {
                buf[i + 1] = '\0';
                break;
            }
            i++;
        }

        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);

        if (buf[0] != '\x1b' || buf[1] != '[') return position;
        if (sscanf(&buf[2], "%d;%d", &position.row, &position.column) != 2) return {};
        position.row -= 1;
        position.column -= 1;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return position;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return position;
        position.row = info.dwCursorPosition.Y;
        position.column = info.dwCursorPosition.X;
#endif
        return position;
    }

    bool TUI::Terminal::print(const std::string &text) {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, text.c_str(), text.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = Win::string2Wide(text);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.length(), nullptr, nullptr)) {
            return false;
        }
#endif
        return true;
    }

    bool TUI::Terminal::printLine(const std::string &text) {
#ifdef TINY_CPP_MY_OS_UNIX
        auto cmd = text + "\x1b[E";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = Win::string2Wide(text);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) {
            return false;
        }
        auto pos = cursorPosition();
        moveCursor(pos.row + 1, 0);
#endif
        return true;
    }

    bool TUI::Terminal::clearScreen() {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, "\x1b[2J\x1b[H", 8);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return false;
        auto size = info.dwSize.X * info.dwSize.Y;
        DWORD written{};
        if (!FillConsoleOutputCharacterW(console, L' ', size, {0, 0}, &written))
            return false;
        if (!SetConsoleCursorPosition(console, {0, 0})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::clearInRow(uint8_t row) {
#ifdef TINY_CPP_MY_OS_UNIX
        auto old_row = cursorPosition().row;
        auto cmd = "\x1b[2K\x1b[" + std::to_string(row) + ";0H\x1b[" + std::to_string(old_row) + ";0H";
        write(STDOUT_FILENO, cmd.data(), cmd.size());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return false;
        auto size = info.dwSize.X;
        DWORD written{};
        if (!FillConsoleOutputCharacterA(console, ' ', size, {0, row}, &written))
            return false;
        if (!SetConsoleCursorPosition(console, {0, row})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::moveCursor(Position position) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[" + std::to_string(position.row + 1) + ";" + std::to_string(position.column + 1) + "H";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        if (!SetConsoleCursorPosition(console, {static_cast<short>(position.column), static_cast<short>(position.row)})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::moveCursor(uint32_t row, uint32_t column) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[" + std::to_string(row + 1) + ";" + std::to_string(column + 1) + "H";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        if (!SetConsoleCursorPosition(console, {static_cast<short>(column), static_cast<short>(row)})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::flushScreen() {
#ifdef TINY_CPP_MY_OS_UNIX
        return fsync(STDOUT_FILENO) != -1;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto handler = GetStdHandle(STD_OUTPUT_HANDLE);
        if (handler == INVALID_HANDLE_VALUE) return false;
        return FlushFileBuffers(handler);
#endif
        return true;
    }

    std::string TUI::Terminal::readLine() {
        std::string out;
#ifdef TINY_CPP_MY_OS_UNIX

#elif defined(TINY_CPP_MY_OS_WINDOWS)
        char buffer[2048] = {};
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return out;
        if (!ReadConsole(console, buffer, 2048, &read_count, nullptr)) return out;
        out.resize(read_count - 2);                // Removed "\r\n"
        strncpy(&out[0], buffer, read_count - 2);
#endif
        return out;
    }

    std::wstring TUI::Terminal::readLineW() {
        std::wstring out;
#ifdef TINY_CPP_MY_OS_UNIX

#elif defined(TINY_CPP_MY_OS_WINDOWS)
        wchar_t buffer[2048] = {};
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return out;
        if (!ReadConsoleW(console, buffer, 2048, &read_count, nullptr)) return out;
        out.resize(read_count - 2);
        wcsncpy(&out[0], buffer, read_count - 2);
#endif
        return out;
    }

    void TUI::Terminal::setBackgroundColor(Color color, bool intensity) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd;
        if (intensity) {
            cmd = "\x1b[10" + std::to_string(static_cast<uint8_t>(color)) + "m";
        } else {
            cmd = "\x1b[22m\x1b[4" + std::to_string(static_cast<uint8_t>(color)) + "m";
        }
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());     
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        auto real_color = info.wAttributes;
        real_color &= ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
        switch (color) {
            case Color::Black:
                real_color |= 0;
                break;
            case Color::Red:
                real_color |= BACKGROUND_RED;
                break;
            case Color::Green:
                real_color |= BACKGROUND_GREEN;
                break;
            case Color::Yellow:
                real_color |= BACKGROUND_RED | BACKGROUND_GREEN;
                break;
            case Color::Blue:
                real_color |= BACKGROUND_BLUE;
                break;
            case Color::Magenta:
                real_color |= BACKGROUND_RED | BACKGROUND_BLUE;
                break;
            case Color::Cyan:
                real_color |= BACKGROUND_GREEN | BACKGROUND_BLUE;
                break;
            case Color::White:
                real_color |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                break;
            default:
                real_color |= 0;
                break;
        }
        if (intensity) real_color |= BACKGROUND_INTENSITY;
        SetConsoleTextAttribute(console, real_color);
#endif

    }

        void TUI::Terminal::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
        std::string cmd = "\x1b[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
            WriteConsole(console, cmd.c_str(), cmd.length(), nullptr, nullptr);
        }
#endif

    }

    void TUI::Terminal::setForegroundColor(Color color, bool intensity) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd;
        if (intensity) {
            cmd = "\x1b[9" + std::to_string(static_cast<uint8_t>(color)) + "m";
        } else {
            cmd = "\x1b[3" + std::to_string(static_cast<uint8_t>(color)) + "m";
        }
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        auto real_color = info.wAttributes;
        real_color &= ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        switch (color) {
            case Color::Black:
                real_color |= 0;
                break;
            case Color::Red:
                real_color |= FOREGROUND_RED;
                break;
            case Color::Green:
                real_color |= FOREGROUND_GREEN;
                break;
            case Color::Yellow:
                real_color |= FOREGROUND_RED | FOREGROUND_GREEN;
                break;
            case Color::Blue:
                real_color |= FOREGROUND_BLUE;
                break;
            case Color::Magenta:
                real_color |= FOREGROUND_RED | FOREGROUND_BLUE;
                break;
            case Color::Cyan:
                real_color |= FOREGROUND_RED | FOREGROUND_GREEN;
                break;
            case Color::White:
                real_color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            default:
                real_color |= 0;
                break;
        }
        if (intensity) real_color |= FOREGROUND_INTENSITY;
        SetConsoleTextAttribute(console, real_color);
#endif
    }

    void TUI::Terminal::setForegroundColor(uint8_t r, uint8_t g, uint8_t b) {
        std::string cmd = "\x1b[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
            WriteConsole(console, cmd.c_str(), cmd.length(), nullptr, nullptr);
        }
#endif

    }

    void TUI::Terminal::setBolder(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[1m" : "\x1b[22m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[1m" : L"\x1b[22m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::setDark(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[2m" : "\x1b[22m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[2m" : L"\x1b[22m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::setItalic(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[3m" : "\x1b[23m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[3m" : L"\x1b[23m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::setUnderline(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[4m" : "\x1b[24m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[4m" : L"\x1b[24m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::setBlinking(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[5m" : "\x1b[25m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[5m" : L"\x1b[25m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::reverseColor(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[7m" : "\x1b[27m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[7m" : L"\x1b[27m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::setCursorVisible(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[?25h" : "\x1b[?25l";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        CONSOLE_CURSOR_INFO cur_info;
        if (!GetConsoleCursorInfo(console, &cur_info)) return;
        cur_info.bVisible = enable;
        SetConsoleCursorInfo(console, &cur_info);
#endif
    }

    void TUI::Terminal::setStrikethrough(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[9m" : "\x1b[29m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return;
        if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return;
        std::wstring cmd = enable ? L"\x1b[9m" : L"\x1b[29m";
        WriteConsoleW(console, cmd.c_str(), cmd.size(), nullptr, nullptr);
#endif
    }

    void TUI::Terminal::reset() {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[0m";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif
    }

    bool TUI::Terminal::printFormattedText(const std::string &str) {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, str.data(), str.size());
        return true;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = Win::string2Wide(str);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) return false;
        return true;
#endif
    }
}

/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/
