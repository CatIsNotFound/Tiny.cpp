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

namespace Tiny {
    EV::Event::Event(uint32_t id, std::string name,
                 const std::function<bool()> &condition, const std::function<void(const std::atomic<bool>&)> &event)
                     : _id(id), _name(std::move(name)), _event(event), _condition(condition) {}

    EV::Event::Event(uint32_t id, std::string name, const std::function<void(const std::atomic<bool> &)> &event)
                : _id(id), _name(std::move(name)), _event(event) {}

    EV::Event::Event(uint32_t id, std::string name)
                : _id(id), _name(std::move(name)) {}

    EV::Event::Event(const EV::Event & event)
           : _id(event._id), _name(event._name), _event(event._event), _condition(event._condition),
             _delay(event._delay.load()), _exec_count(event._exec_count.load()) {}

    EV::Event &EV::Event::operator=(const EV::Event &event) {
        if (this != &event) {
            stop();
            this->_needs_destroy.store(true);
            if (this->_thread.joinable()) this->_thread.join();
            this->_needs_destroy.store(false);
            this->_id = event._id;
            this->_name = event._name;
            this->_event = event._event;
            this->_condition = event._condition;
            this->_delay.store(event._delay.load());
            this->_now_cnt.store(0);
            this->_is_load_event.store(false);
            this->_exec_count.store(0);
            this->_is_running.store(false);
        }
        return *this;
    }

    EV::Event::~Event() {
        stop();
        _needs_destroy.store(true);
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void EV::Event::setID(uint32_t id) {
        _id = id;
    }

    void EV::Event::setName(const std::string &name) {
        _name = name;
    }

    void EV::Event::setDelayMS(uint32_t delay) {
        _delay.store(delay);
    }

    void EV::Event::setRepeatCount(uint32_t count) {
        _exec_count.store(count);
    }

    void EV::Event::setCondition(const std::function<bool()> &condition) {
        std::lock_guard<std::mutex> lock_guard{_sh_mutex};
        _condition = condition;
    }

    void EV::Event::setAllowedFailedEnabled(bool enabled) {
        _allowed_failed.store(enabled);
    }

    void EV::Event::setEvent(const std::function<void(const std::atomic<bool>&)> &callback) {
        std::lock_guard<std::mutex> lock_guard{_sh_mutex};
        _event = callback;
    }

    uint32_t EV::Event::eventID() const {
        return _id;
    }

    const std::string & EV::Event::eventName() const {
        return _name;
    }

    uint32_t EV::Event::eventDelayMS() const {
        return _delay.load();
    }

    uint32_t EV::Event::eventRepeatCount() const {
        return _exec_count.load();
    }

    uint32_t EV::Event::executionCount() const {
        return _allowed_failed.load() ? _attempt_cnt.load() : _now_cnt.load();
    }

    bool EV::Event::isRunning() const {
        return _is_running.load();
    }

    bool EV::Event::hasEvent() const {
        return _event != nullptr;
    }

    bool EV::Event::allowedFailedEnabled() const {
        return _allowed_failed.load();
    }

    void EV::Event::run() {
        if (_is_running.load() || !hasEvent()) return;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _now_cnt.store(0);
            _attempt_cnt.store(0);
            _is_running.store(true);
            _run_var.notify_all();
        }
        if (!_is_load_event.load()) {
            _thread = std::thread(&Event::loop, this);
            _is_load_event.store(true);
        }
    }

