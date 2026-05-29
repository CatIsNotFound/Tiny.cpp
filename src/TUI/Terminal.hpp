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

            KEY_BACKSPACE   = 127,

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

        enum SP_Keys : uint8_t {
            SP_KEY_UNKNOWN,
            SP_KEY_F1,
            SP_KEY_F2,
            SP_KEY_F3,
            SP_KEY_F4,
            SP_KEY_F5,
            SP_KEY_F6,
            SP_KEY_F7,
            SP_KEY_F8,
            SP_KEY_F9,
            SP_KEY_F10,
            SP_KEY_F11,
            SP_KEY_F12,
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
            SP_KEY_RIGHT
        };

        const char* getKeyName(uint8_t KEY, SP_Keys SP); 

        class Terminal {
        public:
            static bool enterRawMode();
            static bool leaveRawMode();
            static bool isInRawMode();
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
            static bool setScrollRegion(uint32_t row_start, uint32_t row_end);
            static bool resetScrollRegion();
            static bool flushScreen();
            static std::string readLine();
            static std::wstring readLineW();
            static uint8_t getKey();
            static void getKey(uint8_t& key, SP_Keys& sp_key);

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
            static std::string readLineOnRaw();
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
