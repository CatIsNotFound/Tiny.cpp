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

#ifndef TINY_CPP_TUI_FORMAT_HPP
#define TINY_CPP_TUI_FORMAT_HPP
#include <algorithm>
#include <sstream>
#include <string>
#include <tuple>
#include <bitset>
#include <utility>
#include <iomanip>
#include <limits>
#include <cmath>
#include <cctype>
#include <complex>
#include <cstdint>

namespace Tiny {
    namespace TUI {
        namespace Format {
            enum class Align : uint8_t {
                Auto,
                Left,
                Center,
                Right
            };

            struct FmtSpec {
                /** Specify the character to fill. Default value: ` `(space) **/
                char fill{' '};
                Align align{};
                /** Whether the value is a floating-point type. **/
                bool sharp{};
                /** Specify the data type. Available value: `d` `x` `X` `o` `b` `f` `e` `g` `s` `c` `p` **/
                char data_type{};
                /** Control the width of the string. `-1` means no width is specified. **/
                int32_t width{-1};
                int32_t precision{-1};
            };

            struct Sink {
                std::string& output;
                void append(const std::string& data, size_t length = std::string::npos) {
                    if (length == std::string::npos) {
                        output.append(data);
                        return;
                    }
                    output.append(data, length);
                }
                void append(char c, size_t length = 1) {
                    while (length--) {
                        output.push_back(c);
                    }
                }
            };

            template <typename T>
            static std::string digitToString(T value, uint8_t base = 10, bool upper_letter = false, bool* ok = nullptr) {
                if (base > 16 || base < 2) {
                    if (ok) *ok = false;
                    return {};
                }
                std::string s;
                bool flag{};
                if (!std::is_unsigned<T>::value && value < 0) {
                    value *= -1;
                    flag = true;
                }
                do {
                    const auto D = value % base;
                    if (D >= 10) s.push_back(upper_letter ? 'A' + D - 10 : 'a' + D - 10);
                    else s.push_back('0' + D);
                    value /= base;
                } while (value);
                if (flag) s.push_back('-');
                std::reverse(s.begin(), s.end());
                if (ok) *ok = true;
                return s;
            }

            static void formatTo(Sink& sink, const std::string& str_view, const FmtSpec& fmt);

            template <typename T>
            static void formatBaseCode(Sink& dest, T value, const FmtSpec& fmt) {
                if (fmt.sharp) {
                    if (fmt.data_type == 'b') {
                        if (value == 0) {
                            formatTo(dest, "0", fmt);
                            return;
                        }
                        std::bitset<sizeof(T) * 8> bits(value);
                        std::string temp(sizeof(T) * 8, '0');
                        int64_t last_one{};
                        for (size_t i = 0; i < temp.size(); ++i) {
                            temp[i] = bits.test(i) ? '1' : '0';
                            if (temp[i] == '1') last_one = static_cast<int64_t>(i);
                        }
                        const int64_t D = last_one >= 8 ? last_one + 1 : 8;
                        temp.erase(temp.begin() + D, temp.end());

                        std::reverse(temp.begin(), temp.end());
                        formatTo(dest, temp, fmt);
                    } else if (fmt.data_type == 'o') {
                        bool ok{};
                        auto out = digitToString(value, 8, false, &ok);
                        if (ok) formatTo(dest, out, fmt);
                    } else if (fmt.data_type == 'd') {
                        formatTo(dest, std::to_string(value), fmt);
                    } else if (fmt.data_type == 'x') {
                        bool ok{};
                        auto out = digitToString(value, 16, false, &ok);
                        if (ok) formatTo(dest, out, fmt);
                    } else if (fmt.data_type == 'X') {
                        bool ok{};
                        auto out = digitToString(value, 16, true, &ok);
                        if (ok) formatTo(dest, out, fmt);
                    }
                }
            }

            template <typename T, typename E = void>
            struct Formatter {
                static void format(std::string& dest, const T& value, const FmtSpec& fmt) {
                    Sink sink{dest};
                    if (fmt.data_type == 'p') {
                        auto s = reinterpret_cast<size_t>(&value);
                        formatTo(sink, digitToString(s, 16, false), fmt);
                        return;
                    }
                    if (fmt.data_type == '\0' || fmt.data_type == 's') {
                        formatTo(sink, value, fmt);
                    }
                }
            };

