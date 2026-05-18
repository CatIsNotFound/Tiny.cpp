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

namespace Tiny {
    template<typename... Args>
    bool TUI::Terminal::printFormat(const char *format, Args... args) {
        std::ostringstream oss;
        formatImpl(oss, format, std::forward<Args>(args)...);
        return printFormattedText(oss.str());;
    }

    template<typename T, typename ... Args>
    void TUI::Terminal::formatImpl(std::ostringstream &ostream, const char *format, T arg, Args... args) {
        while (*format) {
            if (*format == '{' && *(format + 1) == '}') {
                appendArgs(ostream, arg);
                formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                return;
            }
            if (*format == '{' && *(format + 1) == ':') {
                bool no_format = false;
                std::string spec;
                spec += *format;
                spec += *(format + 1);
                format += 2;
                uint8_t int_part = 0, dec_part = 0;
                bool add_int_part = true, is_right_align = true;
                while (*format != '}') {
                    if (*format == '.') {
                        if (!add_int_part) {
                            ostream << spec;
                            no_format = true;
                            break;
                        }
                        add_int_part = false;
                    } else if (*format == '>') {
                        is_right_align = true;  // Right align (Default)
                    } else if (*format == '<') {
                        is_right_align = false;  // Left align
                    } else if (std::isdigit(*format)) {
                        if (add_int_part) {
                            if (int_part > 0) int_part *= 10;
                            int_part += *format - '0';
                        } else {
                            if (dec_part > 0) dec_part *= 10;
                            dec_part += *format - '0';
                        }
                    } else if (*format == 'c' && *(format + 1) == '}') {
                        if (std::is_same<T, char>::value || std::is_same<T, unsigned char>::value) {
                            std::string chs;
                            for (size_t i = 0; i < int_part; i++) chs += arg;
                            ostream << chs;
                            formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                            return;
                        }
                        ostream << spec;
                        no_format = true;
                        break;
                    } else if (*format == 's' && *(format + 1) == '}') {
                        if (std::is_same<T, const char*>::value || std::is_same<T, char*>::value ||
                                std::is_same<T, std::string>::value) {
                            if (!is_right_align) {
                                ostream << std::left << std::setw(int_part) << arg;
                            } else {
                                ostream << std::right << std::setw(int_part) << arg;
                            }
                            formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                            return;
                        }
                        ostream << spec;
                        no_format = true;
                        break;
                    } else if (*format == 'f' && *(format + 1) == '}') {
                        if (std::is_floating_point<T>::value) {
                            if (is_right_align) {
                                ostream << std::left << std::setw(int_part)
                                        << std::fixed << std::setprecision(dec_part) << arg;
                            } else {
                                ostream << std::right << std::setw(int_part)
                                        << std::fixed << std::setprecision(dec_part) << arg;
                            }
                            formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                            return;
                        }
                        ostream << spec;
                        no_format = true;
                        break;
                    } else if (*format == 'e' && *(format + 1) == '}') {
                        if (std::is_floating_point<T>::value) {
                            if (is_right_align) {
                                ostream << std::left << std::setw(int_part)
                                        << std::scientific << std::setprecision(dec_part) << arg;
                            } else {
                                ostream << std::right << std::setw(int_part)
                                        << std::scientific << std::setprecision(dec_part) << arg;
                            }
                            formatImpl(ostream, format + 2, std::forward<Args>(args)...);
                            return;
                        }
                        ostream << spec;
                        no_format = true;
                        break;
                    }
                    spec += *format++;
                }
                if (no_format) continue;
                if (*format == '}') {
                    if (!add_int_part && std::is_floating_point<T>::value) {
                        if (is_right_align) {
                            ostream << std::left << std::setw(int_part)
                                    << std::defaultfloat << std::setprecision(dec_part) << arg;
                        } else {
                            ostream << std::right << std::setw(int_part)
                                    << std::defaultfloat << std::setprecision(dec_part) << arg;
                        }
                        formatImpl(ostream, format + 1, std::forward<Args>(args)...);
                        return;
                    }
                    ostream << spec;
                }
            }
            ostream << *(format++);
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
        ostream << format;
    }
}

#endif //TINY_CPP_TUI_INT_HPP

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
