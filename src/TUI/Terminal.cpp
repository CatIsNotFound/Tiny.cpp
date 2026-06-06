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
#include <conio.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <cstring>
#include <cwchar>
#include <clocale>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
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
        } else if (KEY == KEY_BACKSPACE) {
            return "Backspace";
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
            default:
                return "Unknown";
        }
    }

#ifdef TINY_CPP_MY_OS_UNIX
    termios TUI::Terminal::_old_terminal{};
    bool TUI::Terminal::_is_in_raw_mode{};
    TUI::Position TUI::Terminal::_last_cur_pos{0, 0};
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
        mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
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
#elif defined(TINY_CPP_MY_OS_WINDOWS)
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
#elif defined(TINY_CPP_MY_OS_WINDOWS)
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
            char buffer[2048] = {};
            size_t read_count = readAfterDelay(STDIN_FILENO, buffer, 2048);
            if (buffer[read_count - 1] == '\n') read_count -= 1;
            out.resize(read_count);
            strncpy(&out[0], buffer, read_count);
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

    uint8_t TUI::Terminal::getKey() {
#ifdef TINY_CPP_MY_OS_UNIX
        return getchar();
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return '\0';
        INPUT_RECORD input;
        DWORD read_count = 0;
        while (ReadConsoleInputA(console, &input, 1, &read_count)) {
            if (input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown) continue;
            if (read_count == 0) return '\0';
            return input.Event.KeyEvent.uChar.AsciiChar;
        }
        return '\0';
#endif
    }

    void TUI::Terminal::getKey(uint8_t& key, SP_Keys& sp_key) {
#ifdef TINY_CPP_MY_OS_UNIX
        char buf[16] = {};
        std::string temp_cmd;
        size_t keys_cnt = readAfterDelay(STDIN_FILENO, buf, 16);
        temp_cmd = buf;
        if (keys_cnt > 1) {
            key = KEY_SPECIAL;
            if (temp_cmd == "\x1bOP") {
                sp_key = SP_KEY_F1;
            } else if (temp_cmd == "\x1bOQ") {
                sp_key = SP_KEY_F2;
            } else if (temp_cmd == "\x1bOR") {
                sp_key = SP_KEY_F3;
            } else if (temp_cmd == "\x1bOS") {
                sp_key = SP_KEY_F4;
            } else if (temp_cmd == "\x1b[15~") {
                sp_key = SP_KEY_F5;
            } else if (temp_cmd == "\x1b[17~") {
                sp_key = SP_KEY_F6;
            } else if (temp_cmd == "\x1b[18~") {
                sp_key = SP_KEY_F7;
            } else if (temp_cmd == "\x1b[19~") {
                sp_key = SP_KEY_F8;
            } else if (temp_cmd == "\x1b[20~") {
                sp_key = SP_KEY_F9;
            } else if (temp_cmd == "\x1b[21~") {
                sp_key = SP_KEY_F10;
            } else if (temp_cmd == "\x1b[23~") {
                sp_key = SP_KEY_F11;
            } else if (temp_cmd == "\x1b[24~") {
                sp_key = SP_KEY_F12;
            } else if (temp_cmd == "\x1b[2~") {
                sp_key = SP_KEY_INSERT;
            } else if (temp_cmd == "\x1b[3~") {
                sp_key = SP_KEY_DELETE;
            } else if (temp_cmd == "\x1b[H") {
                sp_key = SP_KEY_HOME;
            } else if (temp_cmd == "\x1b[F") {
                sp_key = SP_KEY_END;
            } else if (temp_cmd == "\x1b[5~") {
                sp_key = SP_KEY_PAGE_UP;
            } else if (temp_cmd == "\x1b[6~") {
                sp_key = SP_KEY_PAGE_DOWN;
            } else if (temp_cmd == "\x1b[A") {
                sp_key = SP_KEY_UP;
            } else if (temp_cmd == "\x1b[D") {
                sp_key = SP_KEY_LEFT;
            } else if (temp_cmd == "\x1b[B") {
                sp_key = SP_KEY_DOWN;
            } else if (temp_cmd == "\x1b[C") {
                sp_key = SP_KEY_RIGHT;
            } else if (temp_cmd == "\x1b[E") {
                sp_key = SP_KEY_CENTER;
            } else {
                key = KEY_UNKNOWN;
            }
        } else {
            key = buf[0];
        }
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        DWORD read_count = 0;
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return;
        INPUT_RECORD input;
        while (ReadConsoleInputA(console, &input, 1, &read_count)) {
            if (input.EventType != KEY_EVENT) continue;
            if (!input.Event.KeyEvent.bKeyDown) continue;
            auto& ev = input.Event.KeyEvent;
            auto v_keycode = ev.wVirtualKeyCode;
            auto ch = ev.uChar.AsciiChar;
            key = KEY_SPECIAL;
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
                default:
                    if (ch > 0) {
                        key = ch;
                    } else {
                        key = KEY_UNKNOWN;
                    }
                    break;
            }
            break;
        }