            template <typename T>
            struct Formatter<T, typename std::enable_if<std::is_integral<T>::value>::type> {
            private:
                static void fmtBool(Sink& dest, bool value, const FmtSpec& fmt) {
                    if (fmt.data_type == 's') {
                        formatTo(dest, value ? "true" : "false", fmt);
                    } else if (fmt.data_type == 'c') {
                        std::string val(abs(fmt.width), value ? '1' : '0');
                        formatTo(dest, val, fmt);
                    } else {
                        formatTo(dest, value ? "1" : "0", fmt);
                    }
                }

                static void fmtChar(Sink& dest, char value, const FmtSpec& fmt) {
                    if (fmt.sharp) {
                        formatBaseCode(dest, value, fmt);
                        return;
                    }
                    if (fmt.data_type != '\0' && fmt.data_type != 'c' && fmt.data_type != 's') {
                        formatTo(dest, std::to_string(value), fmt);
                    } else if (fmt.data_type == 'c') {
                        std::string val(abs(fmt.width), value);
                        formatTo(dest, val, fmt);
                    } else {
                        std::string val(1, value);
                        formatTo(dest, val, fmt);
                    }
                }

            public:
                static void format(std::string& dest, const T& value, const FmtSpec& fmt) {
                    Sink sink{dest};
                    if (fmt.data_type == 'p') {
                        auto s = reinterpret_cast<size_t>(&value);
                        formatTo(sink, digitToString(s, 16, false), fmt);
                        return;
                    }
                    if (std::is_same<T, bool>::value) {
                        fmtBool(sink, static_cast<bool>(value), fmt);
                    } else if (std::is_same<T, char>::value) {
                        fmtChar(sink, static_cast<char>(value), fmt);
                    } else if (fmt.data_type != 'c') {
                        if (fmt.sharp) {
                            formatBaseCode(sink, value, fmt);
                            return;
                        }
                        formatTo(sink, std::to_string(value), fmt);
                    }
                }
            };

            template <typename T>
            struct Formatter<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
                static void format(std::string& dest, const T& value, const FmtSpec& fmt) {
                    if (fmt.data_type != '\0' && fmt.data_type != 'e' &&
                        fmt.data_type != 'g' && fmt.data_type != 'f' && fmt.data_type != 'p') return;
                    Sink sink{dest};
                    if (fmt.data_type == 'p') {
                        auto v = reinterpret_cast<size_t>(&value);
                        formatTo(sink, digitToString(v, 16, false), fmt);
                        return;
                    }
                    std::ostringstream oss;
                    int pt{fmt.precision > 0 ? fmt.precision : 0};
                    bool need_floor{true};
                    if (fmt.data_type == 'e') {
                        oss << std::scientific;
                        need_floor = false;
                    } else if (fmt.data_type == 'f') {
                        oss << std::fixed;
                        need_floor = false;
                    }
                    if (fmt.precision != -1) {
                        oss << std::setprecision(pt);
                    }
                    oss << value;
                    formatTo(sink, oss.str(), fmt);
                }
            };

            void formatTo(Sink& sink, const std::string& str_view, const FmtSpec& fmt) {
                auto pad = fmt.width > 0 ? fmt.width - str_view.size(): 0;
                if (pad == 0 || fmt.width <= str_view.size()) {
                    sink.append(str_view);
                    return;
                }
                uint64_t left{};
                switch (fmt.align) {
                    case Align::Left:
                        break;
                    case Align::Center:
                        left = pad / 2;
                        break;
                    case Align::Right:
                    case Align::Auto:
                        left = pad;
                        break;
                    default:
                        break;
                }
                sink.append(fmt.fill, left);
                sink.append(str_view);
                sink.append(fmt.fill, pad - left);
            }

            template <size_t I = 0, typename T, typename... Args>
            static T getArg(const std::tuple<Args...>& tuple) {
                return std::get<I>(tuple);
            }

            template <typename T, typename... Args>
            void skipAndFmt(std::string& dest, const FmtSpec& spec, Args&&... args) {
                Formatter<T>::format(dest, std::forward<Args>(args)..., spec);
            }
        }



