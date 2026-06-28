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
#include "../OS/File.hpp"

#if defined(TINY_CPP_MY_OS_WINDOWS)
#include <windows.h>
#include <conio.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <cstring>
#include <cwchar>
#include <clocale>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#ifdef TINY_CPP_USE_GPM
#include <gpm.h>
#define HAS_GPM
#define NEED_USE_GPM() !strcmp(getenv("TERM"), "linux")
#endif
#endif

#if defined(TINY_CPP_MY_OS_WINDOWS)
namespace {
    std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001) {
        auto len = MultiByteToWideChar(codepage, 0, str.data(),
        str.size(), nullptr, 0);
        std::wstring w_str(len, 0);
        MultiByteToWideChar(codepage, 0, str.data(), str.size(),
            &w_str[0], len);
        return w_str;
    }

    std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001) {
        auto len = WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            &str[0], len, nullptr, nullptr);
        return str;
    }
}
#elif defined(TINY_CPP_MY_OS_UNIX)
namespace {
    std::wstring string2Wide(const std::string& str) {
        setlocale(LC_ALL, "");
        std::mbstate_t state{};
        auto data = str.data();
        auto len = std::mbsrtowcs(nullptr, &data, 0, &state);
        if (len == (size_t)-1) return {};
        std::wstring w_str(len, 0);
        std::mbsrtowcs(&w_str[0], &data, w_str.size(), &state);
        return w_str;
    }

    std::string wide2String(const std::wstring& w_str) {
        setlocale(LC_ALL, "");
        std::mbstate_t state{};
        auto data = w_str.data();
        auto len = std::wcsrtombs(nullptr, &data, 0, &state);
        if (len == -1) return {};
        std::string str(len, 0);
        std::wcsrtombs(&str[0], &data, len, &state);
        return str;
    }
}
#endif

namespace Tiny {
    const char* TUI::getKeyName(const uint8_t &KEY, const SP_Keys &SP) {
        if (KEY >= 32 && KEY <= 126) {
            static std::string out = "";
            out = std::string(1, KEY);
            return out.c_str();
        } else if (KEY == KEY_SPECIAL) {
            switch (SP) {
            case SP_KEY_F1:
                return "F1";
            case SP_KEY_F2:
                return "F2";
            case SP_KEY_F3:
                return "F3";
            case SP_KEY_F4:
                return "F4";
            case SP_KEY_F5:
                return "F5";
            case SP_KEY_F6:
                return "F6";
            case SP_KEY_F7:
                return "F7";
            case SP_KEY_F8:
                return "F8";
            case SP_KEY_F9:
                return "F9";
            case SP_KEY_F10:
                return "F10";
            case SP_KEY_F11:
                return "F11";
            case SP_KEY_F12:
                return "F12";
            case SP_KEY_INSERT:
                return "Insert";
            case SP_KEY_DELETE:
                return "Delete";
            case SP_KEY_HOME:
                return "Home";
            case SP_KEY_END:
                return "End";
            case SP_KEY_PAGE_UP:
                return "PgUp";
            case SP_KEY_PAGE_DOWN:
                return "PgDn";
            case SP_KEY_CENTER:
                return "Center";
            case SP_KEY_UP:
                return "Up";
            case SP_KEY_LEFT:
                return "Left";
            case SP_KEY_DOWN:
                return "Down";
            case SP_KEY_RIGHT:
                return "Right";
            case SP_KEY_PRINTSCR:
                return "PrintScreen";
            case SP_KEY_CTRL:
                return "Control";
            case SP_KEY_ALT:
                return "Alter";
            case SP_KEY_SHIFT:
                return "Shift";
            case SP_KEY_CAPSLOCK:
                return "CapsLock";
            case SP_KEY_NUMLOCK:
                return "NumLock";
            case SP_KEY_SCROLLLOCK:
                return "ScrollLock";
            default:
                return "Unknown";
            }
        } else if (KEY == KEY_UNKNOWN) {
            return "Unknown";
        } else if (KEY == KEY_NULL) {
            return "NULL";
        } else if (KEY == KEY_SOH) {
            return "SOH";
        } else if (KEY == KEY_STX) {
            return "STX";
        } else if (KEY == KEY_ETX) {
            return "ETX";
        } else if (KEY == KEY_EOT) {
            return "EOT";
        } else if (KEY == KEY_ENQ) {
            return "ENQ";
        } else if (KEY == KEY_ACK) {
            return "ACK";
        } else if (KEY == KEY_BELL) {
            return "BEL";
        } else if (KEY == KEY_BK) {
            return "BK";
        } else if (KEY == KEY_TAB) {
            return "TAB";
        } else if (KEY == KEY_LF) {
            return "LF";
        } else if (KEY == KEY_VT) {
            return "VT";
        } else if (KEY == KEY_FF) {
            return "FF";
        } else if (KEY == KEY_CR) {
            return "CR";
        } else if (KEY == KEY_SO) {
            return "SO";
        } else if (KEY == KEY_SI) {
            return "SI";
        } else if (KEY == KEY_DLE) {
            return "DLE";
        } else if (KEY == KEY_DC1) {
            return "DC1";
        } else if (KEY == KEY_DC2) {
            return "DC2";
        } else if (KEY == KEY_DC3) {
            return "DC3";
        } else if (KEY == KEY_DC4) {
            return "DC4";
        } else if (KEY == KEY_NAK) {
            return "NAK";
        } else if (KEY == KEY_SYN) {
            return "SYN";
        } else if (KEY == KEY_ETB) {
            return "ETB";
        } else if (KEY == KEY_CAN) {
            return "CAN";
        } else if (KEY == KEY_ESC) {
            return "ESC";
        } else if (KEY == KEY_FS) {
            return "FS";
        } else if (KEY == KEY_GS) {
            return "GS";
        } else if (KEY == KEY_RS) {
            return "RS";
        } else if (KEY == KEY_US) {
            return "US";
        } else if (KEY == KEY_DEL) {
            return "DEL";
        } else {
            return "Unknown";
        }
    }

