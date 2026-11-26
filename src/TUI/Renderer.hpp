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

#pragma once
#ifndef TINY_CPP_TUI_RENDERER_HPP
#define TINY_CPP_TUI_RENDERER_HPP
#include "Terminal.hpp"

#include <vector>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

#include "TUI.hpp"


namespace Tiny {
    namespace TUI {
        struct RGB {
            uint8_t r, g, b;

            RGB() : r(0), g(0), b(0) {}
            RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
            RGB(uint32_t code) : r(code >> 16), g((code >> 8) & 0xff), b(code & 0xff) {}
            bool operator==(const RGB& other) const {
                return r == other.r && g == other.g && b == other.b;
            }
            bool operator!=(const RGB& other) const {
                return r != other.r || g != other.g || b != other.b;
            }
            uint32_t toHex() const {
                return static_cast<uint32_t>(r) << 16 | static_cast<uint32_t>(g) << 8 | static_cast<uint32_t>(b);
            }
            void set(uint32_t code) {
                r = code >> 16;
                g = (code >> 8) & 0xff;
                b = code & 0xff;
            }
        };

        class Utf8Char {
        public:
            Utf8Char() : _dp_size(0), _ch_size(0), _data() {}
            Utf8Char(const char* string, uint8_t size = 0);
            Utf8Char(char c);
            Utf8Char& operator=(const char* str) noexcept {
                set(str);
                return *this;
            }
            Utf8Char& operator=(char c) noexcept {
                set(c);
                return *this;
            }

            void set(char C);
            void set(const char* string, uint8_t size = 0);
            void clear();

            const char* data() const;
            uint32_t size() const;
            uint32_t length() const;
            uint32_t displayLength() const;
        private:
            uint32_t _dp_size;  // Display chars size
            uint32_t _ch_size;  // Chars size
            char _data[16];
        };

        struct TermStyle {
            enum Property : uint8_t {
                Bolder            = 1,
                Dark              = 2,
                Italic            = 4,
                Underline         = 8,
                Blinking          = 16,
                Reverse           = 32,
                Strikethrough     = 64
            };

            enum Intensity : uint8_t { Back = 1, Fore = 2 };

            uint8_t property{};   // Bitset, used enum `TUI::Style::Property` is better.
            bool    used_rgb{};   // If set `true`, need set `colors.rgb`, otherwise need set `colors.color` members.
            union {
                struct {
                    Color bg;
                    Color fg;
                } color;
                struct {
                    RGB bg;
                    RGB fg;
                } rgb;
            } colors{};                  // Set background color or foreground color.
            uint8_t intensity{Fore};     // Bitset, used enum `TUI::Style::Intensity` is better.

            TermStyle() {
                colors.color.bg = Color::Default;
                colors.color.fg = Color::Default;
            }
            bool operator==(const TermStyle& other) const {
                bool ok = (property == other.property &&
                       used_rgb == other.used_rgb &&
                       intensity == other.intensity);
                if (!ok) return false;
                return used_rgb ? (colors.rgb.bg == other.colors.rgb.bg &&
                                   colors.rgb.fg == other.colors.rgb.fg) :
                                  (colors.color.bg == other.colors.color.bg &&
                                   colors.color.fg == other.colors.color.fg);
            }

            bool operator!=(const TermStyle& other) const {
                bool ok = (property == other.property &&
                       used_rgb == other.used_rgb &&
                       intensity == other.intensity);
                if (!ok) return true;
                return used_rgb ? (colors.rgb.bg != other.colors.rgb.bg ||
                                   colors.rgb.fg != other.colors.rgb.fg) :
                                  (colors.color.bg != other.colors.color.bg ||
                                   colors.color.fg != other.colors.color.fg);
            }

            bool isDefault() const {
                return property == 0 && !used_rgb && colors.color.bg == Color::Default &&
                       colors.color.fg == Color::Default && intensity == Intensity::Fore;
            }

            void reset() {
                property = 0;
                used_rgb = false;
                colors.color.bg = Color::Default;
                colors.color.fg = Color::Default;
                intensity = 2;
            }
        };

        struct Border {
            Utf8Char left_top{'+'};
            Utf8Char left{'|'};
            Utf8Char left_bottom{'+'};
            Utf8Char right_top{'+'};
            Utf8Char right{'|'};
            Utf8Char right_bottom{'+'};
            Utf8Char top{'-'};
            Utf8Char bottom{'-'};

            Border() = default;
            void makeRect() {
                left = "\u2502";
                right = "\u2502";
                top = "\u2500";
                bottom = "\u2500";
                left_top = "\u250c";
                left_bottom = "\u2514";
                right_top = "\u2510";
                right_bottom = "\u2518";
            }
            void makeRoundedRect() {
                left = "\u2502";
                right = "\u2502";
                top = "\u2500";
                bottom = "\u2500";
                left_top = "\u256d";
                left_bottom = "\u2570";
                right_top = "\u256e";
                right_bottom = "\u256f";
            }
        };

        using TermStyleList = std::vector<TermStyle>;

        class TermBuffer {
            struct Cell {
                Utf8Char  utf8_char{};
                TermStyle style{};
                uint32_t  offset{};
                bool      is_changed{false};

                void set(uint8_t c) {
                    if (utf8_char.length() == 1 && utf8_char.data()[0] == c) return;
                    utf8_char.set(static_cast<char>(c));
                    offset = 1;
                    is_changed = true;
                }

