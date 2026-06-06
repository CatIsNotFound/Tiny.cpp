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

#ifndef TINY_CPP_TUI_HPP
#define TINY_CPP_TUI_HPP

#include "Terminal.hpp"
#include <vector>
#include <bitset>
#include <cstring>

namespace Tiny {
    namespace TUI {
        std::string splitFront(const char* data);
        std::vector<std::string> splitUTF8(const char* data);

        struct RGBColor {
            uint8_t r, g, b;

            RGBColor() : r(0), g(0), b(0) {}
            RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
            bool operator==(const RGBColor& other) const {
                return r == other.r && g == other.g && b == other.b;
            }
            bool operator!=(const RGBColor& other) const {
                return r != other.r || g != other.g || b != other.b;
            }
        };

        class Char {
            std::string _data;
            uint8_t _length;
        public:
            Char() : _data(" "), _length(1) {}
            Char(const char* data) : _data(splitFront(data)), _length(strlen(data)) {}
            Char(const std::string& data) : _data(splitFront(data.data())), _length(data.size()) {}
            Char& operator=(const std::string& ch) {
                this->_data = ch;
                this->_length = ch.length();
                return *this;
            }
            Char& operator=(const char* ch) {
                this->_data = ch;
                this->_length = this->_data.length();
                return *this;
            }
            Char& operator=(const Char& ch) {
                this->_data = ch._data;
                this->_length = this->_data.length();
                return *this;
            }
            bool operator==(const Char& other) const {
                if (this->_data != other._data) return false;
                if (this->_length != other._length) return false;
                return true;
            }
            bool operator!=(const Char& other) const {
                if (this->_data != other._data) return true;
                if (this->_length != other._length) return true;
                return false;
            }

            const std::string& data() const { return _data; }
            uint8_t length() const { return _length; }
        };

        class Renderer {
        public:
            struct Style {
                uint8_t property;   // Used enum `Renderer::Style::Property` is better.
                Color bg_color;
                Color fg_color;
                uint8_t intensity;  // 0 = None, 1 = Only Background, 2 = Only Foreground, 3 = All.
                bool used_rgb_color{};  // If set `true`, need set `bg_rgb_color` and `fg_rgb_color` members.
                RGBColor bg_rgb_color;
                RGBColor fg_rgb_color;


                enum Property : uint8_t {
                    Bolder            = 1,
                    Dark              = 2,
                    Italic            = 4,
                    Underline         = 8,
                    Blinking          = 16,
                    Reverse           = 32,
                    Strikethrough     = 64,
                };

                Style() : property(), bg_color(), fg_color(Color::Default), intensity(2) {}

                void reset() {
                    property = 0;
                    intensity = 2;
                    bg_color = Color::Black;
                    fg_color = Color::Default;
                }

                bool isDefault() const {
                    return property == 0 && intensity == 2 &&
                           bg_color == Color::Black && fg_color == Color::Default &&
                           !used_rgb_color;
                }

                bool operator==(const Style& other) const {
                    if (property != other.property) return false;
                    if (bg_color != other.bg_color) return false;
                    if (fg_color != other.fg_color) return false;
                    if (intensity != other.intensity) return false;
                    return true;
                }

                bool operator!=(const Style& other) const {
                    if (property != other.property) return true;
                    if (bg_color != other.bg_color) return true;
                    if (fg_color != other.fg_color) return true;
                    if (intensity != other.intensity) return true;
                    return false;
                }
            };

            struct Cell {
                Char data;
                bool is_dirty;
                Style style;

                Cell() : data(), is_dirty(true), style() {}

                void reset() {
                    data = " ";
                    is_dirty = true;
                    style.reset();
                }

                void set(const char* ch, Style st) {
                    data = ch;
                    style = st;
                    is_dirty = false;
                }
            };

            struct Corner {
                Char left_top{"+"};
                Char left{"|"};
                Char left_bottom{"+"};
                Char right_top{"+"};
                Char right{"|"};
                Char right_bottom{"+"};
                Char top{"-"};
                Char bottom{"-"};
            };

            static Renderer& self();
            virtual ~Renderer();

            void set(const Position& pos, uint8_t ch, Style style = {});
            void set(uint32_t x, uint32_t y, uint8_t ch, Style style = {});
            void set(const Position& pos, const std::string& str, Style style = {});
            void set(uint32_t x, uint32_t y, const std::string& str, Style style = {});
            template<typename ... Args>
            void setStrF(const Position& pos, const char* format, Args... args);
            template<typename ... Args>
            void setSSF(const Position& pos, const char* format, const Style& style, Args... args);
            void fillScreen(const Style& style = {});
            void fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch = ' ', Style style = {});
            void fillRows(uint32_t start_row, uint32_t end_row, const std::string& ch, Style style = {});
            void fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch = ' ', Style style = {});
            void fillCols(uint32_t start_col, uint32_t end_col, const std::string& ch, Style style = {});
            void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch = ' ', Style style = {});
            void fillRect(const Position& start_pos, const Position& end_pos, const std::string& ch, Style style = {});
            void drawBorder(const Position& start_pos, const Position& end_pos, Corner corner, Style style = {});
            void unset(const Position& pos);
            void unset(uint32_t x, uint32_t y);
            void unsetRow(uint32_t row);
            void unsetCol(uint32_t col);
            void unsetRect(const Position &start_pos, const Position &end_pos);

            void clear();
            void present();
        protected:
            Renderer();
            virtual void renderEvent();
            virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
        private:
            void setChars(const Position& pos, const std::string& str, const Style& style = {});
            void setStyle(const Style& style);
            void fillBuffers();
            Size _win_size;
            std::vector<std::vector<Cell>> _buffer;
            std::vector<std::vector<Cell>> _front_buffer;
        };

        template<typename ... Args>
        void Renderer::setStrF(const Position& pos, const char* format, Args... args) {
            auto f_text = Terminal::formatString(format, args...);
            setChars(pos, f_text);
        }

        template<typename ... Args>
        void Renderer::setSSF(const Position& pos, const char* format, const Style& style, Args... args) {
            std::string f_text = Terminal::formatString(format, args...);
            setChars(pos, f_text, style);
        }

        class AbstractWidget {
        public:
            explicit AbstractWidget(const std::string& name, const Position& position, const Size& size);
            virtual ~AbstractWidget();

            void rename(const std::string& name);
            void move(const Position& position);
            void move(uint32_t x, uint32_t y);
            void resize(const Size& size);
            void resize(uint32_t w, uint32_t h);

            [[nodiscard]] const std::string& name() const;
            [[nodiscard]] const Position& position() const;
            [[nodiscard]] const Size& size() const;
        protected:
            virtual void renderEvent() = 0;

        private:
            std::string _name;
            Position _pos;
            Size _size;
        };


    }
}

#endif //TINY_CPP_TUI_HPP

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