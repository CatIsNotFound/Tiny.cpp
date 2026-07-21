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

#include "Socket.hpp"
#ifdef TINY_CPP_MY_OS_WINDOWS
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define INVALID_SOCKET_VAL INVALID_SOCKET
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define INVALID_SOCKET_VAL (-1)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif
#endif


namespace Tiny {
#ifdef TINY_CPP_MY_OS_WINDOWS
    class WinSocketInit {
    public:
        WinSocketInit() {
            WSADATA wsa;
            WSAStartup(MAKEWORD(2, 2), &wsa);
        }
        ~WinSocketInit() {
            WSACleanup();
        }

        WinSocketInit(const WinSocketInit&) = delete;
        WinSocketInit& operator=(const WinSocketInit&) = delete;
        WinSocketInit(WinSocketInit&&) = delete;
        WinSocketInit& operator=(WinSocketInit&&) = delete;

    private:
        static WinSocketInit _instance;
    };

    WinSocketInit WinSocketInit::_instance{};
#endif

    namespace Socket_Impl {
        Net::Handle create(const Net::Address& address, bool use_udp = false) {
            return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, use_udp ? SOCK_DGRAM : SOCK_STREAM,
                use_udp ? IPPROTO_UDP : IPPROTO_TCP);
        }

        int close(Net::Handle socket) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::closesocket(socket);
#else
            return ::close(socket);
#endif
        }

        int connect(Net::Handle socket, const Net::Address& address) {
            return ::connect(socket, static_cast<sockaddr*>(address.address()),
                address.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
        }

        int bind(Net::Handle socket, const Net::Address& address) {
            return ::bind(socket, static_cast<sockaddr*>(address.address()),
                address.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
        }

        int listen(Net::Handle socket, uint16_t max_connections) {
            return ::listen(socket, max_connections);
        }

        Net::Handle accept(Net::Handle socket, Net::Address& address) {
            SOCKET new_client{};
            if (address.isIPv6()) {
                sockaddr_in6 ipv6{};
                socklen_t ipv6_size = sizeof(sockaddr_in6);
                new_client = ::accept(socket, reinterpret_cast<sockaddr*>(&ipv6), &ipv6_size);
                if (new_client != INVALID_SOCKET_VAL) {
                    char ip_addr[INET6_ADDRSTRLEN] = {};
                    inet_ntop(AF_INET6, &ipv6.sin6_addr, ip_addr, INET6_ADDRSTRLEN);
                    address.setAddress(ip_addr, ntohs(ipv6.sin6_port), true);
                }
            } else {
                sockaddr_in ipv4{};
                socklen_t ipv4_size = sizeof(sockaddr_in);
                new_client = ::accept(socket, reinterpret_cast<sockaddr*>(&ipv4), &ipv4_size);
                if (new_client != INVALID_SOCKET_VAL) {
                    char ip_addr[INET_ADDRSTRLEN] = {};
                    inet_ntop(AF_INET, &ipv4.sin_addr, ip_addr, INET_ADDRSTRLEN);
                    address.setAddress(ip_addr, ntohs(ipv4.sin_port), true);
                }
            }
            return new_client;
        }

        bool send(Net::Handle socket, const Net::Datas& datas) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::send(socket, datas.data(), datas.size(), 0) > 0;
#else
            return ::send(socket, datas.data(), datas.size(), MSG_NOSIGNAL) > 0;
#endif
        }

        bool send(Net::Handle socket, const std::string& datas) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::send(socket, datas.data(), datas.size(), 0) > 0;
#else
            return ::send(socket, datas.data(), datas.size(), MSG_NOSIGNAL) > 0;
#endif
        }

        int recv(Net::Handle socket, Net::Datas& datas, size_t max_len) {
            if (max_len > 0) datas.resize(max_len + 1);
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::recv(socket, &datas[0], datas.size(), 0);
#else
            return ::recv(socket, &datas[0], datas.size(), MSG_NOSIGNAL);
#endif
        }

        int recv(Net::Handle socket, std::string& datas, size_t max_len) {
            if (max_len > 0) datas.resize(max_len + 1);
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::recv(socket, &datas[0], datas.size(), 0);
#else
            return ::recv(socket, &datas[0], datas.size(), MSG_NOSIGNAL);
#endif
        }
    }

    Net::Address::Address(const char *address, uint16_t port, bool use_ipv6) {
        validate(address, port, use_ipv6);
    }

    Net::Address::Address(const char *address, Protocol protocol_num, bool use_ipv6) {
        validate(address, static_cast<uint16_t>(protocol_num), use_ipv6);
    }

    Net::Address::~Address() {
        if (_addr) {
            if (_use_ipv6) {
                delete static_cast<sockaddr_in6*>(_addr);
            } else {
                delete static_cast<sockaddr_in*>(_addr);
            }
            _addr = nullptr;
        }
    }

    std::string Net::Address::toString(bool* ok) const {
        if (!_valid || !_addr) return {};
        char result[64] = {};
        bool is_ok{};
        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr);
            is_ok = inet_ntop(AF_INET6, &ipv6->sin6_addr, result, 64) != nullptr;;
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr);
            is_ok = inet_ntop(AF_INET, &ipv4->sin_addr, result, 64) != nullptr;
        }
        if (ok) *ok = is_ok;
        return result;
    }


    void* Net::Address::address() const {
        return _addr;
    }

    uint16_t Net::Address::port() const {
        return _port;
    }

    bool Net::Address::isValid() const {
        return _valid;
    }

    bool Net::Address::isIPv6() const {
        return _use_ipv6;
    }

    void Net::Address::setAddress(const char *address, uint16_t port, bool use_ipv6) {
        validate(address, port, use_ipv6);
    }

    void Net::Address::setAddress(const char *address, Protocol port, bool use_ipv6) {
        validate(address, static_cast<uint16_t>(port), use_ipv6);
    }

    void Net::Address::setPort(uint16_t port) {
        if (!_valid) return;
        _port = port;
        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr);
            ipv6->sin6_port = htons(port);
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr);
            ipv4->sin_port = htons(port);
        }
    }

    void Net::Address::setPort(Protocol protocol_num) {
        if (!_valid) return;
        _port = static_cast<uint16_t>(protocol_num);
        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr);
            ipv6->sin6_port = htons(_port);
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr);
            ipv4->sin_port = htons(_port);
        }
    }

    Net::Address Net::Address::localHost() {
        return {"127.0.0.1", 0};
    }

    Net::Address Net::Address::localHostIPv6() {
        return {"::1", 0, true};
    }

    Net::Address::Address(Address && addr) noexcept {
        _addr = addr._addr;
        addr._addr = nullptr;
        _port = addr._port;
        addr._port = 0;
        _use_ipv6 = addr._use_ipv6;
        addr._use_ipv6 = false;
        _valid = addr._valid;
        addr._valid = false;
    }

    Net::Address & Net::Address::operator=(Address && addr) noexcept {
        _addr = addr._addr;
        addr._addr = nullptr;
        _port = addr._port;
        addr._port = 0;
        _use_ipv6 = addr._use_ipv6;
        addr._use_ipv6 = false;
        _valid = addr._valid;
        addr._valid = false;
        return *this;
    }

    bool Net::Address::operator==(const Address &addr) const {
        if (_use_ipv6 != addr._use_ipv6) return false;
        if (_port != addr._port) return false;
        if (toString() != addr.toString()) return false;
        return true;
    }

    bool Net::Address::operator!=(const Address &addr) const {
        if (_use_ipv6 != addr._use_ipv6) return true;
        if (_port != addr._port) return true;
        if (toString() != addr.toString()) return true;
        return false;
    }

    void Net::Address::validate(const char *full_address, uint16_t port, bool use_ipv6) {
        if (_addr) {
            if (_use_ipv6) {
                delete static_cast<sockaddr_in6*>(_addr);
            } else {
                delete static_cast<sockaddr_in*>(_addr);
            }
            _addr = nullptr;
        }
        if (use_ipv6) {
            auto new_addr = new sockaddr_in6();
            memset(new_addr, 0, sizeof(sockaddr_in6));
            _valid = inet_pton(AF_INET6, full_address, &new_addr->sin6_addr) == 1;
            if (!_valid) {
                delete new_addr;
                return;
            }
            new_addr->sin6_family = AF_INET6;
            new_addr->sin6_port = htons(port);
            _addr = new_addr;
        } else {
            auto new_addr = new sockaddr_in();
            memset(new_addr, 0, sizeof(sockaddr_in));
            _valid = inet_pton(AF_INET, full_address, &new_addr->sin_addr) == 1;
            if (!_valid) {
                delete new_addr;
                return;
            }
            new_addr->sin_family = AF_INET;
            new_addr->sin_port = htons(port);
            _addr = new_addr;
        }
        _port = port;
        _use_ipv6 = use_ipv6;
    }


    Net::Socket::Socket() : _handle(INVALID_SOCKET_VAL) {}

    Net::Socket::Socket(Address &&address) {
        _address = std::move(address);
        _handle = INVALID_SOCKET_VAL;
    }

    Net::Socket::Socket(Socket &&other) noexcept {
        _handle = other._handle;
        _address = std::move(other._address);
        other._handle = INVALID_SOCKET_VAL;
    }

    Net::Socket &Net::Socket::operator=(Socket &&other) noexcept {
        _handle = other._handle;
        _address = std::move(other._address);
        other._handle = INVALID_SOCKET_VAL;
        return *this;
    }

    Net::Socket::~Socket() {
        if (_handle != INVALID_SOCKET_VAL) close();
    }

    bool Net::Socket::connect(const char *address, uint16_t port, bool use_udp) {
        _address.setAddress(address, port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::connect(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::connect(const char *address, Protocol port, bool use_udp) {
        _address.setAddress(address, port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::connect(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::connect(Address &&address, bool use_udp) {
        _address = std::move(address);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::connect(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::connect(bool use_udp) {
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::connect(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::bind(bool use_udp) {
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::bind(const char *address, uint16_t port, bool use_udp) {
        _address.setAddress(address, port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::bind(const char *address, Protocol port, bool use_udp) {
        _address.setAddress(address, port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::bind(Address &&address, bool use_udp) {
        _address = std::move(address);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::listen(uint16_t port, bool use_udp) {
        _address.setAddress("0.0.0.0", port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;

        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        ret = Socket_Impl::listen(_handle, UINT16_MAX);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::listen(Protocol protocol_num, bool use_udp) {
        auto port = static_cast<uint16_t>(protocol_num);
        _address.setAddress("0.0.0.0", port);
        if (!_address.isValid() || _handle != INVALID_SOCKET_VAL) return false;
        _handle = Socket_Impl::create(_address, use_udp);
        if (_handle == INVALID_SOCKET_VAL) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, "\1", 1);
        if (!use_udp) setsockopt(_handle, SOL_SOCKET, SO_KEEPALIVE, "\1", 1);
#else
        int opt_val = 1;
        setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
        if (!use_udp) setsockopt(_handle, SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(opt_val));
#endif
        auto ret = Socket_Impl::bind(_handle, _address);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        ret = Socket_Impl::listen(_handle, port);
        if (ret == SOCKET_ERROR) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        return true;
    }

    bool Net::Socket::accept(Socket &socket) const {
        bool ok;
        socket = accept(&ok);
        return ok;
    }

    Net::Socket Net::Socket::accept(bool *ok) const {
        if (!_address.isValid() || _handle == INVALID_SOCKET_VAL) {
            if (ok) *ok = false;
            return {};
        }
        Address new_address(_address.toString().data(), _address.port(), _address.isIPv6());
        auto handle = Socket_Impl::accept(_handle, new_address);
        if (handle == INVALID_SOCKET_VAL) {
            if (ok) *ok = false;
            return {};
        }
        Socket cli_socket;
        cli_socket._handle = handle;
        cli_socket._address = std::move(new_address);
        if (ok) *ok = true;
        return cli_socket;
    }


    void Net::Socket::close() {
        if (_handle != INVALID_SOCKET_VAL) {
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
        }
    }

    bool Net::Socket::send(const std::string &message) {
        if (!_address.isValid() || _handle == INVALID_SOCKET_VAL) return false;
        return Socket_Impl::send(_handle, message);
    }

    bool Net::Socket::send(const Datas &data) {
        if (!_address.isValid() || _handle == INVALID_SOCKET_VAL) return false;
        return Socket_Impl::send(_handle, data);
    }

    bool Net::Socket::recv(Datas &data, size_t max_length, int* received_length) {
        if (!_address.isValid() || _handle == INVALID_SOCKET_VAL) return false;
        auto len = Socket_Impl::recv(_handle, data, max_length);
        if (received_length) *received_length = len;
        return len != SOCKET_ERROR;
    }

    bool Net::Socket::recv(std::string &message, size_t max_length, int* received_length) {
        if (!_address.isValid() || _handle == INVALID_SOCKET_VAL) return false;
        auto len = Socket_Impl::recv(_handle, message, max_length);
        if (received_length) *received_length = len;
        return len != SOCKET_ERROR;
    }

    bool Net::Socket::setUnblockEnabled(bool enabled) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto val = static_cast<u_long>(enabled);
        return ioctlsocket(_handle, FIONBIO, &val) == NO_ERROR;
#else
        auto ret = fcntl(_handle, F_GETFL, 0);
        if (ret == -1) return false;
        if (enabled) ret |= O_NONBLOCK;
        else ret &= ~O_NONBLOCK;
        return fcntl(_handle, F_SETFL, ret) != -1;
#endif
    }

    bool Net::Socket::isNull() const {
        return _handle == INVALID_SOCKET_VAL;
    }

    std::string Net::getLastError(int* err_code) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto err = WSAGetLastError();
        if (err_code) *err_code = err;
        if (err == 0) {
            return "No errors found!";
        }
        WSASetLastError(0);
        void* msg_buf = nullptr;
        auto out_size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            (LPSTR)&msg_buf,
            0,
            nullptr);

        if (out_size > 0) {
            std::string info = static_cast<char*>(msg_buf);
            info.pop_back();
            info.pop_back();
            if (msg_buf) LocalFree(msg_buf);
            return info;
        }
        if (msg_buf) LocalFree(msg_buf);
        char out[32]{};
        snprintf(out, 32, "Unknown error! code: 0x%x", err);
        out[31] = '\0';
        return out;
#else
        int err = errno;
        if (err_code) *err_code = err;
        if (err == 0) {
            return "No errors found!";
        }
        auto err_text = strerror(err);
        if (err_text == nullptr) {
            std::ostringstream oss;
            oss << "Unknown error! code: 0x" << std::hex << err;
            return oss.str();
        }
        return err_text;
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