    const char *TUI::getMouseName(const SP_Mouse &SP) {
        switch (SP) {
            case SP_MOUSE_LEFT_BUTTON:
                return "Left Button";
            case SP_MOUSE_MIDDLE_BUTTON:
                return "Middle Button";
            case SP_MOUSE_RIGHT_BUTTON:
                return "Right Button";
            case SP_MOUSE_WHEEL_UP:
                return "Mouse Wheel Up";
            case SP_MOUSE_WHEEL_DOWN:
                return "Mouse Wheel Down";
            case SP_MOUSE_MOVED:
                return "Mouse Moved";
            case SP_MOUSE_RELEASE:
                return "Mouse Release";
            default:
                return "Unknown";
        }
    }

    int8_t TUI::comparePosition(const TUI::Position &pos1, const TUI::Position &pos2) {
        if (pos1.row < pos2.row) return 1;
        if (pos1.row == pos2.row) {
            return (pos1.column < pos2.column) ? 1 : ((pos1.column > pos2.column) ? -1 : 0);
        }
        return -1;
    }

    int8_t TUI::compareSize(const TUI::Size &size1, const TUI::Size &size2) {
        if (size1.height < size2.height) return 1;
        if (size1.height == size2.height) {
            return (size1.width < size2.width) ? 1 : ((size1.width > size2.width) ? -1 : 0);
        }
        return -1;
    }

    bool TUI::isPointInRect(const Position &point, Position &start_pos, Position &end_pos) {
        if (comparePosition(start_pos, end_pos) == -1) {
            auto t = start_pos;
            start_pos = end_pos;
            end_pos = t;
        }
        return (point.row >= start_pos.row && point.row <= end_pos.row) && 
               (point.column >= start_pos.column && point.column <= end_pos.column);
    }

#ifdef TINY_CPP_MY_OS_UNIX
    termios TUI::Terminal::_old_terminal{};
    bool TUI::Terminal::_is_in_raw_mode{};
    TUI::Position TUI::Terminal::_last_cur_pos{0, 0};
    std::string TUI::Terminal::_temp_buffers{};
#ifdef TINY_CPP_USE_GPM
    Gpm_Connect TUI::Terminal::_gpm_connector{};
    int TUI::Terminal::_gpm_fd{};
#endif
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
        cfmakeraw(&new_raw);
        if (tcsetattr(STDIN_FILENO, TCIFLUSH, &new_raw) == -1) {
            _is_in_raw_mode = false;
            return false;
        }
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
        mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE);
#ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
#endif
        // SetConsoleCtrlHandler(nullptr, TRUE);
        if (!SetConsoleMode(new_console, mode)) return false;
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
#ifdef TINY_CPP_USE_GPM
        if (NEED_USE_GPM()) {
            auto cmd = "\x1b[127E";
            write(STDOUT_FILENO, cmd, strlen(cmd));
            return true;
        }
#endif
        auto cmd = "\x1b[?1049l";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        if (_old_console != nullptr) {
            auto console = GetStdHandle(STD_OUTPUT_HANDLE);
            if (console == INVALID_HANDLE_VALUE) return false;
            if (!SetConsoleActiveScreenBuffer(_old_console)) return false;
            SetStdHandle(STD_OUTPUT_HANDLE, _old_console);
            SetStdHandle(STD_ERROR_HANDLE, _old_console);
            // SetConsoleCtrlHandler(nullptr, FALSE);
            CloseHandle(console);
            SetConsoleMode(_old_console, _old_console_handle);
            _old_console = nullptr;
        }
