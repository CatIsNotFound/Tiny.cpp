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

#include "TUI.hpp"
#if defined(TINY_CPP_MY_OS_WINDOWS)
#include <windows.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

#if defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_DEFINED_WIN)
#define TINY_CPP_DEFINED_WIN
namespace Tiny {
    std::string Win::convert2Win(const std::string& path) {
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

namespace Tiny {
    bool TUI::Terminal::enterRawMode() {
#ifdef TINY_CPP_MY_OS_UNIX
        struct termios raw;
        tcgetattr(STDIN_FILENO, &raw);
        raw.c_lflag &= ~(ECHO | ICANON | ISIG);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        setCursorVisible(false);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return false;
        DWORD raw_mode = mode;
        raw_mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
        if (!SetConsoleMode(console, raw_mode)) return false;

#endif
        return true;
    }

    bool TUI::Terminal::leaveRawMode() {
#ifdef TINY_CPP_MY_OS_UNIX
        struct termios raw;
        tcgetattr(STDIN_FILENO, &raw);
        raw.c_lflag |= (ECHO | ICANON | ISIG);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        setCursorVisible(true);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        DWORD mode;
        if (!GetConsoleMode(console, &mode)) return false;
        DWORD raw_mode = mode;
        raw_mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
        if (!SetConsoleMode(console, raw_mode)) return false;
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
        PCONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, info)) return size;
        size.width = info->dwSize.X;
        size.height = info->dwSize.Y;
#endif
        return size;
    }

    TUI::Position TUI::Terminal::cursorPosition() {
        Position position;
#ifdef TINY_CPP_MY_OS_UNIX

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
        write(STDOUT_FILENO, (text + "\n").c_str(), text.length() + 1);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        auto w_str = Win::string2Wide(text + "\n");
        if (!WriteConsoleW(console, w_str.c_str(), w_str.size(), nullptr, nullptr)) {
            return false;
        }
#endif
        return true;
    }

    bool TUI::Terminal::clearScreen() {
#ifdef TINY_CPP_MY_OS_UNIX
        write(STDOUT_FILENO, "\x1b[2J\x1b[6n", 4);
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
        write(STDOUT_FILENO, "\x1b[2J\x1b[6n", 4);
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (!GetConsoleScreenBufferInfo(console, &info)) return false;
        auto size = info.dwSize.Y;
        DWORD written{};
        if (!FillConsoleOutputCharacterA(console, ' ', size, {0, row}, &written))
            return false;
        if (!SetConsoleCursorPosition(console, {0, row})) return false;
#endif
        return true;
    }

    bool TUI::Terminal::moveCursor(Position position) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[" + std::to_string(position.row) + ";" + std::to_string(position.column) + "H";
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
        std::string cmd = "\x1b[" + std::to_string(row) + ";" + std::to_string(column) + "H";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        if (console == INVALID_HANDLE_VALUE) return false;
        if (!SetConsoleCursorPosition(console, {static_cast<short>(column), static_cast<short>(row)})) return false;
#endif
        return true;
    }

    void TUI::Terminal::setBackgroundColor(Color color) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[4" + std::to_string(static_cast<uint8_t>(color)) + "m";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());     

#endif

    }

        void TUI::Terminal::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());     
#endif

    }

    void TUI::Terminal::setForegroundColor(Color color) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[3" + std::to_string(static_cast<uint8_t>(color)) + "m";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());     
#endif

    }

    void TUI::Terminal::setForegroundColor(uint8_t r, uint8_t g, uint8_t b) {
#ifdef TINY_CPP_MY_OS_UNIX
        std::string cmd = "\x1b[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
        write(STDOUT_FILENO, cmd.c_str(), cmd.length());     
#endif

    }

    void TUI::Terminal::setBolder(bool enable) {
        
    }

    void TUI::Terminal::setDark(bool enable) {
    }

    void TUI::Terminal::setItalic(bool enable) {
    }

    void TUI::Terminal::setUnderline(bool enable) {
    }

    void TUI::Terminal::setBlinking(bool enable) {
    }

    void TUI::Terminal::reverseColor(bool enable) {
    }

    void TUI::Terminal::setCursorVisible(bool enable) {
    }

    void TUI::Terminal::setStrikethrough(bool enable) {
    }

    void TUI::Terminal::reset() {
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
