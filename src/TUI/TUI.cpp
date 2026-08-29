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
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <csignal>
#include "win/wcwidth.h"
#else
#include <csignal>
#include <clocale>
#include <cwchar>
#include <climits>
#include <unistd.h>
#endif

namespace Misc {
    template <typename T>
    static T min(T a, T b) {
        return a < b ? a : b;
    }

    template <typename T>
    static T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    static T clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }
}

namespace Tiny {
    static TUI::Application* globalApp{};

    TUI::Char::Char(const char *data) : _data(Code::splitFront(data)), _length(Code::calcStrDisplayWidth(_data)) {}

    TUI::Char::Char(const std::string &data)
        : _data(Code::splitFront(data.c_str())), _length(Code::calcStrDisplayWidth(_data)) {}

    TUI::Char & TUI::Char::operator=(const std::string &ch) {
        _data = ch;
        _length = Code::calcStrDisplayWidth(_data);
        return *this;
    }

    TUI::Char & TUI::Char::operator=(const char *ch) {
        _data = ch;
        _length = Code::calcStrDisplayWidth(_data);
        return *this;
    }

    TUI::Char & TUI::Char::operator=(const Char &ch) {
        _data = ch._data;
        _length = ch._length;
        return *this;
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
        _front_buffer[pos.row][pos.column].set(Code::splitFront(str.c_str()).c_str(), style);
    }