#endif
    }

    bool TUI::Terminal::setMouseEnabled(bool enabled) {
#ifdef TINY_CPP_MY_OS_UNIX
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
        char buf[16] = {};
        size_t read_bytes = readAfterDelay(STDIN_FILENO, buf, 16);
        
        uint32_t ev_type, row, col;
        bool is_big_M = (buf[read_bytes - 1] == 'M');
        if (sscanf(buf, "\x1b[<%d;%d;%d", &ev_type, &col, &row) != 3) {
            if (sscanf(buf, "\x1b[%d;%dH", &row, &col) == 2) {
                if (mouse_pos) {
                    mouse_pos->row = row - 1;
                    mouse_pos->column = col - 1;
                }
                return SP_MOUSE_MOVED;
            }
            return SP_MOUSE_UNKNOWN;
        }

        if (is_pressed) {
            if (is_big_M) {
                *is_pressed = (ev_type != 35);
            } else {
                *is_pressed = false;
            }
        }
        if (mouse_pos) {
            mouse_pos->row = row - 1;
            mouse_pos->column = col - 1;
        }

        switch (ev_type) {
        case 0:
            return SP_MOUSE_LEFT_BUTTON;
        case 1:
            return SP_MOUSE_MIDDLE_BUTTON;
        case 2:
            return SP_MOUSE_RIGHT_BUTTON;
        case 32:
        case 33:
        case 34:
        case 35:
            return SP_MOUSE_MOVED;
        case 64:
            return SP_MOUSE_WHEEL_UP;
        case 65:
            return SP_MOUSE_WHEEL_DOWN;
        }

#elif defined(TINY_CPP_MY_OS_WINDOWS)
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return '\0';
        INPUT_RECORD input;
        DWORD read_count = 0;
        while (ReadConsoleInput(console, &input, 1, &read_count)) {
            if (input.EventType != MOUSE_EVENT) continue;
            if (read_count == 0) return SP_MOUSE_UNKNOWN;
            auto mouse_event = input.Event.MouseEvent;
            if (mouse_pos) {
                mouse_pos->row = mouse_event.dwMousePosition.Y;
                mouse_pos->column = mouse_event.dwMousePosition.X;
            }
            if (mouse_event.dwEventFlags == MOUSE_WHEELED) {
                auto high = mouse_event.dwButtonState >> 24;
                if (high == 0) {
                    return SP_MOUSE_WHEEL_UP;
                }
                return SP_MOUSE_WHEEL_DOWN;
            }
            if (mouse_event.dwEventFlags == MOUSE_MOVED) return SP_MOUSE_MOVED;
            if (mouse_event.dwEventFlags == 0) {
                if (is_pressed) *is_pressed = true;
                if (mouse_event.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    return SP_MOUSE_LEFT_BUTTON;
                } else if (mouse_event.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                    return SP_MOUSE_RIGHT_BUTTON;
                } else if (mouse_event.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
                    return SP_MOUSE_MIDDLE_BUTTON;
                } else if (is_pressed) {
                    *is_pressed = false;
                }
            }
        }
#endif
        return SP_MOUSE_UNKNOWN;
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
        auto w_str = string2Wide(str);
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) return false;
        return true;
#endif
    }

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
