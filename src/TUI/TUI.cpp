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

#include "../TUI/TUI.hpp"
#include <algorithm>

#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#include <csignal>
#else
#include <csignal>
#include <clocale>
#include <cwchar>
#include <climits>
#include <unistd.h>
#endif

namespace {
#ifdef TINY_CPP_MY_OS_WINDOWS
    uint32_t strToCodePoint(const std::string& utf8_char) {
        uint8_t ch = utf8_char[0];
        uint32_t cp = 0;
        if ((ch & 0x80) == 0) {                     // 1 字节 ASCII
            cp = ch;
        } else if ((ch & 0xE0) == 0xC0) {           // 2 字节
            cp = (ch & 0x1F) << 6 | (utf8_char[1] & 0x3F);
        } else if ((ch & 0xF0) == 0xE0) {           // 3 字节
            cp = (ch & 0x0F) << 12 | (utf8_char[1] & 0x3F) << 6 | (utf8_char[2] & 0x3F);
        } else if ((ch & 0xF8) == 0xF0) {           // 4 字节
            cp = (ch & 0x07) << 18 | (utf8_char[1] & 0x3F) << 12 |
                 (utf8_char[2] & 0x3F) << 6 | (utf8_char[3] & 0x3F);
        }
        return cp;
    }

    size_t wcwidth(const std::string& str) {
        uint32_t cp = strToCodePoint(str);
        if (cp == 0 || iswcntrl(cp)) return 0;
        if (cp < 0x80) return 1;
        if ((cp >= 0x1100 && cp <= 0x115F) ||   // Hangul Jamo
        (cp >= 0x2600 && cp <= 0x26FF) ||       // Miscellaneous Symbols
        (cp >= 0x2700 && cp <= 0x27FF) ||       // Decorative Symbols
        (cp >= 0x2E80 && cp <= 0x2EFF) ||       // CJK Radicals Supplement
        (cp >= 0x3000 && cp <= 0x303F) ||       // CJK Symbols and Punctuation
        (cp >= 0x3040 && cp <= 0x309F) ||       // Hiragana
        (cp >= 0x30A0 && cp <= 0x30FF) ||       // Katakana
        (cp >= 0x3100 && cp <= 0x312F) ||       // Bopomofo
        (cp >= 0x3130 && cp <= 0x318F) ||       // Hangul Compatibility Jamo
        (cp >= 0x31A0 && cp <= 0x31BF) ||       // Bopomofo Extended
        (cp >= 0x31C0 && cp <= 0x31EF) ||       // CJK Strokes
        (cp >= 0x3200 && cp <= 0x32FF) ||       // Enclosed CJK Letters and Months
        (cp >= 0x3300 && cp <= 0x33FF) ||       // CJK Compatibility
        (cp >= 0x3400 && cp <= 0x4DBF) ||       // CJK Unified Ideographs Extension A
        (cp >= 0x4E00 && cp <= 0x9FFF) ||       // CJK Unified Ideographs
        (cp >= 0xF900 && cp <= 0xFAFF) ||       // CJK Compatibility Ideographs
        (cp >= 0xFE10 && cp <= 0xFE1F) ||       // Vertical Forms
        (cp >= 0xFE30 && cp <= 0xFE4F) ||       // CJK Compatibility Forms
        (cp >= 0xFF00 && cp <= 0xFFEF) ||       // Halfwidth and Fullwidth Forms
        (cp >= 0x1F300 && cp <= 0x1F6FF) ||     // Emoji
        (cp >= 0x1F900 && cp <= 0x1F9FF) ||     // Emoji Extension
        (cp >= 0x20000 && cp <= 0x2A6DF) ||     // CJK Unified Ideographs Extension B
        (cp >= 0x2A700 && cp <= 0x2B73F) ||     // CJK Unified Ideographs Extension C
        (cp >= 0x2B740 && cp <= 0x2B81F) ||     // CJK Unified Ideographs Extension D
        (cp >= 0x2B820 && cp <= 0x2CEAF) ||     // CJK Unified Ideographs Extension E
        (cp >= 0x2CEB0 && cp <= 0x2EBEF) ||     // CJK Unified Ideographs Extension F
        (cp >= 0x30000 && cp <= 0x3134F) ||     // CJK Unified Ideographs Extension G
        (cp >= 0xE0100 && cp <= 0xE01EF)) {     // Variation Selectors Supplement
            return 2;
        }
        return 1;
    }
#endif

