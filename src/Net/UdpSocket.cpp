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

#include "UdpSocket.hpp"
#include <algorithm>

namespace Tiny {
    Net::UdpClient::UdpClient() : _udp_socket(SocketType::UDP) {
        initConfig();
    }

    Net::UdpClient::UdpClient(const char *remote_address, uint16_t remote_port, bool use_ipv6)
                : _udp_socket(SocketType::UDP) {
        _udp_socket.setPeerAddress(remote_address, remote_port, use_ipv6);
        initConfig();
    }

    Net::UdpClient::UdpClient(Address &&address) : _udp_socket(SocketType::UDP) {
        _udp_socket.setPeerAddress(std::move(address));
        initConfig();
    }

    Net::UdpClient::~UdpClient() {
        _udp_socket.close();
    }

    bool Net::UdpClient::setRemoteAddress(const char *address, uint16_t port, bool use_ipv6) {
        return _udp_socket.setPeerAddress(address, port, use_ipv6);
    }

    bool Net::UdpClient::setRemoteAddress(Address &&address) {
        return _udp_socket.setPeerAddress(std::move(address));
    }

    bool Net::UdpClient::setRemotePort(uint16_t remote_port) {
        return _udp_socket.setPeerPort(remote_port);
    }

    bool Net::UdpClient::setOption(SocketOption option, OptionValue value) {
        bool ok{};
        _udp_socket.setOption(option, value, &ok);
        return ok;
    }

    bool Net::UdpClient::bind() {
        return _udp_socket.bind("0.0.0.0", 0);
    }

    bool Net::UdpClient::close() {
        return _udp_socket.close();
    }

    bool Net::UdpClient::send(const std::string &message) {
        return _udp_socket.sendTo(message, _udp_socket.peerAddress());
    }

    bool Net::UdpClient::receive(std::string &message, size_t max_size, int *recv_length) {
        bool is_specified = _udp_socket.localAddress().isSpecified();
        return _udp_socket.recv(message, max_size, recv_length, nullptr, is_specified);
    }

    const Net::Address & Net::UdpClient::hostAddress() const {
        return _udp_socket.localAddress();
    }

    const Net::Address & Net::UdpClient::remoteAddress() const {
        return _udp_socket.peerAddress();
    }

    bool Net::UdpClient::option(SocketOption option, OptionValue &value) const {
        bool ok{};
        auto opt_val = _udp_socket.option(option, &ok);
        if (ok) value = opt_val;
        return ok;
    }

    Net::SocketState Net::UdpClient::state() const {
        return _udp_socket.state();
    }

    Net::SocketError Net::UdpClient::lastError() const {
        return _udp_socket.lastError();
    }

    void Net::UdpClient::initConfig() {
        _udp_socket.setOption(Net::SocketOption::AllowedBroadcast, true);
#ifdef TINY_CPP_MY_OS_WINDOWS
        _udp_socket.setOption(Net::SocketOption::RecvBufTimeout, 10000);
        _udp_socket.setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
        struct timeval tv{10, 0};
        _udp_socket.setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
        _udp_socket.setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
#endif
    }

    Net::UdpServer::UdpServer(uint16_t port, bool use_ipv6) : _udp_socket(SocketType::UDP) {
        if (use_ipv6) {
            _udp_socket.setLocalAddress("::", port, use_ipv6);
        } else {
            _udp_socket.setLocalAddress("0.0.0.0", port, use_ipv6);
        }
        initConfig();
    }

    Net::UdpServer::~UdpServer() {
        _udp_socket.close();
    }

    bool Net::UdpServer::setPort(uint16_t port) {
        return _udp_socket.setLocalPort(port);
    }

    bool Net::UdpServer::setNonBlocking(bool enable) {
        bool ok{};
        _udp_socket.setOption(SocketOption::NonBlocking, enable, &ok);
        return ok;
    }

    bool Net::UdpServer::listen(uint16_t port, int max_connections) {
        return _udp_socket.listen(port, max_connections);
    }

    bool Net::UdpServer::listen(int max_connections) {
        return _udp_socket.listen(max_connections);
    }

    bool Net::UdpServer::close() {
        return _udp_socket.close();
    }

    bool Net::UdpServer::send(const NetDatas &data, const Address *dest_host, int *send_failed_count) {
        bool is_all_successful{true};
        if (!dest_host) {
            for (auto& addr : _clients_list) {
                bool ok = _udp_socket.sendTo(data, addr);
                if (!ok) {
                    if (send_failed_count) *send_failed_count += 1;
                    is_all_successful = false;
                }
            }
            return is_all_successful;
        }
        auto iter = std::find_if(_clients_list.begin(), _clients_list.end(), [&dest_host](const Address& s) {
            return s == *dest_host;
        });
        if (iter != _clients_list.end()) {
            return _udp_socket.sendTo(data, *iter);
        }
        return false;
    }

    bool Net::UdpServer::receive(NetDatas &data, size_t max_size, int *recv_length, Address *dest_host,
                                 Address *src_host) {
        if (!dest_host) {
            Address recv_addr;
            bool ok = _udp_socket.recv(data, max_size, recv_length, &recv_addr);
            if (!ok) return false;
            ok = false;
            for (auto & address: _clients_list) {
                if (address == recv_addr) {
                    ok = true;
                    if (src_host) src_host->setAddress(address);
                    break;
                }
            }
            return ok;
        }
        auto iter = std::find_if(_clients_list.begin(), _clients_list.end(), [&dest_host](const Address& s) {
            return s == *dest_host;
        });
        if (iter != _clients_list.end()) {
            bool ok = _udp_socket.recv(data, max_size, recv_length, &*iter);
            if (ok && src_host) {
                src_host->setAddress(*iter);
            }
            return ok;
        }
        return false;
    }

    const Net::Address & Net::UdpServer::serverAddress() const {
        return _udp_socket.localAddress();
    }

    uint16_t Net::UdpServer::listeningPort() const {
        return _udp_socket.localAddress().port();
    }

    bool Net::UdpServer::nonBlocking() const {
        bool ok{};
        auto opt_val = _udp_socket.option(Net::SocketOption::NonBlocking, &ok);
        return ok ? opt_val.var.i : false;
    }

    size_t Net::UdpServer::connectedCount() const {
        return _clients_list.size();
    }

    bool Net::UdpServer::connectedAddress(size_t index, Address *dest_address) const {
        if (index >= _clients_list.size()) return false;
        auto& addr = _clients_list[index];
        if (dest_address) {
            dest_address->setAddress(addr.toString().data(), addr.port(), addr.isIPv6());
        }
        return true;
    }

    void Net::UdpServer::initConfig() {
        _udp_socket.setOption(Net::SocketOption::AllowedBroadcast, true);
#ifdef TINY_CPP_MY_OS_WINDOWS
        _udp_socket.setOption(Net::SocketOption::RecvBufTimeout, 10000);
        _udp_socket.setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
        struct timeval tv{10, 0};
        _udp_socket.setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
        _udp_socket.setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
#endif
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