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

#ifndef TINY_CPP_TUI_TERMINAL_HPP
#define TINY_CPP_TUI_TERMINAL_HPP
#include <string>
#include <sstream>
#include <cstdint>

/*******************************************************************************************
 * P.S: - To enabled using Windows32 lib, please use the macro below:                      *
 *      #define TINY_CPP_MY_OS_WINDOWS                                                     *
 *      - To enabled using Unix lib (for Linux, MacOS, etc.), please use the macro below:  *
 *      #define TINY_CPP_MY_OS_UNIX                                                        *
 *******************************************************************************************/

#if !defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_MY_OS_UNIX)
    #if defined(_WIN32) || defined(_WIN64)
        #define TINY_CPP_MY_OS_WINDOWS
    #elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
        #define TINY_CPP_MY_OS_UNIX
    #endif
#endif

#if defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_DEFINED_WIN)
namespace Tiny {
    namespace Win {
        std::string convertPath(const std::string& path);
        std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001);
        std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001);
    }
}
#endif

#if defined(TINY_CPP_MY_OS_UNIX) && !defined(TINY_CPP_DEFINED_UNIX)
namespace Tiny {
    namespace Win {
        std::wstring string2Wide(const std::string& str);
        std::string wide2String(const std::wstring& w_str);
    }
}
#endif

#ifdef TINY_CPP_MY_OS_UNIX
struct termios;
#endif

namespace Tiny {
    namespace TUI {
        struct Size { uint32_t width{}, height{}; };
        struct Position { uint32_t row{}, column{}; };
        enum class Color : uint8_t {
            Black = 0,
            Red = 1,
            Green = 2,
            Yellow = 3,
            Blue = 4,
            Magenta = 5,
            Cyan = 6,
            White = 7,
            Default = 9
        };

        class Terminal {
        public:
            static bool enterRawMode();
            static bool leaveRawMode();
            static Size screenSize();
            static Position cursorPosition();
            static bool print(const std::string &text);
            static bool printLine(const std::string &text);
            template<typename ... Args>
            static bool printFormat(const char* format, Args... args);
            static bool clearScreen();
            static bool clearInRow(uint8_t row);
            static bool moveCursor(Position position);
            static bool moveCursor(uint32_t row, uint32_t column);
            static bool flushScreen();
            static std::string readLine();
            static std::wstring readLineW();

            static void setBackgroundColor(Color color, bool intensity = true);
            static void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
            static void setForegroundColor(Color color, bool intensity = false);
            static void setForegroundColor(uint8_t r, uint8_t g, uint8_t b);
            static void setBolder(bool enable);
            static void setDark(bool enable);
            static void setItalic(bool enable);
            static void setUnderline(bool enable);
            static void setBlinking(bool enable);
            static void reverseColor(bool enable);
            static void setCursorVisible(bool enable);
            static void setStrikethrough(bool enable);
            static void reset();
        private:
            template<typename T, typename... Args>
            static void formatImpl(std::ostringstream &ostream, const char* format, T arg, Args... args);
            static void formatImpl(std::ostringstream &ostream, const char* format);
            template<typename T, typename... Args>
            static void parseFormatSpec(std::ostringstream &ostream, const char*& format, T arg, Args... args);
            template<typename T, typename... Args>
            static void handleAlternateForm(std::ostringstream &ostream, const char*& format,
                                            std::string& spec, T arg, Args... args);
            template<typename T>
            static void appendArgs(std::ostringstream& ostream, T&& t);
            static void appendFormatText(std::ostringstream& ostream, const char* format);
            static bool printFormattedText(const std::string& str);
#ifdef TINY_CPP_MY_OS_WINDOWS
            static void* _old_console;
            static unsigned long _old_console_handle;
        };
#elif defined(TINY_CPP_MY_OS_UNIX)
            static struct termios _old_terminal;
            static bool _is_in_raw_mode;
        };
#endif

    }
}

#include "Terminal_Int.hpp"
#endif //TINY_CPP_TUI_TERMINAL_HPP

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