    size_t calcStrDisplayWidth(const std::string& data) {
#ifdef TINY_CPP_MY_OS_UNIX
        wchar_t w_ch[8];
        std::mbtowc(w_ch, data.c_str(), data.size());
        size_t width = wcwidth(*w_ch);
        if (width == 0) width = 1;
#elif defined(TINY_CPP_MY_OS_WINDOWS)
        size_t width = wcwidth(data);
#endif
        return width;
    }
}

namespace Misc {
#undef min
#undef max
    template <typename T>
    static T min(T a, T b) {
        return a < b ? a : b;
    }

    template <typename T>
    static T max(T a, T b) {
        return a > b ? a : b;
    }
}

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

    TUI::Char::Char(const char *data) : _data(splitFront(data)), _length(calcStrDisplayWidth(_data)) {}

    TUI::Char::Char(const std::string &data)
        : _data(splitFront(data.c_str())), _length(calcStrDisplayWidth(_data)) {}

    TUI::Char & TUI::Char::operator=(const std::string &ch) {
        _data = ch;
        _length = calcStrDisplayWidth(_data);
        return *this;
    }

    TUI::Char & TUI::Char::operator=(const char *ch) {
        _data = ch;
        _length = calcStrDisplayWidth(_data);
        return *this;
    }

    TUI::Char & TUI::Char::operator=(const Char &ch) {
        _data = ch._data;
        _length = ch._length;
        return *this;
    }

    std::vector<std::string> TUI::splitUTF8(const char *data, size_t *display_size) {
        std::vector<std::string> result;
        if (display_size) *display_size = 0;
        if (data == nullptr) return result; 
        while (*data) {
            std::string str;
            uint8_t ch = *data;
            if ((ch & 0xE0) == 0xc0) {
                str += *data++;
                str += *data++;
            } else if ((ch & 0xF0) == 0xE0) {
                str += *data++;
                str += *data++;
                str += *data++;
            } else if ((ch & 0xF8) == 0xF0) {
                str += *data++;
                str += *data++;
                str += *data++;
                str += *data++;
            } else {
                str += *data++;
            }
            result.push_back(str);
            if (display_size) *display_size += calcStrDisplayWidth(str);
        }
        return result;
    }

    TUI::Renderer & TUI::Renderer::self() {
        static Renderer instance;
        return instance;
    }

    TUI::Renderer::~Renderer() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        _is_running.store(false);
        if (_resize_win_signal.joinable()) _resize_win_signal.join();
