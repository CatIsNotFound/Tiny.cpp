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

#include "TcpSocket.hpp"
#include <algorithm>

namespace Tiny {
    Net::TcpClient::TcpClient() : _tcp_socket() {
        initConfig();
    }

    Net::TcpClient::TcpClient(const char *remote_address, uint16_t remote_port, bool use_ipv6) {
        _tcp_socket.setPeerAddress(remote_address, remote_port, use_ipv6);
        initConfig();
    }

    Net::TcpClient::TcpClient(Address &&address) {
        _tcp_socket.setPeerAddress(std::move(address));
        initConfig();
    }

    Net::TcpClient::~TcpClient() {
        if (_tcp_socket.state() != SocketState::Unused) _tcp_socket.close();
    }

    bool Net::TcpClient::setRemoteAddress(Address &&address) {
        return _tcp_socket.setPeerAddress(std::move(address));
    }

    bool Net::TcpClient::setRemoteAddress(const char *address, uint16_t port, bool use_ipv6) {
        return _tcp_socket.setPeerAddress(address, port, use_ipv6);
    }

    bool Net::TcpClient::setRemotePort(uint16_t port) {
        return _tcp_socket.setPeerPort(port);
    }

    bool Net::TcpClient::setOption(SocketOption option, OptionValue value) {
        bool ok{};
        _tcp_socket.setOption(option, value, &ok);
        return ok;
    }

    bool Net::TcpClient::connect(uint32_t timeout_ms) {
        return _tcp_socket.connect(timeout_ms);
    }

    bool Net::TcpClient::close() {
        return _tcp_socket.close();
    }

    bool Net::TcpClient::send(const std::string &message) {
        return _tcp_socket.send(message);
    }

    bool Net::TcpClient::receive(std::string &message, size_t max_size, int *recv_length) {
        return _tcp_socket.recv(message, max_size, recv_length);
    }

    const Net::Address & Net::TcpClient::hostAddress() const {
        return _tcp_socket.localAddress();
    }

    const Net::Address & Net::TcpClient::remoteAddress() const {
        return _tcp_socket.peerAddress();
    }

    bool Net::TcpClient::option(SocketOption option, OptionValue &value) const {
        bool ok{};
        auto opt_val = _tcp_socket.option(option, &ok);
        if (ok) value = opt_val;
        return ok;
    }

    Net::SocketState Net::TcpClient::state() const {
        return _tcp_socket.state();
    }

    Net::SocketError Net::TcpClient::lastError() const {
        return  _tcp_socket.lastError();
    }

    void Net::TcpClient::initConfig() {
        _tcp_socket.setOption(Net::SocketOption::KeepAlive, true);
        _tcp_socket.setOption(Net::SocketOption::NoDelay, true);
#ifdef TINY_CPP_MY_OS_WINDOWS
        _tcp_socket.setOption(Net::SocketOption::RecvBufTimeout, 10000);
        _tcp_socket.setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
        struct timeval tv{10, 0};
        _tcp_socket.setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
        _tcp_socket.setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
#endif
    }

    Net::TcpServer::TcpServer(uint16_t port, bool use_ipv6) : _tcp_socket() {
        _tcp_socket.setLocalAddress(use_ipv6 ? "::" : "0.0.0.0", port, use_ipv6);
        initConfig();
    }

    Net::TcpServer::~TcpServer() {
        for (auto& con : _clients_list) {
            con.close();
        }
        _tcp_socket.close();
    }

    bool Net::TcpServer::setPort(uint16_t port) {
        return _tcp_socket.setLocalPort(port);
    }

    bool Net::TcpServer::setNonBlocking(bool enable) {
        bool ok{};
        _tcp_socket.setOption(SocketOption::NonBlocking, enable, &ok);
        return ok;
    }

    bool Net::TcpServer::listen(uint16_t port, int max_connections) {
        return _tcp_socket.listen(port, max_connections);
    }

    bool Net::TcpServer::listen(int max_connections) {
        return _tcp_socket.listen(max_connections);
    }

    bool Net::TcpServer::close() {
        return _tcp_socket.close();
    }

    bool Net::TcpServer::shutdown() {
        return _tcp_socket.shutdown();
    }

    bool Net::TcpServer::hasNewConnection() {
        bool ok{};
        Socket new_socket = _tcp_socket.accept(&ok);
        if (ok) {
            _clients_list.emplace_back(std::move(new_socket));
            _new_socket = &_clients_list.back();
        }
        return ok;
    }

    Net::Socket* Net::TcpServer::nextNewConnection() {
        auto ret = _new_socket;
        _new_socket = nullptr;
        return ret;
    }

    bool Net::TcpServer::send(const NetDatas &data, const Address *dest_host, int *send_failed_count) {
        bool is_all_successful{true};
        if (!dest_host) {
            for (auto& con : _clients_list) {
                bool ok = con.send(data);
                if (!ok) {
                    if (send_failed_count) *send_failed_count += 1;
                    is_all_successful = false;
                }
            }
            return is_all_successful;
        }
        auto iter = std::find_if(_clients_list.begin(), _clients_list.end(), [&dest_host](const Socket& s) {
            return s.localAddress() == *dest_host;
        });
        if (iter != _clients_list.end()) {
            return iter->send(data);
        }
        return false;
    }

    bool Net::TcpServer::receive(NetDatas & data, size_t max_size, int *recv_length,
                                 Address *dest_host, Address *src_host) {
        if (!dest_host) {
            bool ok{};
            for (auto & socket: _clients_list) {
                ok = socket.recv(data, max_size, recv_length);
                if (ok) {
                    if (src_host) src_host->setAddress(socket.localAddress());
                    break;
                }
            }
            return ok;
        }
        auto iter = std::find_if(_clients_list.begin(), _clients_list.end(), [&dest_host](const Socket& s) {
            return s.localAddress() == *dest_host;
        });
        if (iter != _clients_list.end()) {
            bool ok = iter->recv(data, max_size, recv_length);
            if (ok && src_host) {
                src_host->setAddress(iter->localAddress());
            }
            return ok;
        }
        return false;
    }

    const Net::Address & Net::TcpServer::serverAddress() const {
        return _tcp_socket.localAddress();
    }

    uint16_t Net::TcpServer::listeningPort() const {
        return _tcp_socket.localAddress().port();
    }

    bool Net::TcpServer::nonBlocking() const {
        bool ok{};
        auto opt_val = _tcp_socket.option(SocketOption::NonBlocking, &ok);
        return ok ? opt_val.var.i : false;
    }

    size_t Net::TcpServer::connectedCount() const {
        return _clients_list.size();
    }

    bool Net::TcpServer::connectedAddress(size_t index, Address *dest_address) const {
        if (index >= _clients_list.size()) return false;
        auto& addr = _clients_list[index].localAddress();
        if (dest_address) {
            dest_address->setAddress(addr.toString().data(), addr.port(), addr.isIPv6());
        }
        return true;
    }

    void Net::TcpServer::initConfig() {
        _tcp_socket.setOption(Net::SocketOption::KeepAlive, true);
        _tcp_socket.setOption(Net::SocketOption::NoDelay, true);
#ifdef TINY_CPP_MY_OS_WINDOWS
        _tcp_socket.setOption(Net::SocketOption::RecvBufTimeout, 10000);
        _tcp_socket.setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
        struct timeval tv{10, 0};
        _tcp_socket.setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
        _tcp_socket.setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
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