#endif
        return true;
    }

    bool TUI::Terminal::isInRawMode() {
#ifdef TINY_CPP_MY_OS_UNIX
        struct termios raw;
        tcgetattr(STDIN_FILENO, &raw);
        _is_in_raw_mode = (raw.c_lflag & (ICANON | ECHO | ISIG | IEXTEN)) == 0;
        return _is_in_raw_mode;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console != INVALID_HANDLE_VALUE) return false;
        DWORD mode = 0;
        if (!GetConsoleMode(console, &mode)) return 0;
        return !(mode & (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT));
#endif
    }

    TUI::Size TUI::Terminal::screenSize() {
        Size size{};
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
        Position position{};
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
        size_t read_bytes = readAfterDelay(STDIN_FILENO, &buf, 32);
        if (read_bytes == -1) return _last_cur_pos;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);

        if (sscanf(buf, "\x1b[%d;%dR", &position.row, &position.column) != 2) {
            return _last_cur_pos;
        }
        position.row -= 1;
        position.column -= 1;
        _last_cur_pos = position;
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

    bool TUI::Terminal::print(char ch) {
        char text[] = {ch, '\0'};
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, text, 1);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;

        if (!WriteConsoleA(console, text, 1, nullptr, nullptr)) {
            return false;
        }
#endif
        return true;
    }

    bool TUI::Terminal::print(const std::string &text) {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, text.c_str(), text.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = string2Wide(text);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.length(), nullptr, nullptr)) {
            return false;
        }
#endif
        return true;
    }

    bool TUI::Terminal::printLine(const std::string &text) {
#ifdef TINY_CPP_MY_OS_UNIX
        auto cmd = text + "\r\n";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = string2Wide(text + "\r\n");
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) {
            return false;
        }
#endif
        return true;
    }

    bool TUI::Terminal::clearScreen() {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, "\x1b[2J", 5);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return false;
        auto size = info.dwSize.X * info.dwSize.Y;
        DWORD written{};
        if (!FillConsoleOutputCharacterW(console, L' ', size, {0, 0}, &written))
            return false;
