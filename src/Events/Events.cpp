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

#include "Events.hpp"

#include <utility>

namespace Tiny {
    Event::Event(uint32_t id, std::string name,
                 const std::function<bool()> &condition, const std::function<void(const std::atomic<bool>&)> &event)
                     : _id(id), _name(std::move(name)), _event(event), _condition(condition) {}

    Event::Event(uint32_t id, std::string name)
                : _id(id), _name(std::move(name)) {}

    Event::Event(const Event & event)
           : _id(event._id), _name(event._name), _event(event._event), _condition(event._condition),
             _delay(event._delay.load()), _exec_count(event._exec_count.load()) {}

    Event &Event::operator=(const Event &event) {
        if (this != &event) {
            stop();
            this->_id = event._id;
            this->_name = event._name;
            this->_is_running = false;
            this->_event = event._event;
            this->_condition = event._condition;
            this->_delay.store(event._delay.load());
            this->_exec_count.store(event._exec_count.load());
            this->_now_cnt.store(0);
        }
        return *this;
    }

    Event::~Event() {
        _needs_destroy.store(true);
        stop();
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void Event::setID(uint32_t id) {
        _id = id;
    }

    void Event::setName(const std::string &name) {
        _name = name;
    }

    void Event::setDelayMS(uint32_t delay) {
        _delay.store(delay);
    }

    void Event::setRepeatCount(uint32_t count) {
        _exec_count.store(count);
    }

    void Event::setCondition(const std::function<bool()> &condition) {
        _condition = condition;
    }

    void Event::setEvent(const std::function<void(const std::atomic<bool>&)> &callback) {
        _event = callback;
    }

    uint32_t Event::eventID() const {
        return _id;
    }

    const std::string & Event::eventName() const {
        return _name;
    }

    uint32_t Event::eventDelayMS() const {
        return _delay.load();
    }

    uint32_t Event::eventRepeatCount() const {
        return _exec_count.load();
    }

    bool Event::isRunning() const {
        return _is_running.load();
    }

    bool Event::hasEvent() const {
        return _event != nullptr;
    }

    void Event::run() {
        if (_is_running.load() || !hasEvent()) return;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _now_cnt.store(0);
            _is_running.store(true);
            _run_var.notify_all();
        }
        if (!_is_load_event.load()) {
            _thread = std::thread(&Event::loop, this);
            _is_load_event.store(true);
        }
    }

    void Event::stop() {
        _con_var.notify_all();
        _run_var.notify_all();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _is_running.store(false);
        }
    }

    void Event::loop() {
        while (true) {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _run_var.wait(lock, [this] {
                    return _is_running.load() || _needs_destroy.load();
                });
                if (_needs_destroy.load()) {
                    _is_load_event.store(false);
                    break;
                }
            }

            while (_is_running.load()) {
                if (!_condition || !_event) {
                    _is_running.store(false);
                    break;
                }
                size_t cycle_cnt = _exec_count.load();
                if (cycle_cnt > 0) {
                    if (_now_cnt.load() >= cycle_cnt) {
                        _is_running.store(false);
                        break;
                    }
                }
                try {
                    if (_condition()) {
                        _event(_is_running);
                        _now_cnt.fetch_add(1);
                    }
                } catch (const std::exception &e) {
                    _is_running.store(false);
                    printf("Tiny::Event: An error has occurred: %s\n", e.what());
                }
                std::unique_lock<std::mutex> lock(_mutex);
                _con_var.wait_for(lock, std::chrono::milliseconds(_delay.load()));
            }
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