    void EV::Event::stop() {
        _con_var.notify_all();
        _run_var.notify_all();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _is_running.store(false);
        }
    }

    void EV::Event::loop() {
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
                std::function<bool()> copied_cond;
                std::function<void(const std::atomic<bool>&)> copied_event;
                {
                    std::lock_guard<std::mutex> lock_guard(_sh_mutex);
                    copied_cond = _condition;
                    copied_event = _event;
                }
                if (!copied_cond || !copied_event) {
                    _is_running.store(false);
                    break;
                }
                size_t cycle_cnt = _exec_count.load();
                if (cycle_cnt > 0) {
                    bool cond = _allowed_failed.load() ? _attempt_cnt.load() >= cycle_cnt : _now_cnt.load() >= cycle_cnt;
                    if (cond) {
                        _is_running.store(false);
                        break;
                    }
                }
                try {
                    if (copied_cond()) {
                        copied_event(_is_running);
                        _now_cnt.fetch_add(1);
                    }
                    _attempt_cnt.fetch_add(1);
                } catch (const std::exception &e) {
                    _is_running.store(false);
                }
                std::unique_lock<std::mutex> lock(_mutex);
                _con_var.wait_for(lock, std::chrono::milliseconds(_delay.load()));
            }
        }
    }

    EV::EventsMap::EventsMap() = default;

    EV::EventsMap::~EventsMap() {
        waitAllEvents();
    }

    bool EV::EventsMap::execEvent(const EV::Event &event) {
        if (exist(event.eventID())) return false;
        _event_map.emplace(event.eventID(), event);
        _event_map.at(event.eventID()).run();
        while (!_event_map.at(event.eventID()).isRunning()) {}
        return true;
    }

    bool EV::EventsMap::execEvent(size_t event_id) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).run();
        return true;
    }

    bool EV::EventsMap::addEvent(const EV::Event &event) {
        if (exist(event.eventID())) return false;
        _event_map.emplace(event.eventID(), event);
        return true;
    }

    bool EV::EventsMap::removeEvent(size_t event_id) {
        if (!exist(event_id)) return false;
        waitEvent(event_id);
        _event_map.erase(event_id);
        return true;
    }

    bool EV::EventsMap::removeAllFreeEvents() {
        std::vector<size_t> rm_event_ids;
        for (auto& ev : _event_map) {
            if (!ev.second.isRunning()) {
                rm_event_ids.push_back(ev.first);
            }
        }
        for (auto& id : rm_event_ids) {
            _event_map.erase(id);
        }
        return true;
    }

    void EV::EventsMap::stopEvent(size_t event_id) {
        if (!exist(event_id)) return;
        _event_map.at(event_id).stop();
    }

    void EV::EventsMap::waitEvent(size_t event_id) {
        if (!exist(event_id)) return;
        _event_map.at(event_id).stop();
        while (_event_map.at(event_id).isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void EV::EventsMap::stopAllEvents() {
        for (auto& ev : _event_map) {
            if (!ev.second.isRunning()) ev.second.stop();
        }
    }

    void EV::EventsMap::waitAllEvents() {
        for (auto& ev : _event_map) {
            while (ev.second.isRunning()) {
                ev.second.stop();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }

    bool EV::EventsMap::setConditionByID(size_t event_id, const std::function<bool()> &condition) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).setCondition(condition);
        return true;
    }

    bool EV::EventsMap::setEventByID(size_t event_id, const std::function<void(const std::atomic<bool> &)> &event) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).setEvent(event);
        return true;
    }

    bool EV::EventsMap::setDelayByID(size_t event_id, uint32_t delay_ms) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).setDelayMS(delay_ms);
        return true;
    }

    bool EV::EventsMap::setRepeatByID(size_t event_id, uint32_t repeat_count) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).setRepeatCount(repeat_count);
        return true;
    }

    bool EV::EventsMap::setAllowedFailedEnabledByID(size_t event_id, bool enabled) {
        if (!exist(event_id)) return false;
        _event_map.at(event_id).setAllowedFailedEnabled(enabled);
        return true;
    }

    bool EV::EventsMap::exist(size_t event_id) const {
        return _event_map.find(event_id) != _event_map.end();
    }

    const EV::Event & EV::EventsMap::event(size_t event_id) const {
        if (exist(event_id)) return _event_map.at(event_id);
        throw std::out_of_range("Tiny::EventsMap: The specified event id is not found!");
    }

    EV::EventsMap::constIter EV::EventsMap::cbegin() const {
        return _event_map.cbegin();
    }

    EV::EventsMap::constIter EV::EventsMap::cend() const {
        return _event_map.cend();
    }

    size_t EV::EventsMap::size() const {
        return _event_map.size();
    }

    std::vector<size_t> EV::EventsMap::eventIDList() const {
        std::vector<size_t> event_ids;
        event_ids.reserve(_event_map.size());
        for (auto& ev : _event_map) {
            event_ids.push_back(ev.first);
        }
        return event_ids;
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