#endif
        return true;
    }

    bool TUI::Terminal::clearInRow(uint8_t row) {
#ifdef TINY_CPP_MY_OS_UNIX
        auto old_row = cursorPosition().row;
        auto cmd = "\x1b[" + std::to_string(row + 1) + ";0H\x1b[2K\x1b[" + std::to_string(old_row + 1) + ";0H";
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
        auto scr = screenSize();
        if (position.column >= scr.width) position.column = scr.width - 1;
        if (position.row >= scr.height) position.row = scr.height - 1;
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
        auto scr = screenSize();
        if (column >= scr.width) column = scr.width - 1;
        if (row >= scr.height) row = scr.height - 1;
        if (!SetConsoleCursorPosition(console, {static_cast<short>(column), static_cast<short>(row)})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::setScrollRegion(uint32_t row_start, uint32_t row_end) {
        std::string cmd = "\x1b[" + std::to_string(row_start + 1) + ";" + std::to_string(row_end + 1) + "r";
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return false;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
            WriteConsole(console, cmd.c_str(), cmd.length(), nullptr, nullptr);
        }
#endif
        return true;
    }

    bool TUI::Terminal::resetScrollRegion() {
        std::string cmd = "\x1b[r";
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return false;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
            WriteConsole(console, cmd.c_str(), cmd.length(), nullptr, nullptr);
        }
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
    }

    std::string TUI::Terminal::readLine() {
        std::string out;
#ifdef TINY_CPP_MY_OS_UNIX
        if (_is_in_raw_mode) {
            out = readLineOnRaw();
        } else {
            char buffer[1024] = {};
            while (true) {
                ssize_t read_count = readAfterDelay(STDIN_FILENO, buffer, 1024);
                if (read_count <= 0) continue;
                if (buffer[read_count - 1] == '\n') read_count -= 1;
                out.resize(read_count);
                strncpy(&out[0], buffer, read_count);
                break;
            }
        }
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        char buffer[2048] = {};
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return out;
        if (!ReadConsole(console, buffer, 2048, &read_count, nullptr)) return out;
        if (read_count >= 2) read_count -= 2;
        out.resize(read_count);
        strncpy(&out[0], buffer, read_count);
#endif
        return out;
    }

    std::wstring TUI::Terminal::readLineW() {
        std::wstring out;
#ifdef TINY_CPP_MY_OS_UNIX
        if (_is_in_raw_mode) {
            out = string2Wide(readLineOnRaw());
        } else {
            char buffer[2048] = {};
            size_t read_count = readAfterDelay(STDIN_FILENO, buffer, 2048);
            if (buffer[read_count - 1] == '\n') read_count -= 1;
            out = string2Wide(buffer);
            if (out.back() == L'\n') out.pop_back();
        }
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        wchar_t buffer[2048] = {};
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return out;
        if (!ReadConsoleW(console, buffer, 2048, &read_count, nullptr)) return out;
        if (read_count >= 2) read_count -= 2;
        out.resize(read_count);
        wcsncpy(&out[0], buffer, read_count);
#endif
        return out;
    }

    uint8_t TUI::Terminal::getKey(SP_Keys* sp_key) {
#ifdef TINY_CPP_MY_OS_UNIX
        InputEvent ev{};
        std::string buf;
        
        while (true) {
            if (!pushBuffers(STDIN_FILENO, 10) && !nextBuffers(buf)) {
                continue;
            }
            ev = parseInputEvent(buf.c_str());
            if (ev.type == InputEvent::Keyboard) {
                if (ev.input.keyboard.key == KEY_SPECIAL && sp_key) {
                    *sp_key = ev.input.keyboard.sp_key;
                }
                _temp_buffers.clear();
                break;
            }
        }
        return ev.input.keyboard.key;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return KEY_NONE;
        INPUT_RECORD input;
        while (ReadConsoleInputA(console, &input, 1, &read_count)) {
            auto input_handle = parseInputRecord(&input);
            if (input_handle.type != InputEvent::Keyboard || !input_handle.input.keyboard.is_pressed) continue;
            if (sp_key) *sp_key = input_handle.input.keyboard.sp_key;
            return input_handle.input.keyboard.key;
        }
#endif
        return 0;
    }

    bool TUI::Terminal::setMouseEnabled(bool enabled) {
#ifdef TINY_CPP_MY_OS_UNIX
#ifdef HAS_GPM
        if (NEED_USE_GPM()) {
            int ret;
            if (enabled) {
                _gpm_connector.defaultMask = 0;
                _gpm_connector.eventMask = ~GPM_HARD;
                _gpm_connector.minMod = 0;
                _gpm_connector.maxMod = 0;
                ret = Gpm_Open(&_gpm_connector, 0);
                _gpm_fd = ret;
            } else {
                ret = (Gpm_Close() == 0);
                _gpm_fd = 0;
            }
            return ret > 0;
        }
#endif
        std::string cmd = enabled ? "\x1b[?1000;1003;1006h" : "\x1b[?1000;1003;1006l";
        write(STDIN_FILENO, cmd.data(), cmd.size());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        DWORD mode = 0;
        if (!GetConsoleMode(console, &mode)) return false;
        if (enabled) {
            _old_console_handle = mode;
            mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;
        } else {
            mode = _old_console_handle;
        }
        if (!SetConsoleMode(console, mode)) return false;
#endif
        return true;
    }

    uint8_t TUI::Terminal::getMouseButton(Position* mouse_pos, bool* is_pressed) {
#ifdef TINY_CPP_MY_OS_UNIX
        InputEvent ev{};
        std::string buf;
        bool ok = false;
        while (true) {
#ifdef TINY_CPP_USE_GPM
            if (NEED_USE_GPM()) {
                while (true) {
                    auto mouse = parseMouseEvent(buf, ok);
                    if (ok) {
                        if (mouse_pos) *mouse_pos = mouse.position;
                        if (is_pressed) *is_pressed = mouse.is_pressed;
                        return mouse.button;
                    }
                    usleep(10000);
                }
            }
#endif
            if (!pushBuffers(STDIN_FILENO, 10) || !nextBuffers(buf)) {
                continue;
            }
            ev = parseInputEvent(buf.c_str());
            if (ev.type == InputEvent::Mouse) break;
        }
        if (mouse_pos) *mouse_pos = ev.input.mouse.position;
        if (is_pressed) *is_pressed = ev.input.mouse.is_pressed;
        return ev.input.mouse.button;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return '\0';
        INPUT_RECORD input;
        DWORD read_count = 0;
        while (ReadConsoleInput(console, &input, 1, &read_count)) {
            auto input_handle = parseInputRecord(&input);
            if (input_handle.type != InputEvent::Mouse) continue;
            if (mouse_pos) *mouse_pos = input_handle.input.mouse.position;
            if (is_pressed) *is_pressed = input_handle.input.mouse.is_pressed;
            return input_handle.input.mouse.button;
        }
#endif
        return SP_MOUSE_UNKNOWN;
    }

    TUI::InputEvent TUI::Terminal::getInput() {
        InputEvent ev{};
#ifdef TINY_CPP_MY_OS_UNIX
        std::string buf;
#ifdef TINY_CPP_USE_GPM
        if (NEED_USE_GPM()) {
            bool ok = false;
            auto mouse = parseMouseEvent(buf, ok);
            if (ok) {
                ev.type = InputEvent::Mouse;
                ev.input.mouse = mouse;
                return ev;
            }
            _temp_buffers.clear();
            if (!pushBuffers(STDIN_FILENO, 50) || !nextBuffers(buf)) return ev;
            auto keyboard = parseKeyboardEvent(buf, ok);
            if (ok) {
                ev.type = InputEvent::Keyboard;
                ev.input.keyboard = keyboard;
            } 
            return ev;
        }
#endif
        if (_temp_buffers.empty()) {
            if (!pushBuffers(STDIN_FILENO, 50)) return ev;
        }
        if (!nextBuffers(buf)) return ev;
        ev = parseInputEvent(buf.c_str());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) {
            return ev;
        }
        Sleep(10);
        INPUT_RECORD input;
        DWORD get_length = 0;
        DWORD length = 0;
        GetNumberOfConsoleInputEvents(console, &get_length);
        if (get_length > 0) {
            ReadConsoleInput(console, &input, 1, &length);
            ev = parseInputRecord(&input);
        }
