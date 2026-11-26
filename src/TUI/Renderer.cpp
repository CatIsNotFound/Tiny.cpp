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

#include "Renderer.hpp"
#include <algorithm>
#include <csignal>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <cstring>
#ifdef TINY_CPP_MY_OS_WINDOWS
#include <process.h>
#else
#include <unistd.h>
#endif

namespace Tiny {
    TUI::Utf8Char::Utf8Char(const char *string, uint8_t size) : _data(), _ch_size(size), _dp_size() {
        set(string, size);
    }

    TUI::Utf8Char::Utf8Char(char c) : _data(), _ch_size(1), _dp_size(1) {
        _data[0] = c;
    }

    void TUI::Utf8Char::set(char C) {
        memset(_data, 0, 16);
        _data[0] = C >= ' ' && C <= '~' ? C : ' ';
        _ch_size = 1;
        _dp_size = 1;
    }

    void TUI::Utf8Char::set(const char *string, uint8_t size) {
        char temp[16]{};
        if (size > 0) {
            strncpy(temp, string, std::min(static_cast<uint32_t>(size), 16u));
        } else {
            strncpy(temp, string, 16);
        }
        auto data = U8Code::splitFront(temp);
        memset(_data, 0, 16);
        strncpy(_data, temp, data.size());
        _ch_size = data.size();
        _dp_size = U8Code::calcStrDisplayWidth(_data);
    }

    void TUI::Utf8Char::clear() {
        memset(_data, 0, 16);
        _ch_size = 0;
        _dp_size = 0;
    }

    const char * TUI::Utf8Char::data() const {
        return _data;
    }

    uint32_t TUI::Utf8Char::size() const {
        return _ch_size;
    }

    uint32_t TUI::Utf8Char::length() const {
        return _ch_size;
    }

    uint32_t TUI::Utf8Char::displayLength() const {
        return _dp_size;
    }

    std::thread::id TUI::TermBuffer::_th_id{};
    TUI::TermBuffer &TUI::TermBuffer::self() {
        static TermBuffer self;
        return self;
    }