        template <typename T, typename... Args>
        size_t Terminal::parseSpec(std::string& dest, const char *byte, std::string *output, T value, Args&&... args) {
            Format::FmtSpec fmt_spec;
            std::string temp_str{byte};
            byte += 2;
            int32_t numeric = 0;
            bool parsed_fill_ch{}, parsed_align{}, parsed_width{}, parsed_floating{};
            bool is_closed{}, parsing_width{}, parsing_floating{};
            std::string parsed{"{:"};
            while (*byte) {
                if (*byte == '}') {
                    is_closed = true;
                    byte -= 1;
                    break;
                }
                parsed.push_back(*byte);
                if (!parsed_align) {
                    if (*byte == '<') {
                        fmt_spec.align = Format::Align::Left;
                        parsed_fill_ch = true;
                        parsed_align = true;
                        byte++;
                        continue;
                    }
                    if (*byte == '^') {
                        fmt_spec.align = Format::Align::Center;
                        parsed_fill_ch = true;
                        parsed_align = true;
                        byte++;
                        continue;
                    }
                    if (*byte == '>') {
                        fmt_spec.align = Format::Align::Right;
                        parsed_fill_ch = true;
                        parsed_align = true;
                        byte++;
                        continue;
                    }
                }
                if (*byte == '#' && !fmt_spec.sharp) {
                    fmt_spec.sharp = true;
                    byte++;
                    continue;
                }
                if (!parsed_fill_ch && *byte >= 32 && *byte <= 126) {
                    if (*byte == '*' && *(byte + 1) >= 32 && *(byte + 1) <= 126 &&
                            *(byte + 1) != '<' && *(byte + 1) != '>' && *(byte + 1) != '^') {
                        fmt_spec.fill = *(byte + 1);
                        parsed_fill_ch = true;
                        parsed.push_back(*(byte + 1));
                        byte += 2;
                        continue;
                    }
                }
                if (*(byte + 1) == '}' && *byte != '{') {
                    // 'd' 'x' 'X' 'o' 'b' 'f' 'e' 'g' 's' 'c' 'p'
                    if (*byte == 's' || *byte == 'c' || *byte == 'f' ||
                        *byte == 'e' || *byte == 'g' || *byte == 'p') {
                        fmt_spec.data_type = *byte;
                    } else if (fmt_spec.sharp && (*byte == 'd' || *byte == 'x' ||
                                *byte == 'X' || *byte == 'o' || *byte == 'b')) {
                        fmt_spec.data_type = *byte;
                    } else if (isdigit(*byte)) {
                        numeric = numeric * 10 + *byte - '0';
                        if (!parsed_floating) {
                            fmt_spec.precision = numeric;
                        } else if (!parsed_width) {
                            fmt_spec.width = numeric;
                        }
                        is_closed = true;
                        byte += 2;
                        break;
                    }
                    if (parsing_width && !parsed_width) {
                        fmt_spec.width = numeric;
                    }
                    if (parsing_floating && !parsed_floating) {
                        fmt_spec.precision = numeric;
                    }
                    is_closed = true;
                    byte += 2;
                    break;
                }

                if (!parsed_floating) {
                    if (*byte == '.') {
                        if (parsing_floating) {
                            parsing_floating = false;
                            parsed_floating = true;
                        } else {
                            fmt_spec.data_type = 'g';
                            parsing_floating = true;
                            if (!parsed_width) {
                                parsed_width = true;
                                fmt_spec.width = numeric <= 0 ? -1 : numeric;
                                numeric = 0;
                            }
                        }
                    } else if (parsing_floating && isdigit(*byte)) {
                        numeric = numeric * 10 + *byte - '0';
                    }
                }

                if (!parsed_width) {
                    if (isdigit(*byte)) {
                        if (!parsing_width) parsing_width = true;
                        numeric = numeric * 10 + *byte - '0';
                        byte++;
                        continue;
                    }
                    if (parsing_width) {
                        fmt_spec.width = numeric;
                        parsed_width = true;
                        parsing_width = false;
                        byte++;
                        continue;
                    }
                }

                if (*byte == '{') {
                    std::string tokens = byte;
                    uint8_t bk = 0;
                    size_t dis = 0;
                    for (auto& t : tokens) {
                        if (t == '{') bk++;
                        else if (t == '}') bk--;
                        if (!bk) break;
                        dis++;
                    }
                    if (bk == 0) {
                        // *>{}.{}f}
                        // *>8.{}f}
                        // *>12.8f}
                        std::string new_dest = temp_str.substr(0, parsed.size() - 1);
                        std::string new_buf;
                        renderOnce(new_dest, byte, value);
                        new_dest += temp_str.substr(parsed.size() + dis);
                        if (output) *output = new_dest;
                        return 0;
                    }
                }

                byte++;
            }
            if (!is_closed) {
                dest.append(parsed);
                return parsed.size();
            }

            Format::Formatter<T>::format(dest, value, fmt_spec);
            return parsed.size() + 1;
        }

