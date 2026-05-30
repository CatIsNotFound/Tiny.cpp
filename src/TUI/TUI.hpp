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
#include <cstring>

namespace Tiny {
    namespace TUI {
        std::string splitFront(const char* data);
        std::vector<std::string> splitUTF8(const char* data);

        struct Char {
            std::string data;
            uint8_t display_length;

            Char() : data(" "), display_length(1) {}
            Char(const char* data) : data(splitFront(data)), display_length(strlen(data)) {}
            Char(const std::string& data) : data(splitFront(data.data())), display_length(data.size()) {}
            Char& operator=(const std::string& ch) {
                this->data = ch;
                this->display_length = ch.length();
                return *this;
            }
            Char& operator=(const char* ch) {
                this->data = ch;
                this->display_length = this->data.length();
                return *this;
            }
            Char& operator=(const Char& ch) {
                this->data = ch.data;
                this->display_length = this->data.length();
                return *this;
            }
            bool operator==(const Char& other) const {
                if (this->data != other.data) return false;
                if (this->display_length != other.display_length) return false;
                return true;
            }
            bool operator!=(const Char& other) const {
                if (this->data != other.data) return true;
                if (this->display_length != other.display_length) return true;
                return false;
            }
        };

        class Renderer {
        public:
            struct Style {
                uint16_t property;
                Color bg_color;
                Color fg_color;

                Style() : property(), bg_color(), fg_color(Color::Default) {}

                void reset() {
                    property = 0;
                    bg_color = Color::Black;
                    fg_color = Color::Default;
                }

                bool operator==(const Style& other) const {
                    if (property != other.property) return false;
                    if (bg_color != other.bg_color) return false;
                    if (fg_color != other.fg_color) return false;
                    return true;
                }

                bool operator!=(const Style& other) const {
                    if (property != other.property) return true;
                    if (bg_color != other.bg_color) return true;
                    if (fg_color != other.fg_color) return true;
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
            void setStrF(const Position& pos, const char* format, Args... args) {
                auto f_text = Terminal::formatString(format, args...);
                setChars(pos, f_text);
            }
            template<typename ... Args>
            void setSSF(const Position& pos, const char* format, const std::vector<Style>& style, Args... args) {
                auto f_text = Terminal::formatString(format, args...);
                formatStyle(pos, f_text, style);
                setChars(pos, f_text);
            }
            void fillRect(const Position& start_pos, const Position& end_pos, uint8_t ch, Style style = {});
            void fillRect(const Position& start_pos, const Position& end_pos, const std::string& str, Style style = {});
            void drawBorder(const Position& start_pos, const Position& end_pos, Corner corner, Style style = {});
            void unset(const Position& pos);
            void unset(uint32_t x, uint32_t y);

            void refresh();
            void clear();
            void present();
        protected:
            Renderer();
            virtual void renderEvent();
            virtual void resizeEvent();
        private:
            void setChars(const Position& pos, const std::string& str);
            void formatStyle(const Position& pos, const char* format, const std::vector<Style>& styles);
            std::vector<std::vector<Cell>> _buffer;
            std::vector<std::vector<Cell>> _front_buffer;
        };



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