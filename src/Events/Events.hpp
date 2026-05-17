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
#include <condition_variable>
#include <cstdint>
#include <csignal>

namespace Tiny {
    class Event {
    public:
        Event(uint32_t id, std::string name,
            const std::function<bool()>& condition,
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
        std::string _name{};
        std::function<void(const std::atomic<bool>&)> _event{};
        std::function<bool()> _condition{};
        std::atomic<bool> _is_running{false};
        std::thread _thread{};
        std::atomic<uint32_t> _delay{1000};
        std::atomic<uint32_t> _exec_count{1}, _now_cnt{0};
        std::mutex _mutex{};
        std::condition_variable _con_var{}, _run_var{};
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