#endif
        Terminal::setMouseEnabled(false);
        Terminal::setCursorVisible(true);
        Terminal::reset();
        Terminal::leaveRawMode();
    }

    void TUI::Renderer::set(const Position &pos, uint8_t ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        char c[2] = {static_cast<char>(ch), 0};
        if (pos.row >= _front_buffer.size() || pos.column >= _front_buffer[0].size()) return;
        _front_buffer[pos.row][pos.column].set(c, style);
    }

    void TUI::Renderer::set(uint32_t x, uint32_t y, uint8_t ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (y >= _front_buffer.size() || x >= _front_buffer[0].size()) return;
        char c[2] = {static_cast<char>(ch), 0};
        _front_buffer[y][x].set(c, style);
    }

    void TUI::Renderer::set(const Position &pos, const std::string &str, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (pos.row >= _front_buffer.size() || pos.column >= _front_buffer[0].size()) return;
        _front_buffer[pos.row][pos.column].set(splitFront(str.c_str()).c_str(), style);
    }

    void TUI::Renderer::set(uint32_t x, uint32_t y, const std::string &str, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (y >= _front_buffer.size() || x >= _front_buffer[0].size()) return;
        _front_buffer[y][x].set(splitFront(str.c_str()).c_str(), style);
    }

    void TUI::Renderer::setStyle(const Position &pos, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (pos.row >= _front_buffer.size() || pos.column >= _front_buffer[0].size()) return;
        _front_buffer[pos.row][pos.column].style = style;
    }

    void TUI::Renderer::setStyle(uint32_t x, uint32_t y, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (y >= _front_buffer.size() || x >= _front_buffer[0].size()) return;
        _front_buffer[y][x].style = style;
    }

    void TUI::Renderer::fillScreen(const Style &style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        for (auto& buf : _front_buffer) {
            for (auto& col : buf) {
                col.set(" ", style);
            }
        }
    }

    void TUI::Renderer::fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        char temp[2] = {static_cast<char>(ch), 0};
        if (start_row > end_row) {
            auto t = start_row;
            start_row = end_row;
            end_row = t;
        }
        for (size_t r = start_row; r <= end_row; r++) {
            if (r >= _front_buffer.size()) continue;
            for (auto& col : _front_buffer[r]) {
                col.set(temp, style);
            }
        }
    }

    void TUI::Renderer::fillRows(uint32_t start_row, uint32_t end_row, const std::string &ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (start_row > end_row) {
            auto t = start_row;
            start_row = end_row;
            end_row = t;
        }
        for (size_t r = start_row; r <= end_row; r++) {
            if (r >= _front_buffer.size()) continue;
            for (auto& col : _front_buffer[r]) {
                col.set(ch.data(), style);
            }
        }
    }

    void TUI::Renderer::fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        char temp[2] = {static_cast<char>(ch), 0};
        if (start_col > end_col) {
            auto t = start_col;
            start_col = end_col;
            end_col = t;
        }
        for (auto& buf : _front_buffer) {
            for (size_t c = start_col; c <= end_col; c++) {
                if (c >= buf.size()) continue;
                buf[c].set(temp, style);
            }
        }
    }

    void TUI::Renderer::fillCols(uint32_t start_col, uint32_t end_col, const std::string &ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (start_col > end_col) {
            auto t = start_col;
            start_col = end_col;
            end_col = t;
        }
        for (auto& buf : _front_buffer) {
            for (size_t c = start_col; c <= end_col; c++) {
                if (c >= buf.size()) continue;
                buf[c].set(ch.data(), style);
            }
        }
    }

    void TUI::Renderer::fillRect(const Position &start_pos, const Position &end_pos, uint8_t ch, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        char temp[2] = {static_cast<char>(ch), 0};
        for (uint32_t r = start_pos.row; r <= end_pos.row; r++) {
            for (uint32_t c = start_pos.column; c <= end_pos.column; c++) {
                if (isOutOfRange(r, c)) break;
                _front_buffer[r][c].set(temp, style);
            }
        }
    }

    void TUI::Renderer::fillRect(const Position &start_pos, const Position &end_pos, const std::string &ch,
        Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        for (uint32_t r = start_pos.row; r <= end_pos.row; r++) {
            for (uint32_t c = start_pos.column; c <= end_pos.column; c++) {
                if (isOutOfRange(r, c)) break;
                _front_buffer[r][c].set(splitFront(ch.c_str()).c_str(), style);
            }
        }
    }

    void TUI::Renderer::drawBorder(const Position &start_pos, const Position &end_pos, Corner corner, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        Position r_ed = end_pos;
        if (isOutOfRange(start_pos.row, start_pos.column)) return;
        if (isOutOfRange(end_pos.row, end_pos.column)) {
            r_ed.row = static_cast<uint32_t>(_front_buffer.size()) - 1;
            r_ed.column = static_cast<uint32_t>(_front_buffer[0].size()) - 1;
        }
        // left top
        _front_buffer[start_pos.row][start_pos.column].set(corner.left_top.data().c_str(), style);
        // left bottom
        _front_buffer[r_ed.row][start_pos.column].set(corner.left_bottom.data().c_str(), style);
        // right top
        _front_buffer[start_pos.row][r_ed.column].set(corner.right_top.data().c_str(), style);
        // right bottom
        _front_buffer[r_ed.row][r_ed.column].set(corner.right_bottom.data().c_str(), style);
        // top / bottom
        for (uint32_t c = start_pos.column + 1; c < r_ed.column; c++) {
            _front_buffer[start_pos.row][c].set(corner.top.data().c_str(), style);
            _front_buffer[r_ed.row][c].set(corner.bottom.data().c_str(), style);
        }
        // left / right
        for (uint32_t r = start_pos.row + 1; r < r_ed.row; r++) {
            _front_buffer[r][start_pos.column].set(corner.left.data().c_str(), style);
            _front_buffer[r][r_ed.column].set(corner.right.data().c_str(), style);
        }
    }

    void TUI::Renderer::unset(const Position &pos) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (!isOutOfRange(pos.row, pos.column))
            _front_buffer[pos.row][pos.column].reset();
    }

    void TUI::Renderer::unset(uint32_t x, uint32_t y) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (!isOutOfRange(y, x)) _front_buffer[y][x].reset();
    }

    void TUI::Renderer::unsetRow(uint32_t row) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (row >= _front_buffer.size()) return;
        for (size_t c = 0; c < _front_buffer[row].size(); c++) {
            _front_buffer[row][c].reset();
        }
    }

    void TUI::Renderer::unsetCol(uint32_t col) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (_front_buffer.empty() || col >= _front_buffer[0].size()) return;
        for (auto& buf : _front_buffer) {
            buf[col].reset();
        }
    }

    void TUI::Renderer::unsetRect(const Position &start_pos, const Position &end_pos) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        for (uint32_t r = start_pos.row; r <= end_pos.row; r++) {
            for (uint32_t c = start_pos.column; c <= end_pos.column; c++) {
                if (isOutOfRange(r, c)) continue;
                _front_buffer[r][c].reset();
            }
        }
    }

    void TUI::Renderer::setResizeEvent(const std::function<void(Renderer&)>& event) {
        _resize_event = event;
    }

    void TUI::Renderer::clear() {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        for (auto& front : _front_buffer) {
            for (auto& i : front) {
                i.reset();
            }
        }
    }

    void TUI::Renderer::present() {
        if (std::this_thread::get_id() != _th_id) {
            Terminal::setMouseEnabled(false);
            Terminal::setCursorVisible(true);
            Terminal::leaveRawMode();
            throw std::runtime_error("Tiny::TUI::Renderer::present(): The specified renderer can not used by another thread!");
        }
        renderEvent();
    }

    const TUI::Char& TUI::Renderer::charAt(const Position &position) {
        if (!isOutOfRange(position.row, position.column))
            return _front_buffer[position.row][position.column].data;
        Terminal::leaveRawMode();
        Terminal::setMouseEnabled(false);
        throw std::runtime_error("Tiny::TUI::Renderer::charAt(): The specified position is out of range!");
    }

    const TUI::Renderer::Style& TUI::Renderer::styleAt(const Position &position) {
        if (!isOutOfRange(position.row, position.column))
            return _front_buffer[position.row][position.column].style;
        Terminal::leaveRawMode();
        Terminal::setMouseEnabled(false);
        throw std::runtime_error("Tiny::TUI::Renderer::styleAt(): The specified position is out of range!");
    }

    TUI::Renderer::Renderer() {
#ifdef TINY_CPP_MY_OS_UNIX
        setlocale(LC_ALL, "");
#else
        signal(SIGINT, SIG_IGN);
#endif
        Terminal::enterRawMode();
        Terminal::setMouseEnabled(true);
        Terminal::setCursorVisible(false);
        auto size = Terminal::screenSize();
        _term_size = size;
        _front_buffer.resize(size.height);
        _back_buffer.resize(size.height);
        for (size_t i = 0; i < size.height; ++i) {
            _front_buffer[i].resize(size.width);
            _back_buffer[i].resize(size.width);
        }
        initSignal();
        _th_id = std::this_thread::get_id();
    }

    void TUI::Renderer::renderEvent() {
        Terminal::reset();
        if (_is_resizing.exchange(false)) {
            std::lock_guard<std::mutex> lock(_resize_mutex);
            auto& size = _term_size;
            if (size.height != _front_buffer.size() || size.width != _front_buffer[0].size()) {
                Terminal::clearScreen();
                resizeEvent(false, size);
            }
        }
        // Swap buffers instead of copying
        {
            std::lock_guard<std::mutex> lock(_buffer_mutex);
            std::swap(_front_buffer, _back_buffer);
        }
        fillBuffers();
    }

    void TUI::Renderer::resizeEvent(bool use_default_size, const Size& size) {
        Size new_size{};
        {
            std::lock_guard<std::mutex> lock(_buffer_mutex);
            new_size = (use_default_size ? _term_size : size);

            _front_buffer.resize(new_size.height);
            _back_buffer.resize(new_size.height);

            for (size_t row = 0; row < new_size.height; row++) {
                _front_buffer[row].resize(new_size.width);
                _back_buffer[row].resize(new_size.width);

                for (size_t col = 0; col < new_size.width; col++) {
                    _back_buffer[row][col].reset();
                }
            }
        }
        if (_resize_event) _resize_event(self());
        EventBus::self().publish<Renderer>(new ResizeTermEvent(_term_size, new_size), SIZE_MAX);
    }

    size_t TUI::Renderer::setChars(const Position &pos, const std::string &str, const Style &style) {
        auto strs = splitUTF8(str.c_str());
        auto size = Terminal::screenSize();
        Position temp = pos;
        size_t filled_cnt = 0;
        for (auto &s : strs) {
            if (temp.column + 1 >= size.width) {
                temp.column = 0;
                temp.row += 1;
            }
            set(temp, s, style);
            size_t display_width = calcStrDisplayWidth(s);
            if (display_width > 1) {
                std::lock_guard<std::mutex> lock(_buffer_mutex);
                for (size_t i = 1; i < display_width; ++i) {
                    if (temp.column + i < _front_buffer[temp.row].size()) {
                        _front_buffer[temp.row][temp.column + i].is_dirty = true;
                    }
                }
            }
            temp.column += display_width;
            filled_cnt += display_width;
        }
        return filled_cnt;
    }

    void TUI::Renderer::setStyle(const Style &style) {
        if (style.used_rgb_color) {
            Terminal::setBackgroundColor(style.bg_rgb_color.r,
                                         style.bg_rgb_color.g,
                                         style.bg_rgb_color.b);
            Terminal::setForegroundColor(style.fg_rgb_color.r,
                                         style.fg_rgb_color.g,
                                         style.fg_rgb_color.b);
        } else {
            Terminal::setBackgroundColor(style.bg_color, style.intensity & 1);
            Terminal::setForegroundColor(style.fg_color, style.intensity & 2);
        }
        Terminal::setBolder(style.property & Style::Bolder);
        Terminal::setDark(style.property & Style::Dark);
        Terminal::setItalic(style.property & Style::Italic);
        Terminal::setUnderline(style.property & Style::Underline);
        Terminal::setBlinking(style.property & Style::Blinking);
        Terminal::setReverseColor(style.property & Style::Reverse);
        Terminal::setStrikethrough(style.property & Style::Strikethrough);
    }

    void TUI::Renderer::fillBuffers() {
        Style old_style{};
        for (size_t r = 0; r < _term_size.height; r++) {
            for (size_t c = 0; c < _term_size.width; c++) {
                if (r >= _back_buffer.size() || c >= _back_buffer[0].size()) continue;

                if (!_back_buffer[r][c].is_dirty) {
                    Terminal::moveCursor(r, c);
                    if (_back_buffer[r][c].style != old_style) {
                        old_style = _back_buffer[r][c].style;
                        if (_back_buffer[r][c].style.isDefault()) {
                            Terminal::reset();
                        } else {
                            setStyle(_back_buffer[r][c].style);
                        }
                    }
                    Terminal::print(_back_buffer[r][c].data.data());
                }

                _back_buffer[r][c].is_dirty = true;
            }
        }
    }

    bool TUI::Renderer::isOutOfRange(uint32_t row, uint32_t col) {
        return _front_buffer.empty() || row >= _front_buffer.size() || col >= _front_buffer[0].size();
    }

    void TUI::Renderer::initSignal() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        _is_running.store(true);
        _resize_win_signal = std::thread(&Renderer::resizeWindow, 0);
        // _resize_win_signal.detach();
