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
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <deque>
#include <unordered_set>
#include <algorithm>
#include <set>

namespace Tiny {
    namespace TUI {
        std::string splitFront(const char* data);
        std::vector<std::string> splitUTF8(const char* data, size_t *display_size = nullptr);

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
            Char(const char* data);
            Char(const std::string& data);
            Char& operator=(const std::string& ch);
            Char& operator=(const char* ch);
            Char& operator=(const Char& ch);
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

                Style() : property(), bg_color(Color::Default), fg_color(Color::Default), intensity(2) {}

                void reset() {
                    property = 0;
                    intensity = 2;
                    bg_color = Color::Default;
                    fg_color = Color::Default;
                }

                bool isDefault() const {
                    return property == 0 && intensity == 2 &&
                           bg_color == Color::Default && fg_color == Color::Default &&
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
            
            using StyleList = std::vector<Style>;

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
            void setStyle(const Position& pos, Style style);
            void setStyle(uint32_t x, uint32_t y, Style style);
            template<typename ... Args>
            void setStrF(const Position& pos, const char* format, Args... args);
            template<typename ... Args>
            void setSSF(const Position& pos, const char* format, const Style& style, Args... args);
            template<typename ... Args>
            void setSSFX(const Position& pos, const char* format, const StyleList& styles, Args... args);
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
            void setResizeEvent(const std::function<void(Renderer&)>& event);

            void clear();
            void present();

            const Char& charAt(const Position& position);
            const Style& styleAt(const Position& position);
        protected:
            Renderer();
            virtual void renderEvent();
            virtual void resizeEvent(bool use_default_size = true, const Size& size = {});
        private:
            size_t setChars(const Position &pos, const std::string &str, const Style &style = {});
            void setStyle(const Style& style);
            void fillBuffers();
            bool isOutOfRange(uint32_t row, uint32_t col);
            void initSignal();
            static void resizeWindow(int);
            void formatStyles(const Position& pos, const std::string& fmt, const StyleList& styles);

            Size _term_size{};
            std::vector<std::vector<Cell>> _back_buffer;
            std::vector<std::vector<Cell>> _front_buffer;
            std::atomic<bool> _is_resizing{};
            std::mutex _buffer_mutex{};
            std::mutex _resize_mutex{};
            std::function<void(Renderer&)> _resize_event{};
            std::thread::id _th_id{};
#ifdef TINY_CPP_MY_OS_WINDOWS
            std::thread _resize_win_signal{};
            std::atomic<bool> _is_running{};
#endif
        };

        template<typename ... Args>
        void Renderer::setStrF(const Position& pos, const char* format, Args... args) {
            if (isOutOfRange(pos.row, pos.column)) return;
            auto f_text = Terminal::formatString(format, args...);
            setChars(pos, f_text);
        }

        template<typename ... Args>
        void Renderer::setSSF(const Position& pos, const char* format, const Style& style, Args... args) {
            if (isOutOfRange(pos.row, pos.column)) return;
            std::string f_text = Terminal::formatString(format, args...);
            setChars(pos, f_text, style);
        }

        template<typename ... Args>
        void Renderer::setSSFX(const Position &pos, const char *format, const StyleList &styles,
                                Args... args) {
            if (isOutOfRange(pos.row, pos.column)) return;
            std::string f_text = Terminal::formatString(format, args...);
            formatStyles(pos, f_text, styles);
        }

        class AbstractEvent {
        public:
            AbstractEvent(std::type_index type)
                : _type_hash(type.hash_code()) {}
            virtual ~AbstractEvent() = default;
            size_t hash() const { return _type_hash; }
        private:
            size_t _type_hash;
        };

        class UserInputEvent : public AbstractEvent {
        public:
            UserInputEvent(InputEvent input_event)
                : AbstractEvent(typeid(UserInputEvent)), _input_event(input_event) {}
            virtual ~UserInputEvent() = default;
            const InputEvent& inputEvent() const { return _input_event; }
        private:
            InputEvent _input_event{};
        };

        class RefreshRenderEvent : public AbstractEvent {
        public:
            RefreshRenderEvent() : AbstractEvent(typeid(RefreshRenderEvent)) {}
            virtual ~RefreshRenderEvent() = default;
        };

