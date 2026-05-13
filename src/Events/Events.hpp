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
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstdint>
#include <csignal>

namespace Tiny {
    class EventQueue;
    class Event {
        friend class EventQueue;
    public:
        Event(uint32_t id, const std::string& name,
            const std::function<bool()>& condition,
            const std::function<void(const std::atomic<bool>&)>& event);
        Event(const Event&);
        virtual ~Event();
        Event& operator=(const Event&);
        void setID(uint32_t id);
        void setName(const std::string& name);
        void setCondition(const std::function<bool()>& condition);
        void setEvent(const std::function<void(const std::atomic<bool>&)>& callback);
        [[nodiscard]] uint32_t eventID() const;
        [[nodiscard]] const std::string& eventName() const;
        [[nodiscard]] bool isRunning() const;
        [[nodiscard]] bool hasEvent() const;
        void run();
        void stop();
    private:
        uint32_t _id{};
        std::string _name{};
        std::function<void(const std::atomic<bool>&)> _event{};
        std::function<bool()> _condition{};
        std::atomic<bool> _is_running{false};
        std::thread _thread{};
    };

    class EventQueue {
        friend class Event;
    public:
        using constEventIter = std::unordered_map<uint32_t, Event>::const_iterator;
        using eventIter = std::unordered_map<uint32_t, Event>::iterator;
        static EventQueue& global();
        virtual ~EventQueue() = default;

        bool pushEvent(const Event& event);
        bool removeEvent(uint32_t event_id);
        eventIter begin();
        [[nodiscard]] constEventIter cbegin() const;
        eventIter end();
        [[nodiscard]] constEventIter cend() const;
        [[nodiscard]] const Event &at(uint32_t event_id) const;
        [[nodiscard]] size_t size() const;
        void exec();
        void stop();
        [[nodiscard]] bool isRunning() const;

        EventQueue(const EventQueue&) = delete;
        EventQueue(EventQueue&&) = delete;
        EventQueue& operator=(const EventQueue&) = delete;
        EventQueue& operator=(EventQueue&&) = delete;
    protected:
        explicit EventQueue() = default;
        void run();
    private:
        std::unordered_map<uint32_t, Event> _events_bus;
        std::atomic<bool> _is_running{false};
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