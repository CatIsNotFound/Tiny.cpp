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

namespace Tiny {
    std::string TUI::splitFront(const char *data) {
        std::string str;
        if (data == nullptr) return str; 
        uint8_t ch = *data;
        if ((ch & 0xE0) == 0xc0) {
            str += *data++;
            str += *data;
        } else if ((ch & 0xF0) == 0xE0) {
            str += *data++;
            str += *data++;
            str += *data;
        } else if ((ch & 0xF8) == 0xF0) {
            str += *data++;
            str += *data++;
            str += *data++;
            str += *data;
        } else {
            str += *data;
        }
        return str;
    }

    std::vector<std::string> TUI::splitUTF8(const char *data) {
        std::vector<std::string> result;
        if (data == nullptr) return result; 
        while (*data) {
            std::string str;
            uint8_t ch = *data;
            if ((ch & 0xE0) == 0xc0) {
                str += *data++;
                str += *data;
            } else if ((ch & 0xF0) == 0xE0) {
                str += *data++;
                str += *data++;
                str += *data;
            } else if ((ch & 0xF8) == 0xF0) {
                str += *data++;
                str += *data++;
                str += *data++;
                str += *data;
            } else {
                str += *data;
            }
            result.push_back(str);
        }
        return result;
    }


    TUI::Renderer & TUI::Renderer::self() {
        static Renderer instance;
        return instance;
    }

    TUI::Renderer::~Renderer() {
        Terminal::leaveRawMode();
    }

    void TUI::Renderer::set(const Position &pos, uint8_t ch, Style style) {
        char c[2] = {static_cast<char>(ch), 0};
        _front_buffer[pos.row][pos.column].set(c, style);
    }

    void TUI::Renderer::set(uint32_t x, uint32_t y, uint8_t ch, Style style) {
        char c[2] = {static_cast<char>(ch), 0};
        _front_buffer[y][x].set(c, style);
    }

    void TUI::Renderer::set(const Position &pos, const std::string &str, Style style) {
        _front_buffer[pos.row][pos.column].set(splitFront(str.c_str()).c_str(), style);
    }

    void TUI::Renderer::set(uint32_t x, uint32_t y, const std::string &str, Style style) {
        _front_buffer[y][x].set(splitFront(str.c_str()).c_str(), style);
    }

    void TUI::Renderer::fillRect(const Position &start_pos, const Position &end_pos, uint8_t ch, Style style) {
        char temp[2] = {static_cast<char>(ch), 0};
        for (uint32_t r = start_pos.row; r <= end_pos.row; r++) {
            for (uint32_t c = start_pos.column; c <= end_pos.column; c++) {
                _front_buffer[r][c].set(temp, style);
            }
        }
    }

    void TUI::Renderer::fillRect(const Position &start_pos, const Position &end_pos, const std::string &str,
        Style style) {
        for (uint32_t r = start_pos.row; r <= end_pos.row; r++) {
            for (uint32_t c = start_pos.column; c <= end_pos.column; c++) {
                _front_buffer[r][c].set(splitFront(str.c_str()).c_str(), style);
            }
        }
    }

    void TUI::Renderer::drawBorder(const Position &start_pos, const Position &end_pos, Corner corner, Style style) {
        // left top
        _front_buffer[start_pos.row][start_pos.column].set(corner.left_top.data.c_str(), style);
        // left bottom
        _front_buffer[end_pos.row][start_pos.column].set(corner.left_bottom.data.c_str(), style);
        // right top
        _front_buffer[start_pos.row][end_pos.column].set(corner.right_top.data.c_str(), style);
        // right bottom
        _front_buffer[end_pos.row][end_pos.column].set(corner.right_bottom.data.c_str(), style);
        // top / bottom
        for (uint32_t c = start_pos.column + 1; c < end_pos.column; c++) {
            _front_buffer[start_pos.row][c].set(corner.top.data.c_str(), style);
            _front_buffer[end_pos.row][c].set(corner.bottom.data.c_str(), style);
        }
        // left / right
        for (uint32_t r = start_pos.row + 1; r < end_pos.row; r++) {
            _front_buffer[r][start_pos.column].set(corner.left.data.c_str(), style);
            _front_buffer[r][end_pos.column].set(corner.right.data.c_str(), style);
        }
    }

    void TUI::Renderer::unset(const Position &pos) {
        _front_buffer[pos.row][pos.column].reset();
    }

    void TUI::Renderer::unset(uint32_t x, uint32_t y) {
        _front_buffer[y][x].reset();
    }

    void TUI::Renderer::refresh() {
        resizeEvent();
    }

    void TUI::Renderer::clear() {
        for (auto& front : _front_buffer) {
            for (auto& i : front) {
                i.reset();
            }
        }
    }

    void TUI::Renderer::present() {
        _buffer = _front_buffer;
        renderEvent();
    }

    TUI::Renderer::Renderer() {
        Terminal::enterRawMode();
        auto size = Terminal::screenSize();
        _front_buffer.resize(size.height);
        for (auto &i : _front_buffer) {
            i.resize(size.width);
        }
    }

    void TUI::Renderer::renderEvent() {
        Terminal::clearScreen();
        size_t row = 0;
        for (auto& bufs : _buffer) {
            size_t col = 0;
            for (auto& b : bufs) {
                if (!b.is_dirty) {
                    Terminal::moveCursor(row, col);
                    Terminal::print(b.data.data);
                }
            }
            Terminal::moveCursor(++row, 0);
        }
    }

    void TUI::Renderer::resizeEvent() {
        auto size = Terminal::screenSize();
        _front_buffer.resize(size.height);
        for (auto &i : _front_buffer) {
            i.resize(size.width);
        }
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