                void set(const char* str, size_t length = 0) {
                    size_t len{length};
                    auto u8_data = U8Code::splitFront(str);
                    if (length == 0) len = u8_data.length();
                    auto temp = utf8_char.data();
                    bool is_equal{true};
                    for (size_t i = 0; i < len; i++) {
                        if (u8_data[i] != temp[i]) {
                            is_equal = false;
                            break;
                        }
                    }
                    if (!is_equal) {
                        utf8_char.set(str, len);
                        offset = utf8_char.displayLength();
                    }
                    is_changed = true;
                }

                void setStyle(const TermStyle& src) {
                    if (style == src) return;
                    style = src;
                    is_changed = true;
                }

                void reset() {
                    utf8_char.clear();
                    style.reset();
                    offset = 0;
                    is_changed = true;
                }
            };

        public:
            static TermBuffer& self();
            void set(const Position& pos, uint8_t ch, TermStyle style = {});
            void set(uint32_t row, uint32_t col, uint8_t ch, TermStyle style = {});
            void set(const Position& pos, const char* str, TermStyle style = {});
            void set(uint32_t row, uint32_t col, const char* str, TermStyle style = {});
            void setStyle(const Position& pos, TermStyle style);
            void setStyle(uint32_t row, uint32_t col, TermStyle style);
            void fillScreen(const TermStyle& style = {});
            void fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch = ' ', TermStyle style = {});
            void fillRows(uint32_t start_row, uint32_t end_row, const char* str, TermStyle style = {});
            void fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch = ' ', TermStyle style = {});
            void fillCols(uint32_t start_col, uint32_t end_col, const char* str, TermStyle style = {});
            void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch = ' ', TermStyle style = {});
            void fillRect(const Position& start_pos, const Position& end_pos, const char* str, TermStyle style = {});
            void drawBorder(const Position& start_pos, const Position& end_pos, Border border, TermStyle style = {});
            template<typename ... Args>
            size_t drawString(const Position &pos, const std::string &format, Args... args);
            template<typename ... Args>
            size_t drawString(const Position &pos, const std::string &format, const TermStyle &style, Args... args);
            template<typename ... Args>
            uint32_t drawString(const Position &pos, const std::string &format, const TermStyleList &styles,
                                Args... args);
            void setResizeEvent(const std::function<void(TermBuffer&)>& event);

            void clear();
            void present();

            const Utf8Char& charAt(const Position& position) const;
            const Utf8Char& charAt(uint32_t row, uint32_t col) const;
            const TermStyle& styleAt(const Position& position) const;
            const TermStyle& styleAt(uint32_t row, uint32_t col) const;
            bool isValid(const Position& position) const;
            bool isValid(uint32_t row, uint32_t col) const;
            const Size& size() const;

            virtual ~TermBuffer();
            TermBuffer(const TermBuffer&)            = delete;
            TermBuffer& operator=(const TermBuffer&) = delete;
            TermBuffer(TermBuffer&&)                 = delete;
            TermBuffer& operator=(TermBuffer&&)      = delete;
        protected:
            TermBuffer();
            virtual void renderEvent();
            virtual void resizeEvent();
        private:
            static void initSignals();
            static void cleanUpSignals(int signal);
            static void resizeTerm();
            bool setChars(const Position &pos, const std::string &str, const TermStyle &style = {}, size_t *count = nullptr);

            struct FmtStyle {
                std::string text{};
                size_t index{};
            };
            std::vector<FmtStyle> parseString(const std::string &str);

            uint32_t setFmtStyle(const Position &pos, const std::vector<FmtStyle> &style,
                                 const TermStyleList &style_list);
            void parseStyle(const TermStyle& style);
            bool cellOf(const Position& pos, Cell* cell) const;
            bool cellOf(uint32_t row, uint32_t col, Cell* cell = nullptr) const;

            std::vector<std::vector<Cell>> _buffers;
            Size _term_size;
            std::mutex _buffers_mutex;
            std::condition_variable _buffers_cond_var;
            std::mutex _resize_mutex{};
            std::function<void(TermBuffer&)> _resize_event{};
            std::thread _resize_win_signal{};
            static std::thread::id _th_id;
            std::atomic<bool> _is_resizing{}, _is_rendering{}, _is_running{};
        };


        template<typename ... Args>
        size_t TermBuffer::drawString(const Position &pos, const std::string &format, Args... args) {
            if (!cellOf(pos, nullptr)) return 0;
            auto fmt = Terminal::formatString(format.data(), args...);
            size_t ret{};
            setChars(pos, fmt, {}, &ret);
            return ret;
        }

        template<typename ... Args>
        size_t TermBuffer::drawString(const Position &pos, const std::string &format, const TermStyle &style,
                                      Args... args) {
            if (!cellOf(pos, nullptr)) return 0;
            auto fmt = Terminal::formatString(format.data(), args...);
            size_t ret{};
            setChars(pos, fmt, style, &ret);
            return ret;
        }

        template<typename ... Args>
        uint32_t TermBuffer::drawString(const Position &pos, const std::string &format, const TermStyleList &styles,
                                        Args... args) {
            if (!cellOf(pos, nullptr)) return 0;
            auto fmt = Terminal::formatString(format.data(), args...);
            return setFmtStyle(pos, parseString(fmt), styles);
        }
    }
}

#endif //TINY_CPP_TUI_RENDERER_HPP

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