        template <typename T, typename... Args>
        void Terminal::render(std::string& dest, const char *byte, T arg, Args&&... args) {
            if (!byte) return;
            std::string output;
            while (*byte) {
                if (*byte == '{') {
                    switch (*(byte + 1)) {
                        case '{':
                            dest.push_back('{');
                            byte += 2;
                            continue;
                        case '}':
                            Format::Formatter<T>::format(dest, arg, {});
                            byte += 2;
                            render(dest, byte, std::forward<Args>(args)...);
                            return;
                        case ':':
                            byte += parseSpec(dest, byte, &output, arg, std::forward<Args>(args)...);
                            render(dest, output.empty() ? byte : output.data(), std::forward<Args>(args)...);
                            return;
                        default:
                            break;
                    }
                }
                if (*byte == '}' && *(byte + 1) == '}') {
                    dest.push_back('}');
                    byte += 2;
                }
                dest.push_back(*byte++);
            }
        }

        template <typename T>
        size_t Terminal::renderOnce(std::string& dest, const char* byte, T arg) {
            size_t ret = 0;
            bool is_comment = false;
            while (*byte) {
                if (*byte == '{' && *(byte + 1) == '{') {
                    dest.push_back('{');
                    byte += 2;
                    ret += 2;
                    if (ret == 0) is_comment = true;
                    continue;
                }
                if (*byte == '}' && *(byte + 1) == '}') {
                    dest.push_back('}');
                    byte += 2;
                    ret += 2;
                    if (is_comment) break;
                    continue;
                }
                if (*byte == '{' && *(byte + 1) == '}') {
                    byte += 2;
                    ret += 2;
                    Format::Formatter<T>::format(dest, arg, {});
                    break;
                }
                ret++;
                dest.push_back(*byte++);
            }

            return ret;
        }



        inline void Terminal::render(std::string& dest, const char* byte) {
            if (!byte) return;
            while (*byte) {
                if (*byte == '{' && *(byte + 1) == '{') {
                    dest.push_back('{');
                    byte += 2;
                    continue;
                }
                if (*byte == '}' && *(byte + 1) == '}') {
                    dest.push_back('}');
                    byte += 2;
                    continue;
                }
                if (*byte == '{' && *(byte + 1) == '}') {
                    byte += 2;
                    continue;
                }
                dest.push_back(*byte++);
            }
        }


        template<typename... Args>
        bool Terminal::printFormat(const char *format, Args... args) {
            auto output = formatString(format, std::forward<Args>(args)...);
            return printFormattedText(output, true);
        }

        template<typename... Args>
        std::string Terminal::formatString(const char *format, Args... args) {
            static_assert(sizeof...(Args) <= 100, "Too much arguments to passed!");
            std::string output;
            render(output, format, std::forward<Args>(args)...);
            return output;
        }

        template<typename... Args>
        bool Terminal::printError(const char *format, Args... args) {
            auto output = formatString(format, std::forward<Args>(args)...);
            return printFormattedText(output, false);
        }

        template<typename T>
        Terminal& Terminal::operator<<(const T& expr) {
            if (_print_mode) print(formatString("{}", expr));
            else             printError(formatString("{}", expr));
            return *this;
        }
    }
}

#endif //TINY_CPP_TUI_FORMAT_HPP