    void TUI::Renderer::set(uint32_t x, uint32_t y, const std::string &str, Style style) {
        std::lock_guard<std::mutex> lock(_buffer_mutex);
        if (y >= _front_buffer.size() || x >= _front_buffer[0].size()) return;
        _front_buffer[y][x].set(Code::splitFront(str.c_str()).c_str(), style);
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
                _front_buffer[r][c].set(Code::splitFront(ch.c_str()).c_str(), style);
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
        auto strs = Code::splitUTF8(str.c_str());
        auto size = Terminal::screenSize();
        Position temp = pos;
        size_t filled_cnt = 0;
        for (auto &s : strs) {
            if (temp.column + 1 >= size.width) {
                temp.column = 0;
                temp.row += 1;
            }
            set(temp, s, style);
            size_t display_width = Code::calcStrDisplayWidth(s);
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

    TUI::Object::Object(const std::string &name, std::type_index type_id, Object *parent)
            : _name(name), _type_index(type_id), _parent_index(typeid(void)), _parent(parent) {
        if (parent != nullptr) {
            parent->_children.push_back(this);
            return;
        }
        _parent = nullptr;
        if (globalApp && !_parent) {
            globalApp->_objects.push_back(this);
        }
    }

    TUI::Object::Object(const std::string &name, std::type_index type_id, std::type_index parent_type_id, Object *parent)
            : _name(name), _type_index(type_id), _parent_index(parent_type_id), _parent(parent) {
        if (parent != nullptr) {
            parent->_children.push_back(this);
            return;
        }
        _parent = nullptr;
        if (globalApp && !_parent) {
            globalApp->_objects.push_back(this);
        }
    }

    void TUI::Object::renameObject(const std::string &name) {
        _name = name;
        onObjectNameChanged();
    }

    const std::string & TUI::Object::objectName() const {
        return _name;
    }

    void TUI::Object::setParent(Object *parent) {
        if (_parent) {
            auto p = std::find(_parent->_children.begin(), _parent->_children.end(), this);
            if (p != _parent->_children.end()) {
                _parent->_children.erase(p);
            }
        }
        auto ptr = std::find(globalApp->_objects.begin(), globalApp->_objects.end(), this);
        if (parent != nullptr) {
            parent->_children.push_back(this);
            if (ptr != globalApp->_objects.end()) globalApp->_objects.erase(ptr);
        } else if (ptr == globalApp->_objects.end()) {
            globalApp->_objects.push_back(this);
        }
        _parent = parent;
    }

    TUI::Object * TUI::Object::parent() const {
        return _parent;
    }

    size_t TUI::Object::hash() const {
        return _type_index.hash_code();
    }

    size_t TUI::Object::phash() const {
        return _parent_index.hash_code();
    }

    const char * TUI::Object::className() const {
        return _type_index.name();
    }

    bool TUI::Object::isChild(Object *child) const {
        return std::find(_children.begin(), _children.end(), child) != _children.end();
    }

    TUI::Object * TUI::Object::findChild(const std::string &name) const {
        auto iter = std::find_if(_children.begin(), _children.end(), [&name] (const Object* obj) {
            return obj->objectName() == name;
        });
        return (iter != _children.end()) ? *iter : nullptr;
    }

    TUI::Object * TUI::Object::findChild(std::type_index type_id, const std::string &name) const {
        auto iter = std::find_if(_children.begin(), _children.end(), [&type_id, &name] (const Object* obj) {
            return obj->hash() == type_id.hash_code() && obj->objectName() == name;
        });
        return (iter != _children.end()) ? *iter : nullptr;
    }

    const std::vector<Tiny::TUI::Object*>& TUI::Object::children() const {
        return _children;
    }

    TUI::Application::Application() {
        Renderer::self();
        if (!globalApp) globalApp = this;
        EventBus::self().subscribe<Application>([this](const AbstractEvent& event) {
            for (auto& obj : _objects) {
                obj->onEvent(event);
            }
        });
        EventBus::self().subscribe<Renderer>([this](const AbstractEvent& event) {
            if (event.hash() != typeid(ResizeTermEvent).hash_code()) return;
            if (_refresh.load()) Renderer::self().fillScreen();
            auto sz = dynamic_cast<const ResizeTermEvent&>(event).newSize();
            for (auto& obj : _objects) {
                obj->onResizedTermSize(sz);
            }
        });
    }

    int TUI::Application::run() {
        EventBus::self().publish<Application>(new AbstractEvent(typeid(Application)));
        while (_running.load()) {
            auto input = Terminal::getInput();
            if (input.type != InputEvent::None)
                EventBus::self().publish<Application>(new UserInputEvent(input), SIZE_MAX);
            if (_quit.load() && input.type == InputEvent::Keyboard && input.input.keyboard.key == KEY_CTRL_C) {
                _running.store(false);
            }

            EventBus::self().pollEvents();
            Renderer::self().present();
        }
        return 0;
    }

    void TUI::Application::exit() {
        _running.store(false);
    }

    void TUI::Application::setEnabledExitByKey(bool enabled) {
        _quit.store(enabled);
    }

    bool TUI::Application::isEnabledExitByKey() const {
        return _quit.load();
    }

    void TUI::Application::setRefreshEnabled(bool enabled) {
        _refresh.store(enabled);
    }

    bool TUI::Application::isRefreshEnabled() const {
        return _refresh.load();
    }

    TUI::AbstractWidget::AbstractWidget(const std::string &name, const Position &position, const Size &size,
                                        std::type_index type_id, Object* parent)
            : Object(name, type_id, typeid(AbstractWidget), parent), _pos(position), _size(size),
              _min_size(0, 0), _max_size(INT_MAX, INT_MAX) {
        _styles[S_Normal].intensity = 2;
        _styles[S_Active].intensity = 1;
        _styles[S_Disabled].intensity = 0;
        _status_flag.set(F_Enabled, true);
        _status_flag.set(F_Visible, true);
    }

    TUI::AbstractWidget::AbstractWidget(const std::string &name, std::type_index type_id, Object *parent)
            : Object(name, type_id, parent), _pos(0, 0), _size(0, 0),
              _min_size(0, 0), _max_size(INT_MAX, INT_MAX) {
        _styles[S_Normal].intensity = 2;
        _styles[S_Active].intensity = 1;
        _styles[S_Disabled].intensity = 0;
        _status_flag.set(F_Enabled, true);
        _status_flag.set(F_Visible, true);
        _status_flag.set(F_SizePolicy, true);
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
        _size.width = Misc::clamp(w, _min_size.width, _max_size.width);
        _size.height = Misc::clamp(h, _min_size.height, _max_size.height);
        resizeEvent(_size.width, _size.height);
    }

    void TUI::AbstractWidget::setMinimumSize(const Size &size) {
        _min_size.width = Misc::min(size.width, _max_size.width);
        _min_size.height = Misc::min(size.height, _max_size.height);
        Size new_size{};
        new_size.width = Misc::max(_size.width, _min_size.width);
        new_size.height = Misc::max(_size.height, _min_size.height);
        if (new_size != _size) {
            _size = new_size;
            resizeEvent(_size.width, _size.height);
        }
    }

    void TUI::AbstractWidget::setMinimumSize(uint32_t w, uint32_t h) {
        _min_size.width = Misc::min(w, _max_size.width);
        _min_size.height = Misc::min(h, _max_size.height);
        Size new_size{};
        new_size.width = Misc::max(_size.width, _min_size.width);
        new_size.height = Misc::max(_size.height, _min_size.height);
        if (new_size != _size) {
            _size = new_size;
            resizeEvent(_size.width, _size.height);
        }
    }

    void TUI::AbstractWidget::setMaximumSize(const Size &size) {
        _max_size.width = Misc::max(size.width, _min_size.width);
        _max_size.height = Misc::max(size.height, _min_size.height);
        Size new_size{};
        new_size.width = Misc::min(_size.width, _max_size.width);
        new_size.height = Misc::min(_size.height, _max_size.height);
        if (new_size != _size) {
            _size = new_size;
            resizeEvent(_size.width, _size.height);
        }
    }

    void TUI::AbstractWidget::setMaximumSize(uint32_t w, uint32_t h) {
        _max_size.width = Misc::max(w, _min_size.width);
        _max_size.height = Misc::max(h, _min_size.height);
        Size new_size{};
        new_size.width = Misc::min(_size.width, _max_size.width);
        new_size.height = Misc::min(_size.height, _max_size.height);
        if (new_size != _size) {
            _size = new_size;
            resizeEvent(_size.width, _size.height);
        }
    }

    void TUI::AbstractWidget::setEnabled(bool enabled) {
        _status_flag.set(F_Enabled, enabled);
        _status_flag.set(F_Style + S_Disabled, !enabled);
    }

    void TUI::AbstractWidget::setVisible(bool visible) {
        _status_flag.set(F_Visible, visible);
    }

    void TUI::AbstractWidget::setFocus(bool focus) {
        _status_flag.set(F_Focus, focus);
        _status_flag.set(F_Style + S_Active, focus);
    }

    void TUI::AbstractWidget::setSizePolicy(SizePolicy policy) {
        _status_flag.set(F_SizePolicy, false);
        _status_flag.set(F_SizePolicy + 1, false);
        _status_flag.set(F_SizePolicy + 2, false);
        switch (policy) {
            case SizePolicy::Ignored:
                break;
            case SizePolicy::Fixed:
                _status_flag.set(F_SizePolicy, true);
                break;
            case SizePolicy::Maximized:
                _status_flag.set(F_SizePolicy + 1, true);
                break;
            case SizePolicy::Minimized:
                _status_flag.set(F_SizePolicy + 2, true);
                break;
        }
    }

    void TUI::AbstractWidget::setStyle(uint8_t status, const Renderer::Style &style) {
        if (status >= _styles.size()) return;
        _styles[status] = style;
    }

    void TUI::AbstractWidget::draw() {
        callDrawEvent();
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
        return _status_flag.test(F_Enabled);
    }

    bool TUI::AbstractWidget::visible() const {
        return _status_flag.test(F_Visible);
    }

    bool TUI::AbstractWidget::focus() const {
        return _status_flag.test(F_Focus);
    }

    TUI::SizePolicy TUI::AbstractWidget::sizePolicy() const {
        if (_status_flag.test(F_SizePolicy)) return SizePolicy::Fixed;
        if (_status_flag.test(F_SizePolicy + 1)) return SizePolicy::Maximized;
        if (_status_flag.test(F_SizePolicy + 2)) return SizePolicy::Minimized;
        return SizePolicy::Ignored;
    }

    TUI::Renderer::Style TUI::AbstractWidget::style(uint8_t status) const {
        if (status >= _styles.size()) return {};
        return _styles[status];
    }

    void TUI::AbstractWidget::onEvent(const AbstractEvent &event) {
        if (visible()) renderEvent(Renderer::self());
    }

    void TUI::AbstractWidget::onResizedTermSize(const Size &size) {
        if (visible()) renderEvent(Renderer::self());
    }
    void TUI::AbstractWidget::onObjectNameChanged() {}
    void TUI::AbstractWidget::onParentChanged() {}

    void TUI::AbstractWidget::execEvent(const UserInputEvent& event) {
        auto ev = event.inputEvent();
        if (ev.type == InputEvent::Keyboard) {
            keyEvent(ev.input.keyboard);
            if (ev.input.keyboard.is_pressed) clickedEvent();
        }
        else if (ev.type == InputEvent::Mouse) {
            mouseEvent(ev.input.mouse);
            if (ev.input.mouse.button == SP_MOUSE_LEFT_BUTTON) {
                _status_flag.set(F_Style + S_Pressed, ev.input.mouse.is_pressed);
                if (ev.input.mouse.is_pressed) clickedEvent();
            }
        }
    }

    void TUI::AbstractWidget::callDrawEvent() {
        if (_status_flag.test(1)) renderEvent(Renderer::self());
    }

    void TUI::AbstractWidget::resizeWithoutCalledEvent(uint32_t width, uint32_t height) {
        _size.width = width;
        _size.height = height;
    }

    const TUI::Renderer::Style & TUI::AbstractWidget::currentStyle() const {
        for (uint8_t i = 0; i < _styles.size(); i++) {
            if (_status_flag.test(F_Style + i)) return _styles[i];
        }
        return _styles[0];
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

    TUI::TestWidget::TestWidget(const std::string &name, const Position &position, const Size &size, Object *parent)
            : AbstractWidget(name, position, size, typeid(TUI::TestWidget), parent) {}

    void TUI::TestWidget::onEvent(const AbstractEvent &event) {
        AbstractWidget::onEvent(event);
    }

    void TUI::TestWidget::onResizedTermSize(const Size &size) {
        AbstractWidget::onResizedTermSize(size);
    }

    void TUI::TestWidget::renderEvent(Renderer &renderer) {
        Position end_pos = { position().row + size().height - 1, position().column + size().width - 1 };
        renderer.fillRect(position(), end_pos, '*');
    }

    TUI::Label::Label(const std::string &name, const Position &position, Object* parent)
            : AbstractWidget(name, position, {}, typeid(TUI::Label), parent), _text(name) {
        setMinimumSize(8, 1);
        _status_flag.set(10, true);
        calcAutoSize();
    }

    TUI::Label::Label(const std::string &name, const Position &position, const Size &size, Object* parent)
            : AbstractWidget(name, position, size, typeid(TUI::Label), parent), _text(name) {
        setMinimumSize(8, 1);
        if (size.width < 8 || size.height < 1) resizeWithoutCalledEvent(8, 1);
        calcDisplaySize();
    }


    void TUI::Label::setText(const std::string &text) {
        _text = text;
        Code::splitUTF8(_text.c_str(), &_text_size);
        if (_status_flag.test(10)) {
            calcAutoSize();
        } else {
            calcDisplaySize();
        }
    }

    const std::string & TUI::Label::text() const {
        return _text;
    }

    void TUI::Label::setAutoSizeEnabled(bool enabled) {
        _status_flag.set(10, enabled);
        if (enabled) calcAutoSize(); else calcDisplaySize();
    }

    bool TUI::Label::autoSizeEnabled() const {
        return _status_flag.test(10);
    }

    void TUI::Label::setAlignment(Alignment alignment) {
        for (uint8_t i = 0; i < 9; ++i) _status_flag.reset(i);
        _status_flag.set(static_cast<uint8_t>(alignment), true);
        if (!autoSizeEnabled()) calcDisplaySize();
    }

    TUI::Alignment TUI::Label::alignment() const {
        for (uint8_t i = 0; i < 9; ++i) {
            if (_status_flag.test(i)) return static_cast<Alignment>(i);
        }
        return Alignment::LeftTop;
    }

    void TUI::Label::onEvent(const AbstractEvent &event) {
        AbstractWidget::onEvent(event);
    }

    void TUI::Label::onResizedTermSize(const Size &size) {
        if (autoSizeEnabled()) calcAutoSize(); else calcDisplaySize();
        AbstractWidget::onResizedTermSize(size);
    }

    void TUI::Label::onObjectNameChanged() {
        AbstractWidget::onObjectNameChanged();
    }

    void TUI::Label::onParentChanged() {
        AbstractWidget::onParentChanged();
    }

    void TUI::Label::renderEvent(Renderer &renderer) {
        if (autoSizeEnabled()) {
            Renderer::self().setSSF(_text_pos, _dis_text.c_str(), currentStyle());
            return;
        }
        Position end_pos = position().calcEndPos(size());
        Renderer::self().fillRect(position(), end_pos, ' ', currentStyle());
        Renderer::self().setSSF(_text_pos, _dis_text.c_str(), currentStyle());
    }

    void TUI::Label::resizeEvent(uint32_t w, uint32_t h) {
        if (_status_flag.test(10)) {
            calcAutoSize();
        } else {
            calcDisplaySize();
        }
    }

    void TUI::Label::moveEvent(uint32_t x, uint32_t y) {
        if (_status_flag.test(10)) {
            calcAutoSize();
        } else {
            calcDisplaySize();
        }
    }

    void TUI::Label::keyEvent(KeyEvent) {
    }

    void TUI::Label::mouseEvent(MouseEvent) {
    }

    void TUI::Label::focusEvent(bool) {
    }

    void TUI::Label::enableEvent(bool) {
    }

    void TUI::Label::clickedEvent() {
    }

    void TUI::Label::calcAutoSize() {
        resizeWithoutCalledEvent(_text_size, 1);
        _text_pos = position();
        const auto END_POS = position().calcEndPos(size());
        auto scr = Terminal::screenSize() - Size(1, 1);
        const auto R = END_POS.column >= scr.width;
        if (R) {
            _dis_text = Code::subUTF8(_text.c_str(), _text_size - (END_POS.column - scr.width + 1));
        } else {
            _dis_text = _text;
        }
    }

    void TUI::Label::calcDisplaySize() {
        const auto END_POS = position().calcEndPos(size());
        const auto SCR = Terminal::screenSize() - Size(1, 1);
        size_t dis_len{};
        auto dis_chars = Code::splitUTF8(_text.c_str(), &dis_len);
        int64_t con_len = static_cast<int32_t>(size().width) - static_cast<int32_t>(dis_len);

        if (END_POS.column < SCR.width) {
            if (con_len > 0) {
                _dis_text = _text;
            } else {
                _dis_text.clear();
                dis_len += con_len;
                for (size_t i = 0; i < dis_len; ++i) {
                    _dis_text += dis_chars[i];
                }
            }
        } else {
            _dis_text.clear();
            dis_len -= END_POS.column - SCR.width;
            for (size_t i = 0; i < dis_len; ++i) {
                _dis_text += dis_chars[i];
            }
        }


        switch (alignment()) {
            case Alignment::LeftTop:
                _text_pos = position();
                break;
            case Alignment::CenterTop:
                _text_pos.row = position().row;
                _text_pos.column = position().column + (size().width / 2 - dis_len / 2);
                break;
            case Alignment::RightTop:
                _text_pos.row = position().row;
                _text_pos.column = position().column + (size().width - dis_len);
                break;
            case Alignment::Left:
                _text_pos.row = position().row + (size().height / 2) - (size().height % 2 == 0);
                _text_pos.column = position().column;
                break;
            case Alignment::Center:
                _text_pos.row = position().row + (size().height / 2) - (size().height % 2 == 0);
                _text_pos.column = position().column + (size().width / 2 - dis_len / 2);
                break;
            case Alignment::Right:
                _text_pos.row = position().row + (size().height / 2) - (size().height % 2 == 0);
                _text_pos.column = position().column + (size().width - dis_len);
                break;
            case Alignment::LeftBottom:
                _text_pos.row = END_POS.row;
                _text_pos.column = position().column;
                break;
            case Alignment::CenterBottom:
                _text_pos.row = END_POS.row;
                _text_pos.column = position().column + (size().width / 2 - dis_len / 2);
                break;
            case Alignment::RightBottom:
                _text_pos.row = END_POS.row;
                _text_pos.column = position().column + (size().width - dis_len);
                break;
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