#endif
        return ev;
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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

    void TUI::Terminal::setReverseColor(bool enable) {
#ifdef TINY_CPP_MY_OS_UNIX
        const char* cmd = enable ? "\x1b[7m" : "\x1b[27m";
        write(STDOUT_FILENO, cmd, strlen(cmd));
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS) && defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
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
        auto w_str = string2Wide(str);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) return false;
        return true;
#endif
    }

#ifdef TINY_CPP_MY_OS_WINDOWS
    TUI::InputEvent TUI::Terminal::parseInputRecord(void *input_record) {
        auto input = static_cast<PINPUT_RECORD>(input_record);
        InputEvent result{};
        if (input->EventType == KEY_EVENT) {
            result.type = InputEvent::Keyboard;
            result.input.keyboard.is_pressed = input->Event.KeyEvent.bKeyDown;
            SP_Keys sp_key{};
            auto& ev = input->Event.KeyEvent;
            auto v_keycode = ev.wVirtualKeyCode;
            uint8_t ch = ev.uChar.AsciiChar;
            result.input.keyboard.key = KEY_SPECIAL;
            switch (v_keycode) {
                case VK_F1:
                    sp_key = SP_KEY_F1;
                    break;
                case VK_F2:
                    sp_key = SP_KEY_F2;
                    break;
                case VK_F3:
                    sp_key = SP_KEY_F3;
                    break;
                case VK_F4:
                    sp_key = SP_KEY_F4;
                    break;
                case VK_F5:
                    sp_key = SP_KEY_F5;
                    break;
                case VK_F6:
                    sp_key = SP_KEY_F6;
                    break;
                case VK_F7:
                    sp_key = SP_KEY_F7;
                    break;
                case VK_F8:
                    sp_key = SP_KEY_F8;
                    break;
                case VK_F9:
                    sp_key = SP_KEY_F9;
                    break;
                case VK_F10:
                    sp_key = SP_KEY_F10;
                    break;
                case VK_F11:
                    sp_key = SP_KEY_F11;
                    break;
                case VK_F12:
                    sp_key = SP_KEY_F12;
                    break;
                case VK_INSERT:
                    sp_key = SP_KEY_INSERT;
                    break;
                case VK_DELETE:
                    sp_key = SP_KEY_DELETE;
                    break;
                case VK_HOME:
                    sp_key = SP_KEY_HOME;
                    break;
                case VK_END:
                    sp_key = SP_KEY_END;
                    break;
                case VK_PRIOR:
                    sp_key = SP_KEY_PAGE_UP;
                    break;
                case VK_NEXT:
                    sp_key = SP_KEY_PAGE_DOWN;
                    break;
                case VK_CLEAR:
                    sp_key = SP_KEY_CENTER;
                    break;
                case VK_UP:
                    sp_key = SP_KEY_UP;
                    break;
                case VK_LEFT:
                    sp_key = SP_KEY_LEFT;
                    break;
                case VK_DOWN:
                    sp_key = SP_KEY_DOWN;
                    break;
                case VK_RIGHT:
                    sp_key = SP_KEY_RIGHT;
                    break;
                case VK_SNAPSHOT:
                    sp_key = SP_KEY_PRINTSCR;
                    break;
                case VK_CONTROL:
                    sp_key = SP_KEY_CTRL;
                    break;
                case VK_SHIFT:
                    sp_key = SP_KEY_SHIFT;
                    break;
                case VK_MENU:
                    sp_key = SP_KEY_ALT;
                    break;
                case VK_CAPITAL:
                    sp_key = SP_KEY_CAPSLOCK;
                    break;
                case VK_NUMLOCK:
                    sp_key = SP_KEY_NUMLOCK;
                    break;
                case VK_SCROLL:
                    sp_key = SP_KEY_SCROLLLOCK;
                    break;
                default:
                    result.input.keyboard.key = ch > 0 ? ch : KEY_NONE;
            }
            result.input.keyboard.sp_key = sp_key;
        } else if (input->EventType == MOUSE_EVENT) {
            result.type = InputEvent::Mouse;
            auto mouse_event = input->Event.MouseEvent;
            result.input.mouse.position.row = mouse_event.dwMousePosition.Y;
            result.input.mouse.position.column = mouse_event.dwMousePosition.X;
            result.input.mouse.is_pressed = false;
            if (mouse_event.dwEventFlags == MOUSE_WHEELED) {
                auto high = mouse_event.dwButtonState >> 24;
                if (high == 0) {
                    result.input.mouse.button = SP_MOUSE_WHEEL_UP;
                } else {
                    result.input.mouse.button = SP_MOUSE_WHEEL_DOWN;
                }
            } else if (mouse_event.dwEventFlags == MOUSE_MOVED) {
                result.input.mouse.button = SP_MOUSE_MOVED;
                result.input.mouse.is_pressed = mouse_event.dwButtonState > 0;
            } else {
                result.input.mouse.is_pressed = true;
                if (mouse_event.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    result.input.mouse.button = SP_MOUSE_LEFT_BUTTON;
                } else if (mouse_event.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                    result.input.mouse.button = SP_MOUSE_RIGHT_BUTTON;
                } else if (mouse_event.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
                    result.input.mouse.button = SP_MOUSE_MIDDLE_BUTTON;
                } else {
                    result.input.mouse.button = SP_MOUSE_RELEASE;
                    result.input.mouse.is_pressed = false;
                }
            }
        }
        return result;
    }
