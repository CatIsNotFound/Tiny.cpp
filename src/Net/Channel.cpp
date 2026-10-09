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

#include "Channel.hpp"

namespace Tiny {
    Net::Channel::Channel(EventLoop &event_loop, Socket& socket)
            : _event_loop(&event_loop), _handle(socket.nativeHandle()) {
        bool ok{};
        auto val = socket.option(SocketOption::NonBlocking, &ok);
        if (!ok || !val.var.i) {
            socket.setOption(SocketOption::NonBlocking, true);
        }
        event_loop.addChannel(*this);
    }

    Net::Channel::~Channel() {
        if (_event_loop) {
            _event_loop->removeChannel(*this);
        }
    }

    void Net::Channel::setReadEvent(std::function<void()> &&event) {
        _r_ev = std::move(event);
        if (!_r_ev) {
            _active_status &= ~E_Read;
        }
    }

    void Net::Channel::setWriteEvent(std::function<void()> &&event) {
        _w_ev = std::move(event);
        if (!_w_ev) {
            _active_status &= ~E_Write;
        }
    }

    void Net::Channel::setErrorEvent(std::function<void()> &&event) {
        _e_ev = std::move(event);
        if (_e_ev) {
            _active_status |= E_Error;
        } else {
            _active_status &= ~E_Error;
        }
    }

    void Net::Channel::setClosedEvent(std::function<void()> &&event) {
        _c_ev = std::move(event);
        if (_c_ev) {
            _active_status |= E_Closed;
        } else {
            _active_status &= ~E_Closed;
        }
    }

    void Net::Channel::setEventByIDs(EventStatus ids, std::function<void()> &&event) {
        if (!ids) return;
        if (ids & E_Read) {
            setReadEvent(std::move(event));
        }
        if (ids & E_Write) {
            setWriteEvent(std::move(event));
        }
        if (ids & E_Closed) {
            setClosedEvent(std::move(event));
        }
        if (ids & E_Error) {
            setErrorEvent(std::move(event));
        }
    }

    void Net::Channel::setReadEnabled(bool enable) {
        if (enable) {
            _active_status |= E_Read;
        } else {
            _active_status &= ~E_Read;
        }
        update();
    }

    void Net::Channel::setWriteEnabled(bool enable) {
        if (enable) {
            _active_status |= E_Write;
        } else {
            _active_status &= ~E_Write;
        }
        update();
    }

    void Net::Channel::disableAll() {
        _active_status &= ~E_Read;
        _active_status &= ~E_Write;
        update();
    }

    bool Net::Channel::hasReadEvent() const {
        return _r_ev != nullptr;
    }

    bool Net::Channel::hasWriteEvent() const {
        return _w_ev != nullptr;
    }

    bool Net::Channel::hasErrorEvent() const {
        return _e_ev != nullptr;
    }

    bool Net::Channel::hasClosedEvent() const {
        return _c_ev != nullptr;
    }

    Net::Channel::EventStatus Net::Channel::activeStatus() const {
        return static_cast<EventStatus>(_active_status.load());
    }

    Net::Channel::EventStatus Net::Channel::readyStatus() const {
        return static_cast<EventStatus>(_ready_status.load());
    }

    void Net::Channel::update() {
        /// TODO:

    }

    void Net::Channel::handleEvents() {
        if (_r_ev && _active_status & E_Read && _ready_status & E_Read) {
            _r_ev();
            _ready_status &= ~E_Read;
        }
        if (_w_ev && _active_status & E_Write && _ready_status & E_Write) {
            _w_ev();
            _ready_status &= ~E_Write;
        }
        if (_c_ev && _ready_status & E_Closed) {
            _c_ev();
            _ready_status &= ~E_Closed;
        }
        if (_e_ev && _ready_status & E_Error) {
            _e_ev();
            _ready_status &= ~E_Error;
        }
    }

    void Net::Channel::setReadyEventID(EventStatus id) {
        _ready_status |= id;
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