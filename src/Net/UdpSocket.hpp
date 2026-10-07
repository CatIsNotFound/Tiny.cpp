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

#ifndef TINY_UDPSOCKET_HPP
#define TINY_UDPSOCKET_HPP
#include "Socket.hpp"

namespace Tiny {
    namespace Net {
        class UdpClient {
        public:
            explicit UdpClient();
            explicit UdpClient(const char* remote_address, uint16_t remote_port, bool use_ipv6 = false);
            explicit UdpClient(Address&& address);
            ~UdpClient();

            bool setRemoteAddress(const char *address, uint16_t port, bool use_ipv6 = false);
            bool setRemoteAddress(Address &&address);
            bool setRemotePort(uint16_t remote_port);
            bool setOption(SocketOption option, OptionValue value);
            bool bind();
            bool close();
            bool send(const std::string& message);
            bool receive(std::string& message, size_t max_size = 0, int* recv_length = nullptr);

            const Address& hostAddress() const;
            const Address& remoteAddress() const;
            bool option(SocketOption option, OptionValue& value) const;
            SocketState state() const;
            SocketError lastError() const;
        private:
            void initConfig();
            Socket _udp_socket;
        };

        class UdpServer {
        public:
            explicit UdpServer(uint16_t port, bool use_ipv6 = false);
            ~UdpServer();

            bool setPort(uint16_t port);

            bool setNonBlocking(bool enable);
            bool listen(uint16_t port, int max_connections);
            bool listen(int max_connections);
            bool close();
            bool send(const NetDatas &data, const Address* dest_host = nullptr, int *send_failed_count = nullptr);
            bool receive(NetDatas &data, size_t max_size = 0, int* recv_length = nullptr,
                         Address *dest_host = nullptr, Address* src_host = nullptr);

            const Address& serverAddress() const;
            uint16_t listeningPort() const;
            bool nonBlocking() const;

            size_t connectedCount() const;
            bool connectedAddress(size_t index, Address* dest_address) const;
        private:
            void initConfig();
            Socket _udp_socket;
            std::vector<Address> _clients_list;
        };
    }
}



#endif //TINY_UDPSOCKET_HPP

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