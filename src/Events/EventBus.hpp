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

#ifndef TINY_EVENTBUS_HPP
#define TINY_EVENTBUS_HPP
#include <future>

#include "Events.hpp"
#include <memory>
#include <typeindex>
#include <algorithm>


namespace Tiny {
    namespace EV {
        class AbstractEvent {
            friend class EventBus;
        public:
            AbstractEvent(std::type_index index) : _idx(index) {};
            virtual ~AbstractEvent() = default;
            size_t hashCode() const { return _idx.hash_code(); }
        private:
            std::type_index _idx;
        };

        class AbstractEventHandler : public AbstractEvent {
            friend class EventBus;
        public:
            AbstractEventHandler(std::type_index index) : AbstractEvent(index) {}
            ~AbstractEventHandler() override = default;
        protected:
            virtual void onEvent(const AbstractEvent& event) = 0;
        };

        class EventListener {
            friend class EventBus;
        public:
            using iter = std::unordered_map<uint64_t, std::vector<AbstractEventHandler*>>::iterator;
            using constIter = std::unordered_map<uint64_t, std::vector<AbstractEventHandler*>>::const_iterator;
            EventListener() = default;
            virtual ~EventListener() = default;
            void add(AbstractEventHandler* event);
            void remove(const AbstractEventHandler* event);
            void clear() { _handlers.clear(); }
            iter begin() { return _handlers.begin(); }
            iter end() { return _handlers.end(); }
            constIter begin() const { return _handlers.begin(); }
            constIter end() const { return _handlers.end(); }
            size_t size() const { return _size; }
            bool contains(size_t type_id) const { return _handlers.find(type_id) != _handlers.end(); }
        private:
            std::vector<AbstractEventHandler*>& at(size_t type_id) { return _handlers.at(type_id); }
            AbstractEventHandler* at(size_t type_id, size_t index) { return _handlers.at(type_id).at(index); }
            std::unordered_map<uint64_t, std::vector<AbstractEventHandler*>> _handlers;
            size_t _size{};
        };

        using HandlerID = uint64_t;

        class EventBus {
        public:
            ~EventBus() = default;
            static EventBus* global();
            HandlerID install(EventListener* listener);
            bool uninstall(HandlerID id);
            void emit(AbstractEvent* event);
            void emit(HandlerID id, AbstractEvent* event);

        protected:
            EventBus() = default;
        private:
            std::unordered_map<HandlerID, EventListener*> _event_map;
            std::mutex _mutex;
            HandlerID _next_id{};
            // static std::once_flag _init_flag;
            // static std::unique_ptr<EventBus> _instance;
        };
    }
}


#endif //TINY_EVENTBUS_HPP


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