#endif

#ifdef TINY_CPP_MY_OS_UNIX
    std::string TUI::Terminal::readLineOnRaw() {
        std::string result;
        char temp[256] = {};
        size_t read_bytes = 0;
        auto start_pos = cursorPosition();
        do {
            read_bytes = readAfterDelay(STDIN_FILENO, temp, 256);
            if (read_bytes < 0) break;
            if (temp[0] == KEY_CR || temp[0] == KEY_LF) {
                auto enter = "\r\n";
                write(STDOUT_FILENO, enter, 2);
                break;
            } else if (temp[0] == KEY_BACKSPACE) {
                if (!result.empty()) {
                    size_t ori_length = result.size();
                    size_t del_length = removeFrontCharCount(result);
                    result = result.substr(0, result.size() - del_length);
                    moveCursor(start_pos);
                    auto cover = std::string(ori_length, ' ');
                    write(STDOUT_FILENO, cover.c_str(), cover.size());
                    moveCursor(start_pos);
                    write(STDOUT_FILENO, result.data(), result.size());
                }
            } else if (temp[0] != '\x1b') {
                result += temp;
                write(STDOUT_FILENO, temp, read_bytes);
            }
        } while (read_bytes > 0);

        return result;
    }

    bool TUI::Terminal::isReady(int fd) {
        fd_set sets;
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeval internal = {0, 0};
        auto ret = select(fd + 1, &sets, nullptr, nullptr, &internal);
        if (ret < 0) return false;
        return FD_ISSET(fd, &sets);
    }

    bool TUI::Terminal::pushBuffers(int fd, size_t msecs) {
        fd_set sets;
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeval internal;
        internal.tv_sec = msecs / 1000;
        internal.tv_usec = (msecs % 1000) * 1000;
        auto ret = select(fd + 1, &sets, nullptr, nullptr, &internal);
        if (ret < 0 || !FD_ISSET(fd, &sets)) {
            return false;
        }
        size_t preread_cnt = 0;
        ioctl(STDIN_FILENO, FIONREAD, &preread_cnt);
        char buf[256] = {};
        ssize_t read_cnt = 0;
        
        read_cnt = read(fd, buf, preread_cnt);
        _temp_buffers.insert(_temp_buffers.end(), buf, buf + read_cnt);
        
        return true;
    }

    bool TUI::Terminal::nextBuffers(std::string& buffer) {
        if (_temp_buffers.empty()) return false;
        buffer.clear();
        // If the first char is `ESC`, start parsing.
        if (_temp_buffers[0] == '\x1b') {
            // If getting the current cursor position, discard it.
            int a = 0, b = 0;
            if (sscanf(_temp_buffers.c_str(), "\x1b[%d;%dR", &a, &b) == 2) {
                size_t del_cnt = 4 + std::to_string(a).size() + std::to_string(b).size();
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + del_cnt);
                
                return true;
            }
            if (_temp_buffers.size() == 1 || _temp_buffers[1] != '[') {
                buffer += _temp_buffers[0];
                _temp_buffers.erase(_temp_buffers.begin());
                return true;
            }
            // Parsing mouse
            if (_temp_buffers[2] == '<') {
                size_t dis = 1;
                while (true) {
                    if (_temp_buffers[2 + dis] == 'M' || _temp_buffers[2 + dis] == 'm') break;
                    dis++;
                }
                buffer += _temp_buffers.substr(0, dis + 3);
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + (dis + 3));
                return true;
            }
            // Parsing Special Keys
            if (_temp_buffers[1] == 'O') {
                buffer += _temp_buffers.substr(0, 3);
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + 3);
                return true;
            }
            if (_temp_buffers[3] == '~') {
                buffer += _temp_buffers.substr(0, 4);
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + 4);
                return true;
            }
            if (_temp_buffers[4] == '~') {
                buffer += _temp_buffers.substr(0, 5);
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + 5);
                return true;
            }
            if (_temp_buffers[2] >= 'A' && _temp_buffers[2] <= 'H') {
                buffer += _temp_buffers.substr(0, 3);
                _temp_buffers.erase(_temp_buffers.begin(), _temp_buffers.begin() + 3);
                return true;
            }
        }
        // Only single char
        buffer += *_temp_buffers.begin();
        _temp_buffers.erase(_temp_buffers.begin());
        return true;
    }

    ssize_t TUI::Terminal::readAfterDelay(int fd, void* buffer, size_t size, size_t delay) {
        fd_set sets;
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeval internal;
        internal.tv_sec = delay / 1000;
        internal.tv_usec = (delay % 1000) * 1000;
        auto ret = select(fd + 1, &sets, nullptr, nullptr, &internal);
        if (ret == -1) return -1;
        return read(fd, buffer, size);
    }

    ssize_t TUI::Terminal::writeAfterDelay(int fd, void* buffer, size_t size, size_t delay) {
        fd_set sets;
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeval internal;
        internal.tv_sec = delay / 1000;
        internal.tv_usec = (delay % 1000) * 1000;
        auto ret = select(fd + 1, &sets, nullptr, nullptr, &internal);
        if (ret == -1) return -1;
        return write(fd, buffer, size);
    }

    size_t TUI::Terminal::removeFrontCharCount(const std::string& buf) {
        if (buf.empty()) return 0;
        size_t cnt = 0;
        for (size_t i = buf.size() - 1; ; --i) {
            cnt++;
            if ((static_cast<uint8_t>(buf[i]) & 0xc0) != 0x80) {
                break;
            }
            if (i == 0) break;
        }
        return cnt;
    }

    TUI::InputEvent TUI::Terminal::parseInputEvent(const char *signal) {
        InputEvent result{};
        std::string temp_cmd = signal;
        bool sp_flag = false;
        
#ifdef TINY_CPP_USE_GPM
        sp_flag = NEED_USE_GPM();
#endif
        if (!sp_flag && temp_cmd.empty()) return result;
        bool ok = false;
        auto mouse = parseMouseEvent(temp_cmd, ok);
        if (ok) {
            result.type = InputEvent::Mouse;
            result.input.mouse = mouse;
            return result;
        }
        auto keyboard = parseKeyboardEvent(temp_cmd, ok);
        if (ok) {
            result.type = InputEvent::Keyboard;
            result.input.keyboard = keyboard;
        }
        return result;
    }

    TUI::KeyEvent TUI::Terminal::parseKeyboardEvent(const std::string& buf, bool& ok) {
        InputEvent::Input::Keyboard keyboard{};
        keyboard.is_pressed = true;
        if (buf.size() > 1) {
            if (buf.front() == '\x1b') {
                if (buf == "\x1bOP") {
                    keyboard.sp_key = SP_KEY_F1;
                } else if (buf == "\x1bOQ") {
                    keyboard.sp_key = SP_KEY_F2;
                } else if (buf == "\x1bOR") {
                    keyboard.sp_key = SP_KEY_F3;
                } else if (buf == "\x1bOS") {
                    keyboard.sp_key = SP_KEY_F4;
                } else if (buf == "\x1b[15~") {
                    keyboard.sp_key = SP_KEY_F5;
                } else if (buf == "\x1b[17~") {
                    keyboard.sp_key = SP_KEY_F6;
                } else if (buf == "\x1b[18~") {
                    keyboard.sp_key = SP_KEY_F7;
                } else if (buf == "\x1b[19~") {
                    keyboard.sp_key = SP_KEY_F8;
                } else if (buf == "\x1b[20~") {
                    keyboard.sp_key = SP_KEY_F9;
                } else if (buf == "\x1b[21~") {
                    keyboard.sp_key = SP_KEY_F10;
                } else if (buf == "\x1b[23~") {
                    keyboard.sp_key = SP_KEY_F11;
                } else if (buf == "\x1b[24~") {
                    keyboard.sp_key = SP_KEY_F12;
                } else if (buf == "\x1b[2~") {
                    keyboard.sp_key = SP_KEY_INSERT;
                } else if (buf == "\x1b[3~") {
                    keyboard.sp_key = SP_KEY_DELETE;
                } else if (buf == "\x1b[H") {
                    keyboard.sp_key = SP_KEY_HOME;
                } else if (buf == "\x1b[F") {
                    keyboard.sp_key = SP_KEY_END;
                } else if (buf == "\x1b[5~") {
                    keyboard.sp_key = SP_KEY_PAGE_UP;
                } else if (buf == "\x1b[6~") {
                    keyboard.sp_key = SP_KEY_PAGE_DOWN;
                } else if (buf == "\x1b[A") {
                    keyboard.sp_key = SP_KEY_UP;
                } else if (buf == "\x1b[D") {
                    keyboard.sp_key = SP_KEY_LEFT;
                } else if (buf == "\x1b[B") {
                    keyboard.sp_key = SP_KEY_DOWN;
                } else if (buf == "\x1b[C") {
                    keyboard.sp_key = SP_KEY_RIGHT;
                } else if (buf == "\x1b[E") {
                    keyboard.sp_key = SP_KEY_CENTER;
                }
            }
            if (keyboard.sp_key != SP_KEY_UNKNOWN) {
                keyboard.key = KEY_SPECIAL;
                ok = true;
                return keyboard;
            }
        } else if (buf.size() == 1) {
            keyboard.key = buf.front();
            keyboard.sp_key = SP_KEY_UNKNOWN;
            ok = true;
            return keyboard;
        }
        ok = false;
        return keyboard;
    }

    TUI::MouseEvent TUI::Terminal::parseMouseEvent(const std::string& buf, bool& ok) {
        MouseEvent mouse{};
#ifdef TINY_CPP_USE_GPM
        if (NEED_USE_GPM()) {
            if (_gpm_fd == 0 || _gpm_connector.pid == 0) return mouse;
            Gpm_Event event{};
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(_gpm_fd, &read_set);
            timeval internal;
            internal.tv_sec = 0;
            internal.tv_usec = 50000;
            select(_gpm_fd + 1, &read_set, nullptr, nullptr, &internal);
            if (FD_ISSET(gpm_fd, &read_set) && Gpm_GetEvent(&event) > 0) {
                mouse.position.row = event.y;
                mouse.position.column = event.x;
                if (event.type & GPM_MOVE || event.type & GPM_DRAG) {
                    mouse.button = SP_MOUSE_MOVED;
                } else if (event.type & GPM_DOWN) {
                    // Do nothing, and continue...
                } else if (event.type & GPM_UP) {
                    mouse.button = SP_MOUSE_RELEASE;
                    ok = true;
                    return mouse;
                } else {
                    ok = false;
                    return mouse;
                }
                mouse.is_pressed = (event.buttons > 0);
                if (mouse.is_pressed) {
                    if (event.buttons & GPM_B_LEFT) {
                        mouse.button = SP_MOUSE_LEFT_BUTTON;
                    } else if (event.buttons & GPM_B_RIGHT) {
                        mouse.button = SP_MOUSE_RIGHT_BUTTON;
                    } else if (event.buttons & GPM_B_MIDDLE) {
                        mouse.button = SP_MOUSE_MIDDLE_BUTTON;
                    }
                }
                if (event.wdy > 0) mouse.button = SP_MOUSE_WHEEL_UP;
                else if (event.wdy < 0) mouse.button = SP_MOUSE_WHEEL_DOWN;
                ok = true;
                return mouse;
            }
            ok = false;
            return mouse;
        }
#endif
        uint32_t ev_type, row, col;
        bool is_big_M = (buf[buf.size() - 1] == 'M');
        if (sscanf(buf.c_str(), "\x1b[<%d;%d;%d", &ev_type, &col, &row) != 3) {
            mouse.button = SP_MOUSE_UNKNOWN;
        } else { 
            if (is_big_M) {
                mouse.is_pressed = (ev_type != 35) && (ev_type != 64) && (ev_type != 65);
            } else {
                mouse.is_pressed = false;
            }
            
            mouse.position.row = row - 1;
            mouse.position.column = col - 1;

            switch (ev_type) {
            case 0:
                mouse.button = SP_MOUSE_LEFT_BUTTON;
                break;
            case 1:
                mouse.button = SP_MOUSE_MIDDLE_BUTTON;
                break;
            case 2:
                mouse.button = SP_MOUSE_RIGHT_BUTTON;
                break;
            case 3:
                mouse.button = SP_MOUSE_RELEASE;
                break;
            case 32:
            case 33:
            case 34:
            case 35:
                mouse.button = SP_MOUSE_MOVED;
                break;
            case 64:
                mouse.button = SP_MOUSE_WHEEL_UP;
                break;
            case 65:
                mouse.button = SP_MOUSE_WHEEL_DOWN;
                break;
            }
            if (!mouse.is_pressed && 
                 mouse.button >= SP_MOUSE_LEFT_BUTTON && mouse.button <= SP_MOUSE_RIGHT_BUTTON) {
                mouse.button = SP_MOUSE_RELEASE;
            }
        }
        
        if (mouse.button != SP_MOUSE_UNKNOWN) {
            ok = true;
            return mouse;
        }

        ok = false;
        return mouse;
    }
#endif
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
