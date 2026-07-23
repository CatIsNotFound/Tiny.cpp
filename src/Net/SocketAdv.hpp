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

#ifndef TINY_NET_SOCKETADV_HPP
#define TINY_NET_SOCKETADV_HPP

/**********************************************************************************
 * Notes: This definition file should only be used for special purposes;          *
 *        do not directly include this header file under normal circumstances.    *
 **********************************************************************************/

#include "Socket.hpp"
#include <functional>
#ifdef TINY_CPP_MY_OS_WINDOWS
    #include <winsock2.h>
    #include <ws2ipdef.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define INVALID_SOCKET_VAL INVALID_SOCKET
#elif defined(TINY_CPP_MY_OS_UNIX)
    #include <cstring>
    #include <csignal>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/sctp.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #define INVALID_SOCKET_VAL (-1)
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR (-1)
    #endif
    #ifndef SOCKET
        #define SOCKET int
    #endif
#endif

namespace Tiny {
    namespace Net {
        namespace Advanced {
            using Setter = std::function<bool(Handle, uint32_t, const OptionValue &)>;
            using Getter = std::function<bool(Handle, uint32_t, OptionValue &)>;
            bool setSocketOption(SocketOption option, OptionValue::ValueType val_type,
                                 const Setter &setter,
                                 const Getter &getter);
            void setCustomSocketOption(uint32_t option_id, OptionValue::ValueType val_type,
                                       const Setter &setter,
                                       const Getter &getter);
            bool removeCustomSocketOption(uint32_t option_id);
            bool getValueFromCustomOption(uint32_t option_id, OptionValue& value, Handle socket);
            bool setValueFromCustomOption(uint32_t option_id, const OptionValue& value, Handle socket);
        }
    }
}

#endif //TINY_NET_SOCKETADV_HPP

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