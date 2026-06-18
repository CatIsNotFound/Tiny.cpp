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

#ifndef TINY_CPP_EVENTS_HPP
#define TINY_CPP_EVENTS_HPP

#include <string>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cstdint>

namespace Tiny {
    class Event {
    public:
        Event(uint32_t id, std::string name,
            const std::function<bool()>& condition,
            const std::function<void(const std::atomic<bool>&)>& event);
        Event(uint32_t id, std::string name,
            const std::function<void(const std::atomic<bool>&)>& event);
        Event(uint32_t id, std::string name);
        Event(const Event&);
        virtual ~Event();
        Event& operator=(const Event&);
        void setID(uint32_t id);
        void setName(const std::string& name);
        void setDelayMS(uint32_t delay);
        void setRepeatCount(uint32_t count);
        void setCondition(const std::function<bool()>& condition);
        void setEvent(const std::function<void(const std::atomic<bool>&)>& callback);
        [[nodiscard]] uint32_t eventID() const;
        [[nodiscard]] const std::string& eventName() const;
        [[nodiscard]] uint32_t eventDelayMS() const;
        [[nodiscard]] uint32_t eventRepeatCount() const;
        [[nodiscard]] bool isRunning() const;
        [[nodiscard]] bool hasEvent() const;
        void run();
        void stop();
    private:
        void loop();
        uint32_t _id{};
        std::atomic<bool> _is_load_event{}, _needs_destroy{};
        std::atomic<bool> _is_running{};
        std::string _name{};
        std::function<void(const std::atomic<bool>&)> _event{};
        std::function<bool()> _condition{[] { return true; }};
        std::thread _thread{};
        std::atomic<uint32_t> _delay{1000};
        std::atomic<uint32_t> _exec_count{1}, _now_cnt{0};
        std::mutex _mutex{};
        std::condition_variable _con_var{}, _run_var{};
    };

    class EventsMap {
    public:
        using constIter = std::unordered_map<size_t, Event>::const_iterator;

        EventsMap();
        ~EventsMap();

        bool execEvent(const Event& event);
        bool execEvent(size_t event_id);
        bool addEvent(const Event& event);
        bool removeEvent(size_t event_id);
        bool removeAllFreeEvents();

        void stopEvent(size_t event_id);
        void waitEvent(size_t event_id);
        void stopAllEvents();
        void waitAllEvents();

        bool setConditionByID(size_t event_id, const std::function<bool()>& condition);
        bool setEventByID(size_t event_id, const std::function<void(const std::atomic<bool>&)>& event);
        bool setDelayByID(size_t event_id, uint32_t delay_ms);
        bool setRepeatByID(size_t event_id, uint32_t repeat_count);

        bool exist(size_t event_id) const;
        const Event& event(size_t event_id) const;
        bool event(const Event& find_event, const Event* found_event = nullptr) const;

        constIter cbegin() const;
        constIter cend() const;
        size_t size() const;

        std::vector<size_t> eventIDList() const;

    private:
        std::unordered_map<size_t, Event> _event_map{};
    };
}


#endif //TINY_CPP_EVENTS_HPP

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