        class ResizeTermEvent : public AbstractEvent {
        public:
            ResizeTermEvent(const Size& old_size, const Size& new_size)
                : AbstractEvent(typeid(ResizeTermEvent)), _old_size(old_size), _new_size(new_size) {}
            virtual ~ResizeTermEvent() = default;
            const Size& oldSize() const { return _old_size; }
            const Size& newSize() const { return _new_size; }
        private:
            Size _old_size{};
            Size _new_size{};
        };

        using Subscriber = std::function<void(const AbstractEvent&)>;
        using SubscriberMap = std::unordered_map<size_t, Subscriber>;
        using SubscriberID = size_t;

        class EventBus {
            struct Runner {
                size_t         type_index;
                SubscriberID   id;
                AbstractEvent* sender;
                size_t         priority;
            };
            friend class Application;
        public:
            static EventBus& self();
            virtual ~EventBus();

            template <typename T>
            SubscriberID subscribe(const Subscriber& subscriber);
            template <typename T>
            SubscriberID subscribe(Subscriber&& subscriber);

            template <typename T>
            void unsubscribe(SubscriberID id);

            template <typename T>
            void publish(SubscriberID id, AbstractEvent *event, size_t priority = 0);
            template <typename T>
            void publish(AbstractEvent *event, size_t priority = 0);

            void pollEvents();
            void clear();
        protected:
            EventBus() : _th_id(std::this_thread::get_id()) {}
        private:
            std::unordered_map<size_t, SubscriberMap> _event_map;
            std::deque<Runner> _running_deque;
            std::unordered_set<AbstractEvent*> _temp_mem;
            size_t _next_id{};
            std::thread::id _th_id{};
        };

        template<typename T>
        SubscriberID EventBus::subscribe(const Subscriber &subscriber) {
            _event_map[typeid(T).hash_code()].emplace(++_next_id, subscriber);
            return _next_id;
        }

        template<typename T>
        SubscriberID EventBus::subscribe(Subscriber &&subscriber) {
            _event_map[typeid(T).hash_code()].emplace(++_next_id, std::move(subscriber));
            return _next_id;
        }

        template<typename T>
        void EventBus::unsubscribe(SubscriberID id) {
            auto hash = typeid(T).hash_code();
            _event_map[hash];
            if (_event_map[hash].find(id) != _event_map[hash].end()) {
                _event_map[hash].erase(id);
            }
        }

        template<typename T>
        void EventBus::publish(SubscriberID id, AbstractEvent *event, size_t priority) {
            auto hash = typeid(T).hash_code();
            _event_map[hash];
            if (_event_map[hash].find(id) != _event_map[hash].end()) {
                _temp_mem.emplace(event);
                _running_deque.push_back({hash, id, event, priority});
                std::sort(_running_deque.begin(), _running_deque.end(), [](const Runner& a, const Runner& b) {
                    return a.priority > b.priority;
                });
            }
        }

        template<typename T>
        void EventBus::publish(AbstractEvent *event, size_t priority) {
            auto hash = typeid(T).hash_code();
            _event_map[hash];
            _temp_mem.emplace(event);
            for (auto& ev : _event_map[hash]) {
                _running_deque.push_back({hash, ev.first, event, priority});
                std::sort(_running_deque.begin(), _running_deque.end(), [](const Runner& a, const Runner& b) {
                    return a.priority > b.priority;
                });
            }
        }

        class Application {
        public:
            Application(int argc, char* argv[]);
            int run();
            void exit();
            virtual ~Application() = default;
        private:
            char** _argv;
            int _argc;
            std::atomic<bool> _running{true};
        };

        enum class Alignment : uint8_t {
            LeftTop,
            CenterTop,
            RightTop,
            Left,
            Center,
            Right,
            LeftBottom,
            CenterBottom,
            RightBottom
        };

        enum class TextAlignment : uint8_t {
            Left,
            Center,
            Right
        };

        enum class SizePolicy : uint8_t {
            Ignored,
            Fixed,
            Maximized,
            Minimized
        };