    void TUI::TermBuffer::set(const Position &pos, uint8_t ch, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(pos, &cell)) {
            cell.set(ch);
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::set(uint32_t row, uint32_t col, uint8_t ch, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(row, col, &cell)) {
            cell.set(ch);
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::set(const Position &pos, const char *str, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(pos, &cell)) {
            cell.set(str);
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::set(uint32_t row, uint32_t col, const char *str, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(row, col, &cell)) {
            cell.set(str);
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::setStyle(const Position &pos, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(pos, &cell)) {
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::setStyle(uint32_t row, uint32_t col, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Cell cell;
        if (cellOf(row, col, &cell)) {
            cell.setStyle(style);
        }
    }

    void TUI::TermBuffer::fillScreen(const TermStyle &style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        for (auto & v: _buffers) {
            for (auto & cell: v) {
                cell.set(' ');
                cell.setStyle(style);
                cell.is_changed = true;
            }
        }
    }

    void TUI::TermBuffer::fillRows(uint32_t start_row, uint32_t end_row, uint8_t ch, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        if (start_row >= end_row) {
            std::swap(start_row, end_row);
        }
        if (start_row >= _buffers.size()) return;
        for (uint32_t i = start_row; i < end_row; ++i) {
            if (i >= _buffers.size()) break;
            for (auto& cell: _buffers[i]) {
                cell.set(ch);
                cell.setStyle(style);
            }
        }
    }

    void TUI::TermBuffer::fillRows(uint32_t start_row, uint32_t end_row, const char *str, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        if (start_row >= end_row) {
            std::swap(start_row, end_row);
        }
        if (start_row >= _buffers.size()) return;
        for (uint32_t i = start_row; i < end_row; ++i) {
            if (i >= _buffers.size()) break;
            for (auto& cell: _buffers[i]) {
                cell.set(str);
                cell.setStyle(style);
            }
        }
    }

    void TUI::TermBuffer::fillCols(uint32_t start_col, uint32_t end_col, uint8_t ch, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        if (start_col >= end_col) {
            std::swap(start_col, end_col);
        }
        if (start_col >= _buffers.size()) return;
        for (auto & buffer: _buffers) {
            for (uint32_t i = start_col; i < end_col; ++i) {
                if (i >= _term_size.width) break;
                buffer[i].set(ch);
                buffer[i].setStyle(style);
            }
        }
    }

    void TUI::TermBuffer::fillCols(uint32_t start_col, uint32_t end_col, const char *str, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        if (start_col >= end_col) {
            std::swap(start_col, end_col);
        }
        if (start_col >= _buffers.size()) return;
        for (auto & buffer: _buffers) {
            for (uint32_t i = start_col; i < end_col; ++i) {
                if (i >= _term_size.width) break;
                buffer[i].set(str);
                buffer[i].setStyle(style);
            }
        }
    }

    void TUI::TermBuffer::fillRect(const Position &start_pos, const Position &end_pos, uint8_t ch, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        for (size_t i = start_pos.row; i < end_pos.row; ++i) {
            for (uint32_t j = start_pos.column; j < end_pos.column; ++j) {
                if (cellOf(i, j)) {
                    _buffers[i][j].set(ch);
                    _buffers[i][j].setStyle(style);
                }
            }
        }
    }

    void TUI::TermBuffer::fillRect(const Position &start_pos, const Position &end_pos, const char *str,
                TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        for (size_t i = start_pos.row; i < end_pos.row; ++i) {
            if (i >= _term_size.height) break;
            for (uint32_t j = start_pos.column; i < end_pos.column; ++j) {
                if (j >= _term_size.width) break;
                _buffers[i][j].set(str);
                _buffers[i][j].setStyle(style);
            }
        }
    }

    void TUI::TermBuffer::drawBorder(const Position &start_pos, const Position &end_pos,
                Border border, TermStyle style) {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock, [this] {
                return !_is_rendering.load();
            });
        }
        Position r_ed = end_pos;
        if (!cellOf(start_pos.row, start_pos.column)) return;
        if (!cellOf(end_pos.row, end_pos.column)) {
            r_ed.row = static_cast<uint32_t>(_buffers.size()) - 1;
            r_ed.column = static_cast<uint32_t>(_buffers[0].size()) - 1;
        }
        // left top
        _buffers[start_pos.row][start_pos.column].set(border.left_top.data());
        _buffers[start_pos.row][start_pos.column].setStyle(style);
        // left bottom
        _buffers[r_ed.row][start_pos.column].set(border.left_bottom.data());
        _buffers[r_ed.row][start_pos.column].setStyle(style);
        // right top
        _buffers[start_pos.row][r_ed.column].set(border.right_top.data());
        _buffers[start_pos.row][r_ed.column].setStyle(style);
        // right bottom
        _buffers[r_ed.row][r_ed.column].set(border.right_bottom.data());
        _buffers[r_ed.row][r_ed.column].setStyle(style);
        // top / bottom
        for (uint32_t c = start_pos.column + 1; c < r_ed.column; c++) {
            _buffers[start_pos.row][c].set(border.top.data());
            _buffers[start_pos.row][c].setStyle(style);
            _buffers[r_ed.row][c].set(border.bottom.data());
            _buffers[r_ed.row][c].setStyle(style);
        }
        // left / right
        for (uint32_t r = start_pos.row + 1; r < r_ed.row; r++) {
            _buffers[r][start_pos.column].set(border.left.data());
            _buffers[r][start_pos.column].setStyle(style);
            _buffers[r][r_ed.column].set(border.right.data());
            _buffers[r][r_ed.column].setStyle(style);
        }
    }

    void TUI::TermBuffer::setResizeEvent(const std::function<void(TermBuffer &)> &event) {
        _resize_event = event;
    }

    void TUI::TermBuffer::clear() {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        _buffers_cond_var.wait(lock, [this] {
            return !_is_rendering;
        });
        for (auto& buf : _buffers) {
            for (auto & cell: buf) {
                cell.style.reset();
                cell.utf8_char.set(' ');
                cell.is_changed = true;
                cell.offset = 0;
            }
        }
    }

    void TUI::TermBuffer::present() {
        if (std::this_thread::get_id() != _th_id) {
            Terminal::setMouseEnabled(false);
            Terminal::setCursorVisible(true);
            Terminal::leaveRawMode();
            throw std::runtime_error("Tiny::TUI::TermBuffer::present(): "
                                     "The specified renderer can not used by another thread!");
        }
        renderEvent();
    }

    const TUI::Utf8Char & TUI::TermBuffer::charAt(const Position &position) const {
        bool ok = cellOf(position, nullptr);
        if (ok) {
            return _buffers[position.row][position.column].utf8_char;
        }
        throw std::out_of_range("Tiny::TUI::TermBuffer: The specified position is out of range!");
    }

    const TUI::Utf8Char & TUI::TermBuffer::charAt(uint32_t row, uint32_t col) const {
        bool ok = cellOf(row, col, nullptr);
        if (ok) {
            return _buffers[row][col].utf8_char;
        }
        throw std::out_of_range("Tiny::TUI::TermBuffer: The specified position is out of range!");
    }

    const TUI::TermStyle & TUI::TermBuffer::styleAt(const Position &position) const {
        bool ok = cellOf(position, nullptr);
        if (ok) {
            return _buffers[position.row][position.column].style;
        }
        throw std::out_of_range("Tiny::TUI::TermBuffer: The specified position is out of range!");
    }

    const TUI::TermStyle & TUI::TermBuffer::styleAt(uint32_t row, uint32_t col) const {
        bool ok = cellOf(row, col, nullptr);
        if (ok) {
            return _buffers[row][col].style;
        }
        throw std::out_of_range("Tiny::TUI::TermBuffer: The specified position is out of range!");
    }

    bool TUI::TermBuffer::isValid(const Position &position) const {
        return cellOf(position, nullptr);
    }

    bool TUI::TermBuffer::isValid(uint32_t row, uint32_t col) const {
        return cellOf(row, col, nullptr);
    }

    const TUI::Size & TUI::TermBuffer::size() const {
        return _term_size;
    }

    TUI::TermBuffer::~TermBuffer() {
        cleanUpSignals(0);
        if (_resize_win_signal.joinable()) _resize_win_signal.join();
    }

    TUI::TermBuffer::TermBuffer() {
        Terminal::enterRawMode();
        Terminal::setCursorVisible(false);
        Terminal::setMouseEnabled(true);
        initSignals();
        _th_id = std::this_thread::get_id();
        _term_size = Terminal::screenSize();
        _buffers.resize(_term_size.height);
        for (auto & buf: _buffers) {
            buf.resize(_term_size.width);
        }
        _resize_win_signal = std::thread(&TermBuffer::resizeTerm);
        _is_running.store(true);
    }

    void TUI::TermBuffer::renderEvent() {
        _is_rendering.store(true);
        {
            std::unique_lock<std::mutex> lock(_buffers_mutex);
            size_t row{};
            TermStyle cur_style{};
            for (auto& buf: _buffers) {
                size_t col{};
                if (row >= _term_size.height) break;
                for (auto& cell: buf) {
                    if (cell.is_changed) {
                        cell.is_changed = false;
                        if (cur_style != cell.style) {
                            parseStyle(cell.style);
                            cur_style = cell.style;
                        }
                        Terminal::moveCursor(row, col);
                        Terminal::print(cell.utf8_char.data(), cell.utf8_char.size());
                    }
                    // col += cell.offset;
                    col += cell.offset;
                }
                row += 1;
            }
            parseStyle({});
        }
        _is_rendering.store(false);
        _buffers_cond_var.notify_all();
    }

    void TUI::TermBuffer::resizeEvent() {
        std::unique_lock<std::mutex> lock(_buffers_mutex);
        if (_is_rendering.load()) {
            _buffers_cond_var.wait(lock);
        }
        if (_buffers.size() < _term_size.height) _buffers.resize(_term_size.height);
        for (auto & buf: _buffers) {
            if (buf.size() < _term_size.width) buf.resize(_term_size.width);
        }
    }

    void TUI::TermBuffer::initSignals() {
#ifdef TINY_CPP_MY_OS_UNIX
        setlocale(LC_ALL, "");
#else
        signal(SIGINT, SIG_IGN);
#endif
        signal(SIGSEGV, cleanUpSignals);
        signal(SIGTERM, cleanUpSignals);
    }

    void TUI::TermBuffer::cleanUpSignals(int signal) {
        self()._is_running.store(false);
        Terminal::setMouseEnabled(false);
        Terminal::setCursorVisible(true);
        Terminal::reset();
        Terminal::leaveRawMode();
        if (signal > 0) {
            Terminal::printError("Terminated by signal {}!\r\n", signal);
        }
    }

    void TUI::TermBuffer::resizeTerm() {
        while (self()._is_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto scr = Terminal::screenSize();
            if (scr != self()._term_size) {
                self()._term_size = scr;
                self().resizeEvent();
                if (self()._resize_event) self()._resize_event(self());
            }
        }
    }

    bool TUI::TermBuffer::setChars(const Position &pos, const std::string &str, const TermStyle &style, size_t *count) {
        if (!isValid(pos)) return false;
        auto u8_chars = U8Code::splitUTF8(str.data());
        size_t off{};
        bool ok{true};
        for (auto & buf: u8_chars) {
            _buffers[pos.row][pos.column + off].set(buf.data(), buf.size());
            _buffers[pos.row][pos.column + off].setStyle(style);
            off += 1;
            if (!isValid(pos.row, pos.column + off)) {
                ok = false;
                break;
            }
        }
        if (count) *count = off;
        return ok;
    }

    std::vector<TUI::TermBuffer::FmtStyle> TUI::TermBuffer::parseString(const std::string &str) {
        size_t parsed_num{}, last_index{};
        bool parsing{}, sof{}, stop{};
        std::vector<FmtStyle> out(1);
        for (auto& c: str) {
            if (c == '<') {
                if (!parsing) {
                    parsing = true;
                    sof = true;
                } else {
                    if (sof) {
                        sof = false;
                        parsing = false;
                        out.back().text.push_back('<');
                    }
                }
            } else if (parsing) {
                if (c >= '0' && c <= '9' && !stop) {
                    parsed_num = parsed_num * 10 + c - '0';
                } else if (c == '/') {
                    parsed_num = SIZE_MAX;
                    stop = true;
                } else if (c == '>') {
                    parsing = false;
                    out.back().index = last_index;
                    last_index = parsed_num;
                    parsed_num = 0;
                    out.emplace_back();
                } else {
                    out.back().text.push_back('<');
                    out.back().text.push_back(c);
                    parsing = false;
                }
            } else {
                out.back().text.push_back(c);
            }
        }
        if (out.back().index == 0 && last_index > 0) {
            out.back().index = last_index;
        }
        return out;
    }

    uint32_t TUI::TermBuffer::setFmtStyle(const Position &pos, const std::vector<FmtStyle> &style,
                                          const TermStyleList &style_list) {
        uint32_t col{};
        size_t count{};
        bool stop_now{};
        for (auto& fs : style) {
            if (fs.index >= style_list.size()) {
                if (!setChars({pos.row, pos.column + col}, fs.text, {}, &count)) {
                    stop_now = true;
                }
            } else if (!setChars({pos.row, pos.column + col}, fs.text, style_list[fs.index], &count)) {
                stop_now = true;
            }
            col += count;
            if (stop_now) break;
        }
        return col;
    }

    void TUI::TermBuffer::parseStyle(const TermStyle &style) {
        Terminal::reset();
        if (style.isDefault()) return;
        if (style.used_rgb) {
            Terminal::setBackgroundColor(style.colors.rgb.bg.r,
                                         style.colors.rgb.bg.g,
                                         style.colors.rgb.bg.b);
            Terminal::setForegroundColor(style.colors.rgb.fg.r,
                                         style.colors.rgb.fg.g,
                                         style.colors.rgb.fg.b);
        } else {
            Terminal::setBackgroundColor(style.colors.color.bg, style.intensity & TermStyle::Back);
            Terminal::setForegroundColor(style.colors.color.fg, style.intensity & TermStyle::Fore);
        }
        if (style.property == 0) return;
        if (style.property & TermStyle::Bolder) {
            Terminal::setBolder(true);
        }
        if (style.property & TermStyle::Dark) {
            Terminal::setDark(true);
        }
        if (style.property & TermStyle::Italic) {
            Terminal::setItalic(true);
        }
        if (style.property & TermStyle::Underline) {
            Terminal::setUnderline(true);
        }
        if (style.property & TermStyle::Blinking) {
            Terminal::setBlinking(true);
        }
        if (style.property & TermStyle::Reverse) {
            Terminal::setReverseColor(true);
        }
        if (style.property & TermStyle::Strikethrough) {
            Terminal::setStrikethrough(true);
        }
    }

    bool TUI::TermBuffer::cellOf(const Position &pos, Cell *cell) const {
        if (pos.row < _term_size.height || pos.column < _term_size.width ||
                pos.row < _buffers.size()) {
            if (cell) *cell = _buffers[pos.row][pos.column];
            return true;
        }
        return false;
    }

    bool TUI::TermBuffer::cellOf(uint32_t row, uint32_t col, Cell *cell) const {
        if (row >= _term_size.height || col >= _term_size.width ||
                row >= _buffers.size()) {
            return false;
        }
        if (cell) *cell = _buffers[row][col];
        return true;
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