#else
        signal(SIGWINCH, Renderer::resizeWindow);

#endif
    }

    void TUI::Renderer::resizeWindow(int) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        while (self()._is_running.load()) {
            Sleep(100);
            Size new_size = Terminal::screenSize();
            if (new_size != self()._term_size) {
                std::unique_lock<std::mutex> lock(self()._resize_mutex);
                self()._term_size = new_size;
                self()._is_resizing.store(true);
            }
        }
#else
        std::unique_lock<std::mutex> lock(self()._resize_mutex);
        usleep(100000);
        Size new_size = Terminal::screenSize();
        if (new_size != self()._term_size) {
            self()._is_resizing.store(true);
            self()._term_size = new_size;
        }
#endif
    }

    void TUI::Renderer::formatStyles(const Position& pos, const std::string &fmt, const StyleList &styles) {
        uint32_t n = 0, i = 0, st = 0;
        Style current_style{};
        for (; i < fmt.size(); ) {
            if (fmt[i] == '<') {
                int id = 0;
                bool flag = false;
                size_t j = 1;
                for (; ; ++j) {
                    size_t k = i + j;
                    if (k >= fmt.size()) break;
                    if (fmt[k] == '>') {
                        flag = true;
                        break;
                    }
                    if (fmt[k] < '0' || fmt[k] > '9') break;
                    id = id * 10 + fmt[k] - '0';
                }
                if (flag) {
                    auto t = setChars({pos.row, pos.column + n}, fmt.substr(st, i - st), current_style);
                    n += t;
                    i += j + 1;
                    st = i;
                    if (id - 1 > 0 && id - 1 < styles.size()) current_style = styles[id - 1];
                    continue;
                }
            }
            i += 1;
        }
        if (fmt.back() != '>') {
            setChars({pos.row, pos.column + n}, fmt.substr(st, i - st), current_style);

        }
    }

    TUI::EventBus& TUI::EventBus::self() {
        static EventBus _instance;
        return _instance;
    }

    TUI::EventBus::~EventBus() {
        if (_temp_mem.empty()) return;
        for (auto ptr : _temp_mem) {
            delete ptr;
            ptr = nullptr;
        }
    }

    void TUI::EventBus::pollEvents() {
        if (std::this_thread::get_id() != _th_id) {
            Terminal::setMouseEnabled(false);
            Terminal::setCursorVisible(true);
            Terminal::leaveRawMode();
            throw std::runtime_error("Tiny::TUI::EventBus::pollEvents(): Eventbus can not used by another thread!");
        }
        for (auto& runner : _running_deque) {
            if (_event_map[runner.type_index].find(runner.id) != _event_map[runner.type_index].end()) {
                if (_event_map[runner.type_index][runner.id]) _event_map[runner.type_index][runner.id](*runner.sender);
            }
        }
        for (auto ptr : _temp_mem) {
            delete ptr;
            ptr = nullptr;
        }
        _running_deque.clear();
        _temp_mem.clear();
    }

    void TUI::EventBus::clear() {
        _event_map.clear();
    }

    TUI::Application::Application(int argc, char *argv[]) : _argc(argc), _argv(argv) {
        // _event_bus.subscribe<Application>();
        Renderer::self();
    }

    int TUI::Application::run() {
        while (_running.load()) {
            auto input = Terminal::getInput();
            if (input.type != InputEvent::None)
                EventBus::self().publish<Application>(new UserInputEvent(input), SIZE_MAX);
            if (input.type == InputEvent::Keyboard && input.input.keyboard.key == KEY_CTRL_C) {
                _running.store(false);
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            EventBus::self().pollEvents();
            Renderer::self().present();
        }
        return 0;
    }

    void TUI::Application::exit() {
        _running.store(false);
    }

    TUI::AbstractWidget::AbstractWidget(const std::string &name, const Position &position, const Size &size)
            : _name(name), _pos(position), _size(size),
              _min_size(0, 0), _max_size(INT_MAX, INT_MAX) {}

    void TUI::AbstractWidget::rename(const std::string &name) {
        _name = name;
    }

    void TUI::AbstractWidget::move(const Position &position) {
        _pos = position;
        moveEvent(_pos.column, _pos.row);
    }

    void TUI::AbstractWidget::move(uint32_t x, uint32_t y) {
        _pos.column = x;
        _pos.row = y;
        moveEvent(_pos.column, _pos.row);
    }

    void TUI::AbstractWidget::resize(const Size &size) {
        Size new_size{};
        new_size.height = Misc::max(size.height, _min_size.height);
        new_size.width = Misc::max(size.width, _min_size.width);
        if (new_size == size) {
            new_size.height = Misc::min(size.height, _max_size.height);
            new_size.width = Misc::min(size.width, _max_size.width);
            _size = (new_size == size) ? size : new_size;
        } else {
            _size = new_size;
        }
        resizeEvent(_size.width, _size.height);
    }

    void TUI::AbstractWidget::resize(uint32_t w, uint32_t h) {
        Size new_size{}, size{w, h};
        new_size.height = Misc::max(h, _min_size.height);
        new_size.width = Misc::max(w, _min_size.width);
        if (new_size == size) {
            new_size.height = Misc::min(h, _max_size.height);
            new_size.width = Misc::min(w, _max_size.width);
            _size = (new_size == size) ? size : new_size;
        } else {
            _size = new_size;
        }
        resizeEvent(_size.width, _size.height);
    }

    void TUI::AbstractWidget::setMinimumSize(const Size &size) {
        _min_size = size;
    }

    void TUI::AbstractWidget::setMinimumSize(uint32_t w, uint32_t h) {
        _min_size.width = w;
        _min_size.height = h;
    }

    void TUI::AbstractWidget::setMaximumSize(const Size &size) {
        _max_size = size;
    }

    void TUI::AbstractWidget::setMaximumSize(uint32_t w, uint32_t h) {
        _max_size.width = w;
        _max_size.height = h;
    }

    void TUI::AbstractWidget::setEnabled(bool enabled) {
        _status_flag.set(2, enabled);
    }

    void TUI::AbstractWidget::setVisible(bool visible) {
        _status_flag.set(1, visible);
    }

    void TUI::AbstractWidget::setFocus(bool focus) {
        _status_flag.set(0, focus);
    }

    void TUI::AbstractWidget::setSizePolicy(SizePolicy policy) {
        _status_flag.set(3, false);
        _status_flag.set(4, false);
        _status_flag.set(5, false);
        switch (policy) {
            case SizePolicy::Ignored:
                break;
            case SizePolicy::Fixed:
                _status_flag.set(3, true);
                break;
            case SizePolicy::Maximized:
                _status_flag.set(4, true);
                break;
            case SizePolicy::Minimized:
                _status_flag.set(5, true);
                break;
        }
    }

    void TUI::AbstractWidget::draw() {
        callDrawEvent();
    }

    const std::string & TUI::AbstractWidget::name() const {
        return _name;
    }

    const TUI::Position & TUI::AbstractWidget::position() const {
        return _pos;
    }

    const TUI::Size & TUI::AbstractWidget::size() const {
        return _size;
    }

    const TUI::Size & TUI::AbstractWidget::minimumSize() const {
        return _min_size;
    }

    const TUI::Size & TUI::AbstractWidget::maximumSize() const {
        return _max_size;
    }

    bool TUI::AbstractWidget::enabled() const {
        return _status_flag.test(2);
    }

    bool TUI::AbstractWidget::visible() const {
        return _status_flag.test(1);
    }

    bool TUI::AbstractWidget::focus() const {
        return _status_flag.test(0);
    }

    TUI::SizePolicy TUI::AbstractWidget::sizePolicy() const {
        if (_status_flag.test(3)) return SizePolicy::Fixed;
        if (_status_flag.test(4)) return SizePolicy::Maximized;
        if (_status_flag.test(5)) return SizePolicy::Minimized;
        return SizePolicy::Ignored;
    }

    void TUI::AbstractWidget::execEvent(const AbstractEvent& event) {
        auto hash = event.hash();
        if (hash == typeid(UserInputEvent).hash_code()) {
            auto ev = dynamic_cast<const UserInputEvent&>(event).inputEvent();
            if (ev.type == InputEvent::Keyboard) keyEvent(ev.input.keyboard);
            else if (ev.type == InputEvent::Mouse) mouseEvent(ev.input.mouse);
        }
    }

    void TUI::AbstractWidget::callDrawEvent() {
        renderEvent(Renderer::self());
    }

    TUI::AbstractLayout::AbstractLayout(const std::string &name) : _name(name) {
        _status_flag.set(0, true);
    }

    void TUI::AbstractLayout::rename(const std::string &name) {
        _name = name;
    }

    void TUI::AbstractLayout::move(const Position &position) {
        _pos = position;
        moveEvent(_pos.column, _pos.row);
    }

    void TUI::AbstractLayout::move(uint32_t x, uint32_t y) {
        _pos.row = y;
        _pos.column = x;
        moveEvent(x, y);
    }

    void TUI::AbstractLayout::resize(const Size &size) {
        _size = size;
        resizeEvent(_size.width, _size.height);
    }

    void TUI::AbstractLayout::resize(uint32_t w, uint32_t h) {
        _size.width = w;
        _size.height = h;
        resizeEvent(w, h);
    }

    void TUI::AbstractLayout::setEnabled(bool enabled) {
        _status_flag.set(0, enabled);
    }

    void TUI::AbstractLayout::setVisible(bool visible) {
        _status_flag.set(1, visible);
    }

    bool TUI::AbstractLayout::appendWidget(AbstractWidget *widget) {
        if (std::find(_widgets_list.begin(), _widgets_list.end(), widget) == _widgets_list.end()) {
            _widgets_list.push_back(widget);
            calcSize();
            return true;
        }
        return false;
    }

    bool TUI::AbstractLayout::insertWidget(uint64_t index, AbstractWidget *widget) {
        if (std::find(_widgets_list.begin(), _widgets_list.end(), widget) == _widgets_list.end()) {
            if (index >= _widgets_list.size()) return false;
            _widgets_list.insert(_widgets_list.begin() + index, widget);
            calcSize();
            return true;
        }
        return false;
    }

    bool TUI::AbstractLayout::removeWidget(AbstractWidget *widget) {
        auto iter = std::find(_widgets_list.begin(), _widgets_list.end(), widget);
        if (iter == _widgets_list.end()) return false;
        _widgets_list.erase(iter);
        calcSize();
        return true;
    }

    bool TUI::AbstractLayout::removeWidget(uint64_t index) {
        if (index >= _widgets_list.size()) return false;
        auto iter = _widgets_list[index];
        _widgets_list.erase(_widgets_list.begin() + index);
        calcSize();
        return true;
    }

    bool TUI::AbstractLayout::swapWidget(uint64_t index_1, uint64_t index_2) {
        if (index_1 >= _widgets_list.size() || index_2 >= _widgets_list.size()) return false;
        std::swap(_widgets_list[index_1], _widgets_list[index_2]);
        return true;
    }

    bool TUI::AbstractLayout::swapWidget(AbstractWidget *widget_1, AbstractWidget *widget_2) {
        auto iter1 = std::find(_widgets_list.begin(), _widgets_list.end(), widget_1);
        auto iter2 = std::find(_widgets_list.begin(), _widgets_list.end(), widget_2);
        if (iter1 == _widgets_list.end() || iter2 == _widgets_list.end()) return false;
        std::swap(iter1, iter2);
        return true;
    }

    void TUI::AbstractLayout::clear() {
        _widgets_list.clear();
    }

    const std::string & TUI::AbstractLayout::name() const {
        return _name;
    }

    const TUI::Position & TUI::AbstractLayout::position() const {
        return _pos;
    }

    const TUI::Size & TUI::AbstractLayout::size() const {
        return _size;
    }

    bool TUI::AbstractLayout::enabled() const {
        return _status_flag.test(0);
    }

    bool TUI::AbstractLayout::visible() const {
        return _status_flag.test(1);
    }

    TUI::AbstractLayout::WidgetIter TUI::AbstractLayout::begin() {
        return _widgets_list.begin();
    }

    TUI::AbstractLayout::WidgetIter TUI::AbstractLayout::end() {
        return _widgets_list.end();
    }

    TUI::AbstractLayout::CWidgetIter TUI::AbstractLayout::cbegin() const {
        return _widgets_list.cbegin();
    }

    TUI::AbstractLayout::CWidgetIter TUI::AbstractLayout::cend() const {
        return _widgets_list.cend();
    }

    size_t TUI::AbstractLayout::count() const {
        return _widgets_list.size();
    }

    TUI::AbstractWidget* TUI::AbstractLayout::widget(size_t index) const {
        if (index >= _widgets_list.size()) return nullptr;
        return _widgets_list[index];
    }

    uint64_t TUI::AbstractLayout::indexOf(const AbstractWidget *widget) const {
        for (uint64_t i = 0; i < _widgets_list.size(); i++) {
            if (_widgets_list[i] == widget) return i;
        }
        return SIZE_MAX;
    }

    void TUI::AbstractLayout::calcSize() {

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
