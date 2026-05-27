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

#ifndef TINY_CPP_TUI_INT_HPP
#define TINY_CPP_TUI_INT_HPP
#include "Terminal.hpp"
#include <type_traits>
#include <cmath>
#include <iomanip>
#include <bitset>
#include <cassert>

namespace Tiny {
    namespace TUI {
        template<typename T>
        struct FormatHandler {
            static void handleCharRepeat(std::ostringstream& ostream, const T&, int) {}
            static void handleString(std::ostringstream& ostream, const T& arg, int width, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << arg;
                } else {
                    ostream << std::left << std::setw(width) << arg;
                }
            }
            
            static void handleFloat(std::ostringstream& ostream, const T& arg, int width, int decPart, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width)
                            << std::fixed << std::setprecision(decPart) << arg;
                } else {
                    ostream << std::left << std::setw(width)
                            << std::fixed << std::setprecision(decPart) << arg;
                }
            }
            
            static void handleScientific(std::ostringstream& ostream, const T& arg, int width, int decPart, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width)
                            << std::scientific << std::setprecision(decPart) << arg;
                } else {
                    ostream << std::left << std::setw(width)
                            << std::scientific << std::setprecision(decPart) << arg;
                }
            }
            
            static void handleDefaultFloat(std::ostringstream& ostream, const T& arg, int width, int decPart, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width)
                            << std::setprecision(decPart) << arg;
                } else {
                    ostream << std::left << std::setw(width)
                            << std::setprecision(decPart) << arg;
                }
            }
            
            static void handleBinary(std::ostringstream& ostream, const T& arg) {
                std::bitset<sizeof(T) * 8> bits(arg);
                auto str = bits.to_string();
                size_t pos = str.find_first_not_of('0');
                if (pos != std::string::npos) {
                    ostream << str.substr(pos);
                } else {
                    ostream << "0";
                }
            }
            
            static void handleOctal(std::ostringstream& ostream, const T& arg) {
                ostream << std::oct << arg;
            }
            
            static void handleDecimal(std::ostringstream& ostream, const T& arg) {
                ostream << std::dec << arg;
            }
            
            static void handleHex(std::ostringstream& ostream, const T& arg) {
                ostream << std::hex << arg;
            }
        };

        template<>
        struct FormatHandler<bool> {
            static void handleCharRepeat(std::ostringstream& ostream, bool arg, int width) {
                ostream << std::string(width, arg ? '1' : '0');
            }

            static void handleString(std::ostringstream& ostream, bool arg, int width, bool isRightAlign) {
                std::string s = arg ? "true" : "false";
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << s;
                } else {
                    ostream << std::left << std::setw(width) << s;
                }
            }
            static void handleFloat(std::ostringstream& ostream, bool arg, int, int, bool) {
                ostream << arg;
            }
            static void handleScientific(std::ostringstream& ostream, bool arg, int, int, bool) {
                ostream << arg;
            }
            static void handleDefaultFloat(std::ostringstream& ostream, bool arg, int, int, bool) {
                ostream << arg;
            }
            static void handleBinary(std::ostringstream& ostream, bool arg) {
                ostream << arg;
            }
            static void handleOctal(std::ostringstream& ostream, char arg) {
                ostream << arg;
            }
            static void handleDecimal(std::ostringstream& ostream, char arg) {
                ostream << arg;
            }
            static void handleHex(std::ostringstream& ostream, char arg) {
                ostream << arg;
            }
        };
        
        template<>
        struct FormatHandler<char> {
            static void handleCharRepeat(std::ostringstream& ostream, char arg, int width) {
                ostream << std::string(width, arg);
            }
            
            static void handleString(std::ostringstream& ostream, char arg, int width, bool isRightAlign) {
                std::string s(1, arg);
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << s;
                } else {
                    ostream << std::left << std::setw(width) << s;
                }
            }

            static void handleFloat(std::ostringstream&, char, int, int, bool) {}
            static void handleScientific(std::ostringstream&, char, int, int, bool) {}
            static void handleDefaultFloat(std::ostringstream&, char, int, int, bool) {}
            static void handleBinary(std::ostringstream& ostream, char arg) {
                std::bitset<8> bits(arg);
                ostream << bits.to_string();
            }
            static void handleOctal(std::ostringstream& ostream, char arg) {
                ostream << std::oct << static_cast<int>(arg);
            }
            static void handleDecimal(std::ostringstream& ostream, char arg) {
                ostream << std::dec << static_cast<int>(arg);
            }
            static void handleHex(std::ostringstream& ostream, char arg) {
                ostream << std::hex << static_cast<int>(arg);
            }
        };
        
        
        template<>
        struct FormatHandler<unsigned char> {
            static void handleCharRepeat(std::ostringstream& ostream, unsigned char arg, int width) {
                ostream << std::string(width, static_cast<char>(arg));
            }
            
            static void handleString(std::ostringstream& ostream, unsigned char arg, int width, bool isRightAlign) {
                std::string s(1, static_cast<char>(arg));
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << s;
                } else {
                    ostream << std::left << std::setw(width) << s;
                }
            }
            
            static void handleFloat(std::ostringstream&, unsigned char, int, int, bool) {}
            static void handleScientific(std::ostringstream&, unsigned char, int, int, bool) {}
            static void handleDefaultFloat(std::ostringstream&, unsigned char, int, int, bool) {}
            static void handleBinary(std::ostringstream& ostream, unsigned char arg) {
                std::bitset<8> bits(arg);
                ostream << bits.to_string();
            }
            static void handleOctal(std::ostringstream& ostream, unsigned char arg) {
                ostream << std::oct << static_cast<int>(arg);
            }
            static void handleDecimal(std::ostringstream& ostream, unsigned char arg) {
                ostream << std::dec << static_cast<int>(arg);
            }
            static void handleHex(std::ostringstream& ostream, unsigned char arg) {
                ostream << std::hex << static_cast<int>(arg);
            }
        };

        template<>
        struct FormatHandler<const char*> {
            static void handleCharRepeat(std::ostringstream&, const char*, int) {}
            static void handleString(std::ostringstream& ostream, const char* arg, int width, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << arg;
                } else {
                    ostream << std::left << std::setw(width) << arg;
                }
            }
            static void handleFloat(std::ostringstream&, const char*, int, int, bool) {}
            static void handleScientific(std::ostringstream&, const char*, int, int, bool) {}
            static void handleDefaultFloat(std::ostringstream&, const char*, int, int, bool) {}
            static void handleBinary(std::ostringstream&, const char*) {}
            static void handleOctal(std::ostringstream&, const char*) {}
            static void handleDecimal(std::ostringstream&, const char*) {}
            static void handleHex(std::ostringstream&, const char*) {}
        };

        template<>
        struct FormatHandler<std::string> {
            static void handleCharRepeat(std::ostringstream&, const std::string&, int) {}
            static void handleString(std::ostringstream& ostream, const std::string& arg, int width, bool isRightAlign) {
                if (isRightAlign) {
                    ostream << std::right << std::setw(width) << arg;
                } else {
                    ostream << std::left << std::setw(width) << arg;
                }
            }
            static void handleFloat(std::ostringstream&, const std::string&, int, int, bool) {}
            static void handleScientific(std::ostringstream&, const std::string&, int, int, bool) {}
            static void handleDefaultFloat(std::ostringstream&, const std::string&, int, int, bool) {}
            static void handleBinary(std::ostringstream&, const std::string&) {}
            static void handleOctal(std::ostringstream&, const std::string&) {}
            static void handleDecimal(std::ostringstream&, const std::string&) {}
            static void handleHex(std::ostringstream&, const std::string&) {}
        };
    }
    
    template<typename... Args>
    bool TUI::Terminal::printFormat(const char *format, Args... args) {
        std::ostringstream oss;
        formatImpl(oss, format, std::forward<Args>(args)...);
        return printFormattedText(oss.str());
    }

    template<typename T, typename ... Args>
    void TUI::Terminal::formatImpl(std::ostringstream &ostream, const char *format, T arg, Args... args) {
        while (*format) {
            if (*format == '{' && *(format + 1) == '}') {
                appendArgs(ostream, arg);
                formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                return;
            }
            if (*format == '{' && *(format + 1) == '{') {
                ostream << '{';
                format += 2;
                continue;
            }
            if (*format == '}' && *(format + 1) == '}') {
                ostream << '}';
                format += 2;
                continue;
            }

            if (*format == '{' && *(format + 1) == ':') {
                parseFormatSpec<T, Args...>(ostream, format, arg, std::forward<Args>(args)...);
                return;
            }
            ostream << *(format++);
        }
    }
    
    template<typename T, typename... Args>
    void TUI::Terminal::parseFormatSpec(std::ostringstream &ostream, const char*& format,
                                        T arg, Args... args) {
        format += 2; 
        int intPart = 0, decPart = 0;
        bool addIntPart = true, isRightAlign = true;
        std::string spec;
        spec += "{:";
        
        while (*format && *format != '}') {
            if (*format == '.') {
                if (!addIntPart) {
                    ostream << spec;
                    formatImpl(ostream, format, arg, std::forward<Args>(args)...);
                    return;
                }
                addIntPart = false;
                spec += *format;
            } else if (*format == '>') {
                isRightAlign = true;
                spec += *format;
            } else if (*format == '<') {
                isRightAlign = false;
                spec += *format;
            } else if (*format == '#') {
                spec += *format;
                handleAlternateForm<T, Args...>(ostream, format, spec, arg, std::forward<Args>(args)...);
                return;
            } else if (std::isdigit(*format)) {
                if (addIntPart) {
                    if (intPart > 0) intPart *= 10;
                    intPart += *format - '0';
                } else {
                    if (decPart > 0) decPart *= 10;
                    decPart += *format - '0';
                }
                spec += *format;
            } else if (*format == 'c' && *(format + 1) == '}') {
                FormatHandler<T>::handleCharRepeat(ostream, arg, intPart);
                format += 2;
                formatImpl(ostream, format, std::forward<Args>(args)...);
                return;
            } else if (*format == 's' && *(format + 1) == '}') {
                FormatHandler<T>::handleString(ostream, arg, intPart, isRightAlign);
                format += 2;
                formatImpl(ostream, format, std::forward<Args>(args)...);
                return;
            } else if (*format == 'f' && *(format + 1) == '}') {
                FormatHandler<T>::handleFloat(ostream, arg, intPart, decPart, isRightAlign);
                format += 2;
                formatImpl(ostream, format, std::forward<Args>(args)...);
                return;
            } else if (*format == 'e' && *(format + 1) == '}') {
                FormatHandler<T>::handleScientific(ostream, arg, intPart, decPart, isRightAlign);
                format += 2;
                formatImpl(ostream, format, std::forward<Args>(args)...);
                return;
            } else {
                ostream << spec;
                formatImpl(ostream, format, arg, std::forward<Args>(args)...);
                return;
            }
            format++;
        }
        
        
        if (*format == '}') {
            if (!addIntPart) {
                
                FormatHandler<T>::handleDefaultFloat(ostream, arg, intPart, decPart, isRightAlign);
            } else {
                
                ostream << arg;
            }
            format++;
            formatImpl(ostream, format, std::forward<Args>(args)...);
        }
    }
    
    template<typename T, typename... Args>
    void TUI::Terminal::handleAlternateForm(std::ostringstream &ostream, const char*& format,
                                             std::string& spec, T arg, Args... args) {
        format++; 
        spec += *format;
        
        switch (*format) {
            case 'b':
                FormatHandler<T>::handleBinary(ostream, arg);
                break;
            case 'o':
                FormatHandler<T>::handleOctal(ostream, arg);
                break;
            case 'd':
                FormatHandler<T>::handleDecimal(ostream, arg);
                break;
            case 'x':
            case 'X':
                FormatHandler<T>::handleHex(ostream, arg);
                break;
            default:
                ostream << spec;
                formatImpl(ostream, format, arg, std::forward<Args>(args)...);
                return;
        }
        format++;
        if (*format == '}') {
            format++;
            formatImpl(ostream, format, std::forward<Args>(args)...);
        }
    }

    template<typename T>
    void TUI::Terminal::appendArgs(std::ostringstream &ostream, T &&t) {
        ostream << std::forward<T>(t);
    }

    inline void TUI::Terminal::appendFormatText(std::ostringstream &ostream, const char *format) {
        ostream << format;
    }

    inline void TUI::Terminal::formatImpl(std::ostringstream &ostream, const char *format) {
        while (*format) {
            if (*format == '{' && *(format + 1) == '{') {
                ostream << '{';
                format += 2;
                continue;
            }
            if (*format == '}' && *(format + 1) == '}') {
                ostream << '}';
                format += 2;
                continue;
            }
            ostream << *(format++);
        }
    }
}

#endif 

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
