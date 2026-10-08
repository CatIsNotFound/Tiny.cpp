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

#ifndef TINY_CPP_NET_CHANNEL_HPP
#define TINY_CPP_NET_CHANNEL_HPP
#include "Socket.hpp"
#include <array>
#include <functional>

namespace Tiny {
    namespace Net {
        class EventLoop;
        class Channel {
            friend class EventLoop;
        public:
            enum EventStatus : uint8_t {
                E_None,
                E_Read   = 1,
                E_Write  = 2,
                E_Error  = 4,
                E_Closed = 8
            };
            explicit Channel(EventLoop& event_loop, Socket* socket);
            ~Channel();

            void setReadEvent(std::function<void()>&& event);
            void setWriteEvent(std::function<void()>&& event);
            void setErrorEvent(std::function<void()>&& event);
            void setClosedEvent(std::function<void()>&& event);
            void setEventByID(EventStatus id, std::function<void()>&& event);

            void setReadEnabled(bool enable);
            void setWriteEnabled(bool enable);
            void disableAll();
            void setReadyEventID(EventStatus id);

            bool hasReadEvent() const;
            bool hasWriteEvent() const;
            bool hasErrorEvent() const;
            bool hasClosedEvent() const;
            EventStatus runningStatus() const;
            EventStatus readyStatus() const;
        private:
            void update();
            void handleEvents();

            std::function<void()>    _r_ev, _w_ev, _e_ev, _c_ev;
            EventLoop*   _event_loop;
            Handle       _handle;
            EventStatus  _ready_status{}, _running_status{};
        };
    }
}

#include "EventLoop.hpp"
#endif //TINY_CPP_NET_CHANNEL_HPP

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