        class AbstractWidget {
        public:
            explicit AbstractWidget(const std::string& name, const Position& position, const Size& size);
            virtual ~AbstractWidget() = 0;

            void rename(const std::string& name);
            void move(const Position& position);
            void move(uint32_t x, uint32_t y);
            void resize(const Size& size);
            void resize(uint32_t w, uint32_t h);
            void setMinimumSize(const Size& size);
            void setMinimumSize(uint32_t w, uint32_t h);
            void setMaximumSize(const Size& size);
            void setMaximumSize(uint32_t w, uint32_t h);
            void setEnabled(bool enabled);
            void setVisible(bool visible);
            void setFocus(bool focus);
            void setSizePolicy(SizePolicy policy);
            void draw();

            [[nodiscard]] const std::string& name() const;
            [[nodiscard]] const Position& position() const;
            [[nodiscard]] const Size& size() const;
            [[nodiscard]] const Size& minimumSize() const;
            [[nodiscard]] const Size& maximumSize() const;
            [[nodiscard]] bool enabled() const;
            [[nodiscard]] bool visible() const;
            [[nodiscard]] bool focus() const;
            [[nodiscard]] SizePolicy sizePolicy() const;
        protected:
            virtual void renderEvent(Renderer& renderer) = 0;
            virtual void resizeEvent(uint32_t width, uint32_t height) = 0;
            virtual void moveEvent(uint32_t x, uint32_t y) = 0;
            virtual void keyEvent(KeyEvent keyboard) = 0;
            virtual void mouseEvent(MouseEvent mouse) = 0;
            virtual void focusEvent(bool focus) = 0;
            virtual void enableEvent(bool enable) = 0;
            virtual void clickedEvent() = 0;
            virtual void execEvent(const AbstractEvent& event);
                    void callDrawEvent();

        private:
            std::string _name;
            Position _pos;
            Size _size, _min_size, _max_size;
            std::bitset<32> _status_flag{};
        };


        class AbstractLayout {
        public:
            using WidgetIter = std::vector<AbstractWidget*>::iterator;
            using CWidgetIter = std::vector<AbstractWidget*>::const_iterator;

            AbstractLayout(const std::string& name);
            virtual ~AbstractLayout() = default;

            void rename(const std::string& name);
            void move(const Position& position);
            void move(uint32_t x, uint32_t y);
            void resize(const Size& size);
            void resize(uint32_t w, uint32_t h);
            void setEnabled(bool enabled);
            void setVisible(bool visible);
            bool appendWidget(AbstractWidget* widget);
            bool insertWidget(uint64_t index, AbstractWidget* widget);
            bool removeWidget(AbstractWidget* widget);
            bool removeWidget(uint64_t index);
            bool swapWidget(uint64_t index_1, uint64_t index_2);
            bool swapWidget(AbstractWidget* widget_1, AbstractWidget* widget_2);
            void clear();

            [[nodiscard]] const std::string& name() const;
            [[nodiscard]] const Position& position() const;
            [[nodiscard]] const Size& size() const;
            [[nodiscard]] bool enabled() const;
            [[nodiscard]] bool visible() const;

            [[nodiscard]] WidgetIter begin();
            [[nodiscard]] WidgetIter end();
            [[nodiscard]] CWidgetIter cbegin() const;
            [[nodiscard]] CWidgetIter cend() const;
            [[nodiscard]] size_t count() const;
            [[nodiscard]] AbstractWidget* widget(size_t index) const;
            [[nodiscard]] uint64_t indexOf(const AbstractWidget* widget) const;
        protected:
            virtual void renderEvent(Renderer& renderer) = 0;
            virtual void moveEvent(uint32_t x, uint32_t y) = 0;
            virtual void resizeEvent(uint32_t width, uint32_t height) = 0;

        private:
            void calcSize();

            std::vector<AbstractWidget*> _widgets_list;
            std::string _name;
            Position _pos{};
            Size _size{};
            std::bitset<8> _status_flag{};
        };

        class Label : public AbstractWidget {
        public:
            explicit Label(const std::string& name, const Position& position);
            virtual ~Label() = default;

            void setText(const std::string& text);

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