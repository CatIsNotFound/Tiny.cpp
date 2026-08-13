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

#include "EventBus.hpp"

// std::once_flag Tiny::EV::EventBus::_init_flag{};
// std::unique_ptr<Tiny::EV::EventBus> Tiny::EV::EventBus::_instance{};

void Tiny::EV::EventListener::add(AbstractEventHandler *event) {
    if (_handlers.find(event->hashCode()) != _handlers.end()) {
        auto& handlers = _handlers[event->hashCode()];
        if (std::find(handlers.begin(), handlers.end(), event) == handlers.end()) {
            _handlers[event->hashCode()].emplace_back(event);
        }
    } else {
        _handlers[event->hashCode()].emplace_back(event);
    }
    _size++;
}

void Tiny::EV::EventListener::remove(const AbstractEventHandler *event) {
    if (_handlers.find(event->hashCode()) != _handlers.end()) {
        auto& ev_list = _handlers[event->hashCode()];
        size_t before = ev_list.size();
        ev_list.erase(std::remove_if(ev_list.begin(), ev_list.end(), [event](const AbstractEvent* handler) {
            return handler == event;
        }), ev_list.end());
        size_t after = ev_list.size();
        _size -= before - after;
    }
}

Tiny::EV::EventBus* Tiny::EV::EventBus::global() {
    // std::call_once(_init_flag, [] {
    //     _instance.reset(new EventBus());
    // });
    // return _instance.get();
    static EventBus _instance;
    return &_instance;
}

Tiny::EV::HandlerID Tiny::EV::EventBus::install(EventListener* listener) {
    std::lock_guard<std::mutex> lock(_mutex);
    bool ok = _event_map.emplace(++_next_id, listener).second;
    return ok ? _next_id : 0;
}

bool Tiny::EV::EventBus::uninstall(HandlerID id) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_event_map.find(id) != _event_map.end()) {
        _event_map.erase(id);
        return true;
    }
    return false;
}

void Tiny::EV::EventBus::emit(AbstractEvent *event) {
    std::vector<AbstractEventHandler*> exec_list;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto idx = event->hashCode();
        for (auto& handler : _event_map) {
            if (handler.second->contains(idx)) {
                for (auto& ev : handler.second->at(idx)) {
                    exec_list.push_back(ev);
                }
            }
        }
    }
    for (auto& ev : exec_list) {
        ev->onEvent(*event);
    }
}

void Tiny::EV::EventBus::emit(HandlerID id, AbstractEvent *event) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_event_map.find(id) != _event_map.end()) {
        auto ev_list = _event_map[id]->at(event->hashCode());
        for (auto& ev : ev_list) {
            ev->onEvent(*event);
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

