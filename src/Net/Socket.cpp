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

#include "SocketAdv.hpp"
#include <sstream>
#include <mutex>
#include <memory>
#define SET_SOCK_OPT_ERR static_cast<int>(Net::SocketError::SetOptionError) * (-1)
#ifdef TINY_CPP_MY_OS_WINDOWS
#define SOCK_LEN_T int
#else
#define SOCK_LEN_T unsigned int
#endif

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
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>
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

std::string num2str(int num) {
    std::string ret;
    while (num != 0) {
        ret.insert(ret.begin(), num % 10 + '0');
        num /= 10;
    }
    if (num * (-1) > 0) ret.insert(ret.begin(), '-');
    return ret;
}

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
#else
    class SocketInit {
    public:
        SocketInit() {
            signal(SIGPIPE, SIG_IGN);
        }

        ~SocketInit() = default;

        SocketInit(const SocketInit&) = delete;
        SocketInit& operator=(const SocketInit&) = delete;
        SocketInit(SocketInit&&) = delete;
        SocketInit& operator=(SocketInit&&) = delete;
    private:
        static SocketInit _instance;
    };

    SocketInit SocketInit::_instance{};
#endif

#ifdef TINY_CPP_MY_OS_WINDOWS
    const std::unordered_map<int, Net::SocketError> __SocketErrorsMap__{
        {0,                 Net::SocketError::Success},
        {WSAEINVAL,         Net::SocketError::InvalidParameter},
        {WSAEPROTONOSUPPORT,Net::SocketError::ProtoNotSupported},
        {WSAEPROTOTYPE,     Net::SocketError::ProtoNotSupported},
        {WSAENOTCONN,       Net::SocketError::SocketClosed},
        {WSAESHUTDOWN,      Net::SocketError::SocketClosed},
        {WSAEISCONN,        Net::SocketError::SocketInUse},
        {WSAEADDRINUSE,     Net::SocketError::AddressInUse},
        {WSAEADDRNOTAVAIL,  Net::SocketError::AddressNotAvailable},
        {WSAEAFNOSUPPORT,   Net::SocketError::AddressNotSupport},
        {WSAECONNREFUSED,   Net::SocketError::ConnectionRefused},
        {WSAEINPROGRESS,    Net::SocketError::ConnectionInProgress},
        {WSAEALREADY,       Net::SocketError::ConnectionInProgress},
        {WSAECONNRESET,     Net::SocketError::ConnectionReset},
        {WSAECONNABORTED,   Net::SocketError::ConnectionAborted},
        {WSAETIMEDOUT,      Net::SocketError::ConnectionTimeout},
        {WSAENETUNREACH,    Net::SocketError::NetworkUnreachable},
        {WSAEHOSTUNREACH,   Net::SocketError::HostUnreachable},
        {WSAENETDOWN,       Net::SocketError::NetworkDown},
        {WSAEMSGSIZE,       Net::SocketError::MessageTooLong},
        {WSAEWOULDBLOCK,    Net::SocketError::ResourceUnavailable},
        {WSAEACCES,         Net::SocketError::OperationDenied},
        {WSAENOBUFS,        Net::SocketError::SystemResourceLimit},
        {WSAEMFILE,         Net::SocketError::SystemResourceLimit},
        {WSAEINTR,          Net::SocketError::OperationCancelled},
        {WSAEOPNOTSUPP,     Net::SocketError::OperationNotSupported},
        {WSAENOTSOCK,       Net::SocketError::OperationNotSupported},
        {WSAEFAULT,         Net::SocketError::SegmentationFault}
    };
#else
    static std::unordered_map<int, Net::SocketError> __SocketErrorsMap__{
            {0,                 Net::SocketError::Success},
            {EINVAL,            Net::SocketError::InvalidParameter},
            {EPROTONOSUPPORT,   Net::SocketError::ProtoNotSupported},
            {EPROTOTYPE,        Net::SocketError::ProtoNotSupported},
            {ENOTCONN,          Net::SocketError::SocketClosed},
            {ESHUTDOWN,         Net::SocketError::SocketClosed},
            {EISCONN,           Net::SocketError::SocketInUse},
            {EADDRINUSE,        Net::SocketError::AddressInUse},
            {EADDRNOTAVAIL,     Net::SocketError::AddressNotAvailable},
            {EAFNOSUPPORT,      Net::SocketError::AddressNotSupport},
            {ECONNREFUSED,      Net::SocketError::ConnectionRefused},
            {EINPROGRESS,       Net::SocketError::ConnectionInProgress},
            {EALREADY,          Net::SocketError::ConnectionInProgress},
            {ECONNRESET,        Net::SocketError::ConnectionReset},
            {EPIPE,             Net::SocketError::ConnectionReset},
            {ECONNABORTED,      Net::SocketError::ConnectionAborted},
            {ETIMEDOUT,         Net::SocketError::ConnectionTimeout},
            {ENETUNREACH,       Net::SocketError::NetworkUnreachable},
            {EHOSTUNREACH,      Net::SocketError::HostUnreachable},
            {ENETDOWN,          Net::SocketError::NetworkDown},
            {EMSGSIZE,          Net::SocketError::MessageTooLong},
            {EWOULDBLOCK,       Net::SocketError::ResourceUnavailable},
            {EACCES,            Net::SocketError::OperationDenied},
            {ENOBUFS,           Net::SocketError::SystemResourceLimit},
            {EMFILE,            Net::SocketError::SystemResourceLimit},
            {ENFILE,            Net::SocketError::SystemResourceLimit},
            {EINTR,             Net::SocketError::OperationCancelled},
            {EOPNOTSUPP,        Net::SocketError::OperationNotSupported},
            {ENOTSUP,          Net::SocketError::OperationNotSupported},
            {ENOTSOCK,          Net::SocketError::OperationNotSupported},
            {EFAULT,            Net::SocketError::SegmentationFault}
    };
#endif

    const std::unordered_map<Net::SocketError, const char*> __SocketErrorStrings__{
            {Net::SocketError::Success,                 "Tiny::Net::SocketError::Success"},
            {Net::SocketError::InvalidParameter,        "Tiny::Net::SocketError::InvalidParameter"},
            {Net::SocketError::SetOptionError,          "Tiny::Net::SocketError::SetOptionError"},
            {Net::SocketError::ProtoNotSupported,       "Tiny::Net::SocketError::ProtoNotSupported"},
            {Net::SocketError::SocketIsNotOpened,       "Tiny::Net::SocketError::SocketIsNotOpened"},
            {Net::SocketError::SocketClosed,            "Tiny::Net::SocketError::SocketClosed"},
            {Net::SocketError::SocketInUse,             "Tiny::Net::SocketError::SocketInUse"},
            {Net::SocketError::AddressInUse,            "Tiny::Net::SocketError::AddressInUse"},
            {Net::SocketError::AddressNotAvailable,     "Tiny::Net::SocketError::AddressNotAvailable"},
            {Net::SocketError::AddressNotSupport,       "Tiny::Net::SocketError::AddressNotSupport"},
            {Net::SocketError::ConnectionRefused,       "Tiny::Net::SocketError::ConnectionRefused"},
            {Net::SocketError::ConnectionInProgress,    "Tiny::Net::SocketError::ConnectionInProgress"},
            {Net::SocketError::ConnectionInUsed,        "Tiny::Net::SocketError::ConnectionInUsed"},
            {Net::SocketError::ConnectionReset,         "Tiny::Net::SocketError::ConnectionReset"},
            {Net::SocketError::ConnectionAborted,       "Tiny::Net::SocketError::ConnectionAborted"},
            {Net::SocketError::ConnectionTimeout,       "Tiny::Net::SocketError::ConnectionTimeout"},
            {Net::SocketError::NetworkUnreachable,      "Tiny::Net::SocketError::NetworkUnreachable"},
            {Net::SocketError::HostUnreachable,         "Tiny::Net::SocketError::HostUnreachable"},
            {Net::SocketError::NetworkDown,             "Tiny::Net::SocketError::NetworkDown"},
            {Net::SocketError::MessageTooLong,          "Tiny::Net::SocketError::MessageTooLong"},
            {Net::SocketError::ResourceUnavailable,     "Tiny::Net::SocketError::ResourceUnavailable"},
            {Net::SocketError::OperationDenied,         "Tiny::Net::SocketError::OperationDenied"},
            {Net::SocketError::SystemResourceLimit,     "Tiny::Net::SocketError::SystemResourceLimit"},
            {Net::SocketError::OperationInProgress,     "Tiny::Net::SocketError::OperationInProgress"},
            {Net::SocketError::OperationCancelled,      "Tiny::Net::SocketError::OperationCancelled"},
            {Net::SocketError::OperationNotSupported,   "Tiny::Net::SocketError::OperationNotSupported"},
            {Net::SocketError::SegmentationFault,       "Tiny::Net::SocketError::SegmentationFault"},
            {Net::SocketError::UnknownError,            "Tiny::Net::SocketError::UnknownError"}
    };

    struct SocketSetting {
        Net::Advanced::Setter setter;
        Net::Advanced::Getter getter;
    };

    namespace Socket_Impl {
#ifdef TINY_CPP_MY_OS_WINDOWS
        constexpr bool USING_WIN32 = true;
#else
        constexpr bool USING_WIN32 = false;
#endif
        class InitMapSet {
            InitMapSet() {
                _SOCKET_LEVEL_MAP = {
                    {static_cast<uint32_t>(Net::SocketOption::AllowedBroadcast), SO_BROADCAST},
                    {static_cast<uint32_t>(Net::SocketOption::DontRoute),        SO_DONTROUTE},
                    {static_cast<uint32_t>(Net::SocketOption::KeepAlive),        SO_KEEPALIVE},
                    {static_cast<uint32_t>(Net::SocketOption::SendBufSize),      SO_SNDBUF},
                    {static_cast<uint32_t>(Net::SocketOption::RecvBufSize),      SO_RCVBUF},
                    {static_cast<uint32_t>(Net::SocketOption::SendBufTimeout),   SO_SNDTIMEO},
                    {static_cast<uint32_t>(Net::SocketOption::RecvBufTimeout),   SO_RCVTIMEO},
                    {static_cast<uint32_t>(Net::SocketOption::Linger),           SO_LINGER},
                    {static_cast<uint32_t>(Net::SocketOption::ReuseAddr),        SO_REUSEADDR},
                    {static_cast<uint32_t>(Net::SocketOption::NativeSocketError),SO_ERROR}
                };
                _TCP_LEVEL_MAP = {
                    {static_cast<uint32_t>(Net::SocketOption::NoDelay),          TCP_NODELAY},
#if defined(TCP_KEEPIDLE)
                    {static_cast<uint32_t>(Net::SocketOption::KeepIdle),         TCP_KEEPIDLE},
#elif defined(TCP_KEEPALIVE)
                    {static_cast<uint32_t>(Net::SocketOption::KeepIdle),         TCP_KEEPALIVE},
#endif
#if defined(TCP_KEEPINTVL)
                    {static_cast<uint32_t>(Net::SocketOption::KeepInterval),     TCP_KEEPINTVL},
#endif
#if defined(TCP_KEEPCNT)
                    {static_cast<uint32_t>(Net::SocketOption::KeepCount),        TCP_KEEPCNT},
#endif
#if defined(TCP_MAXSEG)
                    {static_cast<uint32_t>(Net::SocketOption::MaxSegmentSize),   TCP_MAXSEG}
#endif
                };

                _IP_LEVEL_MAP = {
                    {static_cast<uint32_t>(Net::SocketOption::TTL),                IP_TTL},
                    {static_cast<uint32_t>(Net::SocketOption::TOS),                IP_TOS},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastTTL),       IP_MULTICAST_TTL},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastLoopback),  IP_MULTICAST_LOOP},
                    {static_cast<uint32_t>(Net::SocketOption::AddMembership),      IP_ADD_MEMBERSHIP},
                    {static_cast<uint32_t>(Net::SocketOption::RemoveMembership),   IP_DROP_MEMBERSHIP}
                };
                _IPV6_LEVEL_MAP = {
                    {static_cast<uint32_t>(Net::SocketOption::MapIPv6Only),            IPV6_V6ONLY},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastLoopbackIPv6),  IPV6_MULTICAST_LOOP},
                    {static_cast<uint32_t>(Net::SocketOption::JoinGroup),              IPV6_JOIN_GROUP},
                    {static_cast<uint32_t>(Net::SocketOption::LeaveGroup),             IPV6_LEAVE_GROUP},
                    {static_cast<uint32_t>(Net::SocketOption::UnicastHops),            IPV6_UNICAST_HOPS},
                    {static_cast<uint32_t>(Net::SocketOption::MuticastHops),           IPV6_MULTICAST_HOPS}
                };
                _SOCKET_LEVEL_TMAP = {
                    {static_cast<uint32_t>(Net::SocketOption::AllowedBroadcast), Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::DontRoute),        Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::KeepAlive),        Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::SendBufSize),      Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::RecvBufSize),      Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::SendBufTimeout),
                          USING_WIN32 ? Net::OptionValue::ValueType::Int : Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::RecvBufTimeout),
                          USING_WIN32 ? Net::OptionValue::ValueType::Int : Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::Linger),           Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::ReuseAddr),        Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::NativeSocketError),Net::OptionValue::ValueType::Int}
                };
                _TCP_LEVEL_TMAP = {
                    {static_cast<uint32_t>(Net::SocketOption::NoDelay),          Net::OptionValue::ValueType::Int},
#if defined(TCP_KEEPIDLE)
                    {static_cast<uint32_t>(Net::SocketOption::KeepIdle),         Net::OptionValue::ValueType::Int},
#elif defined(TCP_KEEPALIVE)
                    {static_cast<uint32_t>(Net::SocketOption::KeepIdle),         Net::OptionValue::ValueType::Int},
#endif
#if defined(TCP_KEEPINTVL)
                    {static_cast<uint32_t>(Net::SocketOption::KeepInterval),     Net::OptionValue::ValueType::Int},
#endif
#if defined(TCP_KEEPCNT)
                    {static_cast<uint32_t>(Net::SocketOption::KeepCount),        Net::OptionValue::ValueType::Int},
#endif
                    {static_cast<uint32_t>(Net::SocketOption::MaxSegmentSize),   Net::OptionValue::ValueType::Int}
                };
                _IP_LEVEL_TMAP = {
                    {static_cast<uint32_t>(Net::SocketOption::TTL),               Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::TOS),               Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastTTL),      Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastLoopback), Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::AddMembership),     Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::RemoveMembership),  Net::OptionValue::ValueType::Custom}
                };
                _IPV6_LEVEL_TMAP = {
                    {static_cast<uint32_t>(Net::SocketOption::MapIPv6Only),           Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::MulticastLoopbackIPv6), Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::JoinGroup),             Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::LeaveGroup),            Net::OptionValue::ValueType::Custom},
                    {static_cast<uint32_t>(Net::SocketOption::UnicastHops),           Net::OptionValue::ValueType::Int},
                    {static_cast<uint32_t>(Net::SocketOption::MuticastHops),          Net::OptionValue::ValueType::Int}
                };
                _SOC_SIZE_OF_MAP = {
                    {static_cast<uint32_t>(Net::SocketOption::SendBufTimeout),   sizeof(timeval)},
                    {static_cast<uint32_t>(Net::SocketOption::RecvBufTimeout),   sizeof(timeval)},
                    {static_cast<uint32_t>(Net::SocketOption::Linger),           sizeof(linger)},
                    {static_cast<uint32_t>(Net::SocketOption::AddMembership),    sizeof(ip_mreq)},
                    {static_cast<uint32_t>(Net::SocketOption::RemoveMembership), sizeof(ip_mreq)},
                    {static_cast<uint32_t>(Net::SocketOption::JoinGroup),        sizeof(ipv6_mreq)},
                    {static_cast<uint32_t>(Net::SocketOption::LeaveGroup),       sizeof(ipv6_mreq)},

                };
            }

        public:
            static InitMapSet& instance() {
                std::call_once(_once, [] {
                    _instance.reset(new InitMapSet());
                });
                return *_instance.get();
            }

            std::unordered_map<uint32_t, int>& socketLevelMap() {
                return _instance->_SOCKET_LEVEL_MAP;
            }

            std::unordered_map<uint32_t, int>& tcpLevelMap() {
                return _instance->_TCP_LEVEL_MAP;
            }

            std::unordered_map<uint32_t, int>& ipLevelMap() {
                return _instance->_IP_LEVEL_MAP;
            }

            std::unordered_map<uint32_t, int>& ipv6LevelMap() {
                return _instance->_IPV6_LEVEL_MAP;
            }

            std::unordered_map<uint32_t, Net::OptionValue::ValueType>& socketLevelTypeMap() {
                return _instance->_SOCKET_LEVEL_TMAP;
            }

            std::unordered_map<uint32_t, Net::OptionValue::ValueType>& tcpLevelTypeMap() {
                return _instance->_TCP_LEVEL_TMAP;
            }

            std::unordered_map<uint32_t, Net::OptionValue::ValueType>& ipLevelTypeMap() {
                return _instance->_IP_LEVEL_TMAP;
            }

            std::unordered_map<uint32_t, Net::OptionValue::ValueType>& ipv6LevelTypeMap() {
                return _instance->_IPV6_LEVEL_TMAP;
            }

            std::unordered_map<uint32_t, int>& socketSizeOfMap() {
                return _instance->_SOC_SIZE_OF_MAP;
            }

        private:
            std::unordered_map<uint32_t, int> _SOCKET_LEVEL_MAP;
            std::unordered_map<uint32_t, int> _TCP_LEVEL_MAP;
            std::unordered_map<uint32_t, int> _IP_LEVEL_MAP;
            std::unordered_map<uint32_t, int> _IPV6_LEVEL_MAP;
            std::unordered_map<uint32_t, Net::OptionValue::ValueType> _SOCKET_LEVEL_TMAP;
            std::unordered_map<uint32_t, Net::OptionValue::ValueType> _TCP_LEVEL_TMAP;
            std::unordered_map<uint32_t, Net::OptionValue::ValueType> _IP_LEVEL_TMAP;
            std::unordered_map<uint32_t, Net::OptionValue::ValueType> _IPV6_LEVEL_TMAP;
            std::unordered_map<uint32_t, int> _SOC_SIZE_OF_MAP;

            static std::once_flag _once;
            static std::unique_ptr<InitMapSet> _instance;
        };

        std::once_flag InitMapSet::_once;
        std::unique_ptr<InitMapSet> InitMapSet::_instance;

        bool setNativeSocketOption(Net::Handle handle, int opt_id, int opt_level, const Net::OptionValue& value) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            char* set_val;
            Net::OptionValue::Value v{};
            switch (value.type) {
                case Net::OptionValue::Int:
                    v.i = value.var.i;
                    set_val = reinterpret_cast<char*>(&v.i);
                    break;
                case Net::OptionValue::UInt:
                    v.u = value.var.u;
                    set_val = reinterpret_cast<char*>(&v.u);
                    break;
                case Net::OptionValue::Float:
                    v.f = value.var.f;
                    set_val = reinterpret_cast<char*>(&v.f);
                    break;
                case Net::OptionValue::String:
                    v.s = value.var.s;
                    set_val = v.s;
                    break;
                case Net::OptionValue::Custom:
                    v.v = value.var.v;
                    set_val = static_cast<char*>(v.v);
                    break;
                default:
                    v.v = nullptr;
                    set_val = nullptr;
                    break;
            }
            return ::setsockopt(handle, opt_level, opt_id, set_val, value.size) != SOCKET_ERROR;
#else
            const void* VAL;
            switch (value.type) {
                case Net::OptionValue::Int:
                    VAL = &value.var.i;
                    break;
                case Net::OptionValue::UInt:
                    VAL = &value.var.u;
                    break;
                case Net::OptionValue::Float:
                    VAL = &value.var.f;
                    break;
                case Net::OptionValue::String:
                    VAL = value.var.s;
                    break;
                default:
                    VAL = value.var.v;
                    break;
            }
            return ::setsockopt(handle, opt_level, opt_id, VAL, value.size) != SOCKET_ERROR;
#endif
        }

        int getNativeSocketOption(Net::Handle handle, int opt_id, int opt_level, Net::OptionValue& value) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            switch (value.type) {
                case Net::OptionValue::Int:
                    value.size = sizeof(int);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<char*>(&value.var.i), &value.size);
                    break;
                case Net::OptionValue::String:
                    /// Need user to manually set size of the value.
                    return ::getsockopt(handle, opt_level, opt_id,
                                       value.var.s, &value.size);
                    break;
                case Net::OptionValue::UInt:
                    value.size = sizeof(uint32_t);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<char*>(&value.var.u), &value.size);
                    break;
                case Net::OptionValue::Float:
                    value.size = sizeof(float);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<char*>(&value.var.f), &value.size);
                    break;
                case Net::OptionValue::Custom:
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<char*>(value.var.v), &value.size);
                    break;
                default:
                    value.unset();
                    break;
            }
#else
            switch (value.type) {
                case Net::OptionValue::Int:
                    value.size = sizeof(int);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<void*>(&value.var.i), &value.size);
                    break;
                case Net::OptionValue::String:
                    /// Need user to manually set size of the value.
                    return ::getsockopt(handle, opt_level, opt_id,
                                       value.var.s, &value.size);
                    break;
                case Net::OptionValue::UInt:
                    value.size = sizeof(uint32_t);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<void*>(&value.var.u), &value.size);
                    break;
                case Net::OptionValue::Float:
                    value.size = sizeof(float);
                    return ::getsockopt(handle, opt_level, opt_id,
                                       reinterpret_cast<void*>(&value.var.f), &value.size);
                    break;
                case Net::OptionValue::Custom:
                    return ::getsockopt(handle, opt_level, opt_id,
                                       value.var.v, &value.size);
                    break;
                default:
                    value.unset();
                    break;
            }
#endif
            return SOCKET_ERROR;
        }

        bool setSocketLevelOption(Net::Handle handle, uint32_t opt_id, const Net::OptionValue& value) {
            auto& __SOL_SOCKET_LEVEL_MAP__ = InitMapSet::instance().socketLevelMap();
            auto& __SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__ = InitMapSet::instance().socketLevelTypeMap();
            if (__SOL_SOCKET_LEVEL_MAP__.find(opt_id) != __SOL_SOCKET_LEVEL_MAP__.end()) {
                auto& so_id = __SOL_SOCKET_LEVEL_MAP__.at(opt_id);
                auto& expected_type = __SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__.at(opt_id);
                if (expected_type == value.type) {
                    return setNativeSocketOption(handle, so_id, SOL_SOCKET, value);
                }
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool getSocketLevelOption(Net::Handle handle, uint32_t opt_id, Net::OptionValue& value) {
            auto& __SOL_SOCKET_LEVEL_MAP__ = InitMapSet::instance().socketLevelMap();
            auto& __SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__ = InitMapSet::instance().socketLevelTypeMap();
            auto& __SOL_SOCKET_SIZE_OF_MAP__ = InitMapSet::instance().socketSizeOfMap();
            if (__SOL_SOCKET_LEVEL_MAP__.find(opt_id) != __SOL_SOCKET_LEVEL_MAP__.end()) {
                auto& so_id = __SOL_SOCKET_LEVEL_MAP__.at(opt_id);
                if (__SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__.find(opt_id) == __SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__.end()) {
                    value.unset();
                    return true;
                }
                auto var_type = __SOL_SOCKET_LEVEL_VALUE_TYPE_MAP__.at(opt_id);
                if (__SOL_SOCKET_SIZE_OF_MAP__.find(opt_id) != __SOL_SOCKET_SIZE_OF_MAP__.end()) {
                    value.size = __SOL_SOCKET_SIZE_OF_MAP__.at(opt_id);
                }
                value.type = var_type;
                return getNativeSocketOption(handle, so_id, SOL_SOCKET, value) != SOCKET_ERROR;
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool setTCPLevelOption(Net::Handle handle, uint32_t opt_id, const Net::OptionValue& value) {
            auto& __TCP_LEVEL_MAP__ = InitMapSet::instance().tcpLevelMap();
            auto& __TCP_LEVEL_TMAP__ = InitMapSet::instance().tcpLevelTypeMap();
            if (__TCP_LEVEL_MAP__.find(opt_id) != __TCP_LEVEL_MAP__.end()) {
                auto& op_id = __TCP_LEVEL_MAP__.at(opt_id);
                auto& expected_type = __TCP_LEVEL_TMAP__.at(opt_id);
                if (expected_type == value.type) {
                    return setNativeSocketOption(handle, op_id, IPPROTO_TCP, value);
                }
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool getTCPLevelOption(Net::Handle handle, uint32_t opt_id, Net::OptionValue& value) {
            auto& __TCP_LEVEL_MAP__ = InitMapSet::instance().tcpLevelMap();
            auto& __TCP_LEVEL_TMAP__ = InitMapSet::instance().tcpLevelTypeMap();
            auto& __SOL_SOCKET_SIZE_OF_MAP__ = InitMapSet::instance().socketSizeOfMap();
            if (__TCP_LEVEL_MAP__.find(opt_id) != __TCP_LEVEL_MAP__.end()) {
                auto& op_id = __TCP_LEVEL_MAP__.at(opt_id);
                value.type = __TCP_LEVEL_TMAP__.at(opt_id);
                if (__SOL_SOCKET_SIZE_OF_MAP__.find(opt_id) != __SOL_SOCKET_SIZE_OF_MAP__.end()) {
                    value.size = __SOL_SOCKET_SIZE_OF_MAP__.at(opt_id);
                }
                return getNativeSocketOption(handle, op_id, IPPROTO_TCP, value) != SOCKET_ERROR;
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool setIPLevelOption(Net::Handle handle, uint32_t opt_id, const Net::OptionValue& value) {
            auto& __IP_LEVEL_MAP__ = InitMapSet::instance().ipLevelMap();
            auto& __IP_LEVEL_TMAP__ = InitMapSet::instance().ipLevelTypeMap();
            if (__IP_LEVEL_MAP__.find(opt_id) != __IP_LEVEL_MAP__.end()) {
                auto& op_id = __IP_LEVEL_MAP__.at(opt_id);
                auto& expected_type = __IP_LEVEL_TMAP__.at(opt_id);
                if (expected_type == value.type) {
                    return setNativeSocketOption(handle, op_id, IPPROTO_IP, value);
                }
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool getIPLevelOption(Net::Handle handle, uint32_t opt_id, Net::OptionValue& value) {
            auto& __IP_LEVEL_MAP__ = InitMapSet::instance().ipLevelMap();
            auto& __SOL_SOCKET_SIZE_OF_MAP__ = InitMapSet::instance().socketSizeOfMap();
            auto& __IP_LEVEL_TMAP__ = InitMapSet::instance().ipLevelTypeMap();
            if (__IP_LEVEL_MAP__.find(opt_id) != __IP_LEVEL_MAP__.end()) {
                auto& op_id = __IP_LEVEL_MAP__.at(opt_id);
                value.type = __IP_LEVEL_TMAP__.at(opt_id);
                if (__SOL_SOCKET_SIZE_OF_MAP__.find(opt_id) != __SOL_SOCKET_SIZE_OF_MAP__.end()) {
                    value.size = __SOL_SOCKET_SIZE_OF_MAP__.at(opt_id);
                }
                return getNativeSocketOption(handle, op_id, IPPROTO_IP, value) != SOCKET_ERROR;
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool setIPv6LevelOption(Net::Handle handle, uint32_t opt_id, const Net::OptionValue& value) {
            auto& __IPV6_LEVEL_MAP__ = InitMapSet::instance().ipv6LevelMap();
            auto& __IPV6_LEVEL_TMAP__ = InitMapSet::instance().ipv6LevelTypeMap();
            if (__IPV6_LEVEL_MAP__.find(opt_id) != __IPV6_LEVEL_MAP__.end()) {
                auto& op_id = __IPV6_LEVEL_MAP__.at(opt_id);
                auto& expected_type = __IPV6_LEVEL_TMAP__.at(opt_id);
                if (expected_type == value.type) {
                    return setNativeSocketOption(handle, op_id, IPPROTO_IPV6, value);
                }
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        bool getIPv6LevelOption(Net::Handle handle, uint32_t opt_id, Net::OptionValue& value) {
            auto& __IPV6_LEVEL_MAP__ = InitMapSet::instance().ipv6LevelMap();
            auto& __IPV6_LEVEL_TMAP__ = InitMapSet::instance().ipv6LevelTypeMap();
            auto& __SOL_SOCKET_SIZE_OF_MAP__ = InitMapSet::instance().socketSizeOfMap();
            if (__IPV6_LEVEL_MAP__.find(opt_id) != __IPV6_LEVEL_MAP__.end()) {
                auto& op_id = __IPV6_LEVEL_MAP__.at(opt_id);
                value.type = __IPV6_LEVEL_TMAP__.at(opt_id);
                if (__SOL_SOCKET_SIZE_OF_MAP__.find(opt_id) != __SOL_SOCKET_SIZE_OF_MAP__.end()) {
                    value.size = __SOL_SOCKET_SIZE_OF_MAP__.at(opt_id);
                }
                return getNativeSocketOption(handle, op_id, IPPROTO_IPV6, value) != SOCKET_ERROR;
            }
#ifdef TINY_CPP_MY_OS_WINDOWS
            ::WSASetLastError(WSAEOPNOTSUPP);
#else
            errno = ENOTSUP;
#endif
            return false;
        }

        Net::Handle create(const Net::Address& address, Net::SocketType sock_type, uint8_t type = 0, uint8_t protocol = 0) {
            switch (sock_type) {
                case Net::SocketType::TCP:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
                case Net::SocketType::UDP:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                case Net::SocketType::SCTP:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
                case Net::SocketType::ICMP:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, SOCK_RAW, IPPROTO_ICMP);
                case Net::SocketType::ICMPV6:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, SOCK_RAW, IPPROTO_ICMPV6);
                case Net::SocketType::Custom:
                    return ::socket(address.isIPv6() ? AF_INET6 : AF_INET, type, protocol);
                default:
                    return INVALID_SOCKET_VAL;
            }
        }

        int close(Net::Handle socket) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::closesocket(socket);
#else
            return ::close(socket);
#endif
        }

        int shutdown(Net::Handle socket) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::shutdown(socket, SD_BOTH);
#else
            return ::shutdown(socket, SHUT_RDWR);
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

        int listen(Net::Handle socket, int max_connections) {
            return ::listen(socket, max_connections);
        }

        Net::Handle accept(Net::Handle socket, Net::Address& address, bool use_ipv6 = false) {
            SOCKET new_client{};
            if (use_ipv6) {
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
                    address.setAddress(ip_addr, ntohs(ipv4.sin_port), false);
                }
            }
            return new_client;
        }

        int send(Net::Handle socket, const Net::NetDatas& datas) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::send(socket, datas.data(), datas.size(), 0);
#else
            return ::send(socket, datas.data(), datas.size(), MSG_NOSIGNAL);
#endif
        }

        int send(Net::Handle socket, const std::string& datas) {
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::send(socket, datas.data(), datas.size(), 0);
#else
            return ::send(socket, datas.data(), datas.size(), MSG_NOSIGNAL);
#endif
        }

        int sendto(Net::Handle socket, const Net::NetDatas& datas, const Net::Address& dest, int flag = 0) {
            return ::sendto(socket, datas.data(), datas.size(), flag,
                            static_cast<sockaddr*>(dest.address()),
                            dest.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
        }

        int sendto(Net::Handle socket, const std::string& datas, const Net::Address& dest, int flag = 0) {
            return ::sendto(socket, datas.data(), datas.size(), flag,
                            static_cast<sockaddr*>(dest.address()),
                            dest.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
        }

        int recv(Net::Handle socket, Net::NetDatas& datas, size_t max_len) {
            if (max_len > 0) datas.resize(max_len + 1);
#ifdef TINY_CPP_MY_OS_WINDOWS
            return ::recv(socket, &datas[0], datas.size(), 0);
#else
            return ::recv(socket, &datas[0], datas.size(), MSG_NOSIGNAL);
#endif
        }

        int recv(Net::Handle socket, std::string& datas, size_t max_len) {
            std::string temp(max_len > 0 ? max_len + 1 : datas.size(), '\0');
#ifdef TINY_CPP_MY_OS_WINDOWS
            int ret = ::recv(socket, &temp[0], temp.size(), 0);
#else
            int ret = ::recv(socket, &temp[0], temp.size(), MSG_NOSIGNAL);
#endif
            if (ret > 0) {
                datas.resize(ret);
                datas.assign(&temp[0], &temp[ret]);
            }
            return ret;
        }

        int recvfrom(Net::Handle socket, Net::NetDatas& datas, const Net::Address& src, int flag = 0) {
            SOCK_LEN_T sz = src.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
            return ::recvfrom(socket, &datas[0], datas.size(), flag,
                            static_cast<sockaddr*>(src.address()), &sz);
        }

        int recvfrom(Net::Handle socket, std::string& datas, const Net::Address& src, int flag = 0) {
            SOCK_LEN_T sz = src.isIPv6() ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
            
            return ::recvfrom(socket, &datas[0], datas.size(), flag,
                            static_cast<sockaddr*>(src.address()), &sz);
        }

        int getsockname(Net::Handle socket, Net::Address& addr, bool use_ipv6) {
            SOCK_LEN_T addr_size = use_ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
            return ::getsockname(socket, reinterpret_cast<sockaddr*>(addr.address()), &addr_size);
        }

        int getpeername(Net::Handle socket, Net::Address& addr, bool use_ipv6) {
            SOCK_LEN_T addr_size = use_ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
            return ::getpeername(socket, reinterpret_cast<sockaddr*>(addr.address()), &addr_size);
        }

        bool setUnblockEnabled(Net::Handle socket, uint32_t, const Net::OptionValue& value) {
            if (value.type == Net::OptionValue::Int) {
#ifdef TINY_CPP_MY_OS_WINDOWS
                auto val = static_cast<u_long>(value.var.i);
                return ioctlsocket(socket, FIONBIO, &val) == NO_ERROR;
#else
                bool enabled = value.var.i != 0;
                auto ret = fcntl(socket, F_GETFL, 0);
                if (ret == -1) return false;
                if (enabled) ret |= O_NONBLOCK;
                else ret &= ~O_NONBLOCK;
                return fcntl(socket, F_SETFL, ret) != -1;
#endif
            }
            return false;
        }
    }


    static std::unordered_map<uint32_t, SocketSetting> __SocketSettingsMap__{
            {
                static_cast<uint32_t>(Net::SocketOption::AllowedBroadcast),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::DontRoute),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::KeepAlive),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::NoDelay),
                {
                    &Socket_Impl::setTCPLevelOption,
                    &Socket_Impl::getTCPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MaxSegmentSize),
                {
                    &Socket_Impl::setTCPLevelOption,
                    &Socket_Impl::getTCPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::KeepIdle),
                {
                    &Socket_Impl::setTCPLevelOption,
                    &Socket_Impl::getTCPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::KeepInterval),
                {
                    &Socket_Impl::setTCPLevelOption,
                    &Socket_Impl::getTCPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::KeepCount),
                {
                    &Socket_Impl::setTCPLevelOption,
                    &Socket_Impl::getTCPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::SendBufSize),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::RecvBufSize),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::SendBufTimeout),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::RecvBufTimeout),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::Linger),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::ReuseAddr),
                {
                    &Socket_Impl::setSocketLevelOption,
                    &Socket_Impl::getSocketLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::TTL),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::TOS),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MulticastTTL),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MulticastLoopback),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::AddMembership),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::RemoveMembership),
                {
                    &Socket_Impl::setIPLevelOption,
                    &Socket_Impl::getIPLevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MapIPv6Only),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MulticastLoopbackIPv6),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::JoinGroup),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::LeaveGroup),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::UnicastHops),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::MuticastHops),
                {
                    &Socket_Impl::setIPv6LevelOption,
                    &Socket_Impl::getIPv6LevelOption
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::NonBlocking),
                {
                    &Socket_Impl::setUnblockEnabled,
                    {}
                }
            },
            {
                static_cast<uint32_t>(Net::SocketOption::NativeSocketError),
                {
                    {},
                    &Socket_Impl::getSocketLevelOption
                }
            }
    };

    bool Net::Advanced::setSocketOption(SocketOption option,
                                        const Setter &setter, const Getter &getter) {
        auto id = static_cast<uint32_t>(option);
        if (__SocketSettingsMap__.find(id) != __SocketSettingsMap__.end()) {
            auto& socket_settings = __SocketSettingsMap__.at(id);
            if (setter) socket_settings.setter = setter;
            if (getter) socket_settings.getter = getter;
            return true;
        }
        return false;
    }

    void Net::Advanced::setCustomSocketOption(uint32_t option_id,
                                              const Setter &setter,
                                              const Getter &getter) {
        if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
            auto& socket_settings = __SocketSettingsMap__.at(option_id);
            if (setter) socket_settings.setter = setter;
            if (getter) socket_settings.getter = getter;
        } else {
            SocketSetting socket_setting = {setter, getter};
            __SocketSettingsMap__.emplace(option_id, std::move(socket_setting));
        }
    }

    bool Net::Advanced::removeCustomSocketOption(uint32_t option_id) {
        if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
            __SocketSettingsMap__.erase(option_id);
            return true;
        }
        return false;
    }

    bool Net::Advanced::getValueFromCustomOption(uint32_t option_id, OptionValue &value, Handle socket) {
        if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
            auto& setting = __SocketSettingsMap__.at(option_id);
            if (!setting.getter) return false;
            return setting.getter(socket, option_id, value);
        }
        return false;
    }

    bool Net::Advanced::setValueFromCustomOption(uint32_t option_id, const OptionValue &value, Handle socket) {
        if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
            auto& setting = __SocketSettingsMap__.at(option_id);
            if (!setting.setter) return false;
            return setting.setter(socket, option_id, value);
        }
        return false;
    }

    void Net::Address::Deleter::operator()(void *ptr) const {
        if (!_self) return;
        if (_self->isIPv6()) {
            delete static_cast<sockaddr_in6*>(ptr);
        } else {
            delete static_cast<sockaddr_in*>(ptr);
        }
        ptr = nullptr;
    }

    Net::Address::Address(const char *address, uint16_t port, bool use_ipv6) {
        validate(address, port, use_ipv6);
    }

    Net::Address::Address(const char *address, PortProtocol protocol_num, bool use_ipv6) {
        validate(address, static_cast<uint16_t>(protocol_num), use_ipv6);
    }

    Net::Address::~Address() = default;

    std::string Net::Address::toString(bool* ok) const {
        if (!_valid || !_addr_ptr) return {};
        char result[64] = {};
        bool is_ok{};

        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr_ptr.get());
            is_ok = inet_ntop(AF_INET6, &ipv6->sin6_addr, result, 64) != nullptr;;
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr_ptr.get());
            is_ok = inet_ntop(AF_INET, &ipv4->sin_addr, result, 64) != nullptr;
        }
        if (ok) *ok = is_ok;
        return result;
    }


    void* Net::Address::address() const {
        return _addr_ptr.get();
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

    void Net::Address::setAddress(const char *address, PortProtocol port, bool use_ipv6) {
        validate(address, static_cast<uint16_t>(port), use_ipv6);
    }

    void Net::Address::setPort(uint16_t port) {
        if (!_valid) return;
        _port = port;
        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr_ptr.get());
            ipv6->sin6_port = htons(port);
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr_ptr.get());
            ipv4->sin_port = htons(port);
        }
    }

    void Net::Address::setPort(PortProtocol protocol_num) {
        if (!_valid) return;
        _port = static_cast<uint16_t>(protocol_num);
        if (_use_ipv6) {
            auto ipv6 = static_cast<sockaddr_in6*>(_addr_ptr.get());
            ipv6->sin6_port = htons(_port);
        } else {
            auto ipv4 = static_cast<sockaddr_in*>(_addr_ptr.get());
            ipv4->sin_port = htons(_port);
        }
    }

    Net::Address Net::Address::localHost() {
        return {"127.0.0.1", 0};
    }

    Net::Address Net::Address::localHostIPv6() {
        return {"::1", 0, true};
    }

    std::vector<Net::Address> Net::Address::parseFromHostname(const char *hostname, bool *ok, int *err_cnt) {
        addrinfo hints = {}, *res;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int ecnt = 0;
        auto is_ok = getaddrinfo(hostname, nullptr, &hints, &res) == 0;
        if (!is_ok) {
            if (ok) *ok = false;
            if (err_cnt) *err_cnt = ecnt;
            return {};
        }
        std::vector<Address> result;
        for (addrinfo *ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
            void* addr{};
            char ip_str[128] = {};
            size_t addr_len{sizeof(ip_str)};
            uint16_t port{};
            if (ptr->ai_family == AF_INET) {
                auto ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
                addr = &ipv4->sin_addr;
                port = ipv4->sin_port;
            } else if (ptr->ai_family == AF_INET6) {
                auto ipv6 = reinterpret_cast<sockaddr_in6*>(ptr->ai_addr);
                addr = &ipv6->sin6_addr;
                port = ipv6->sin6_port;
            }
            const bool SUCCESS = inet_ntop(ptr->ai_family, addr, ip_str, addr_len) != nullptr;
            if (SUCCESS) {
                result.emplace_back(ip_str, ntohs(port), ptr->ai_family == AF_INET6);
            } else {
                ecnt += 1;
            }
        }
        freeaddrinfo(res);
        if (ok) *ok = (ecnt == 0);
        if (err_cnt) *err_cnt = ecnt;
        return result;
    }

    Net::Address Net::Address::parseFirstHostname(const char *hostname, bool *ok) {
        addrinfo hints = {}, *res;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        auto is_ok = getaddrinfo(hostname, nullptr, &hints, &res) == 0;
        if (!is_ok) {
            if (ok) *ok = false;
            return {};
        }
        Address result{};
        void* addr{};
        char ip_str[128] = {};
        size_t addr_len{sizeof(ip_str)};
        uint16_t port{};
        if (res->ai_family == AF_INET) {
            auto ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
            addr = &ipv4->sin_addr;
            port = ipv4->sin_port;
        } else if (res->ai_family == AF_INET6) {
            auto ipv6 = reinterpret_cast<sockaddr_in6*>(res->ai_addr);
            addr = &ipv6->sin6_addr;
            port = ipv6->sin6_port;
        }
        const bool SUCCESS = inet_ntop(res->ai_family, addr, ip_str, addr_len) != nullptr;
        if (SUCCESS) {
            result.setAddress(ip_str, ntohs(port), res->ai_family == AF_INET6);
        }
        freeaddrinfo(res);
        if (ok) *ok = SUCCESS;
        return result;
    }

    Net::Address::Address(Address && addr) noexcept {
        _addr_ptr = std::move(addr._addr_ptr);
        _port = addr._port;
        addr._port = 0;
        _use_ipv6 = addr._use_ipv6;
        addr._use_ipv6 = false;
        _valid = addr._valid;
        addr._valid = false;
    }

    Net::Address & Net::Address::operator=(Address && addr) noexcept {
        _addr_ptr = std::move(addr._addr_ptr);
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
        _addr_ptr.reset();
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
            _addr_ptr = std::move(std::unique_ptr<void, Deleter>(new_addr, _deleter));
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
            _addr_ptr = std::move(std::unique_ptr<void, Deleter>(new_addr, _deleter));
        }
        _port = port;
        _use_ipv6 = use_ipv6;
    }

    const char *Net::getSocketErrorName(SocketError err) {
        if (__SocketErrorStrings__.find(err) != __SocketErrorStrings__.end()) {
            return __SocketErrorStrings__.at(err);
        }
        return __SocketErrorStrings__.at(SocketError::UnknownError);
    }

    Net::Socket::Socket(SocketType type, uint8_t msg_type, uint8_t proto_no)
        : _handle(INVALID_SOCKET_VAL), _type(type), _err(), _state(), _msg_type(msg_type), _proto_no(proto_no) {}

    Net::Socket::Socket(Socket &&other) noexcept {
        _handle = other._handle;
        _local_addr = std::move(other._local_addr);
        _peer_addr = std::move(other._peer_addr);
        other._handle = INVALID_SOCKET_VAL;
        _err = other._err;
        _state = other._state;
        _type = other._type;
        _msg_type = other._msg_type;
        _proto_no = other._proto_no;
    }

    Net::Socket &Net::Socket::operator=(Socket &&other) noexcept {
        _handle = other._handle;
        _local_addr = std::move(other._local_addr);
        _peer_addr = std::move(other._peer_addr);
        other._handle = INVALID_SOCKET_VAL;
        _err = other._err;
        _state = other._state;
        _type = other._type;
        _msg_type = other._msg_type;
        _proto_no = other._proto_no;
        return *this;
    }

    Net::Socket::~Socket() {
        if (_handle != INVALID_SOCKET_VAL) close();
    }

    void Net::Socket::setLocalAddress(const char *address, uint16_t port, bool use_ipv6) {
        if (_state != SocketState::Unused) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _local_addr.setAddress(address, port, use_ipv6);
        _err = _local_addr.isValid() ? SocketError::Success : SocketError::InvalidParameter;
        _sys_errno = 0;
    }

    void Net::Socket::setLocalAddress(Address &&address) {
        if (_state != SocketState::Unused) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _local_addr = std::move(address);
        _err = _local_addr.isValid() ? SocketError::Success : SocketError::InvalidParameter;
        _sys_errno = 0;
    }

    void Net::Socket::setLocalPort(uint16_t port) {
        if (_state != SocketState::Unused) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _local_addr.setPort(port);
        _err = SocketError::Success;
        _sys_errno = 0;
    }

    void Net::Socket::setPeerAddress(const char *address, uint16_t port, bool use_ipv6) {
        if (_state != SocketState::Unused && _state != SocketState::Bound) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _peer_addr.setAddress(address, port, use_ipv6);
        _err = _peer_addr.isValid() ? SocketError::Success : SocketError::InvalidParameter;
        _sys_errno = 0;
    }

    void Net::Socket::setPeerAddress(Address &&address) {
        if (_state != SocketState::Unused && _state != SocketState::Bound) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _peer_addr = std::move(address);
        _err = _peer_addr.isValid() ? SocketError::Success : SocketError::InvalidParameter;
        _sys_errno = 0;
    }

    void Net::Socket::setPeerPort(uint16_t port) {
        if (_state != SocketState::Unused && _state != SocketState::Bound) {
            _sys_errno = 0;
            _err = SocketError::SocketInUse;
            return;
        }
        _peer_addr.setPort(port);
        _err = SocketError::Success;
        _sys_errno = 0;
    }

    void Net::Socket::setSocketType(SocketType type) {
        if (_state == SocketState::Unused) {
            _type = type;
            copeSuccess();
        } else {
            _err = SocketError::SocketInUse;
            _sys_errno = 0;
        }
    }

    void Net::Socket::setCustomSocketType(uint8_t type, uint8_t proto_no) {
        _msg_type = type;
        _proto_no = proto_no;
    }

    bool Net::Socket::connect(const char *address, uint16_t port) {
        _peer_addr.setAddress(address, port);
        return connect();
    }

    bool Net::Socket::connect(const char *address, PortProtocol port) {
        _peer_addr.setAddress(address, port);
        return connect();
    }

    bool Net::Socket::connect(Address &&address) {
        _peer_addr = std::move(address);
        return connect();
    }

    bool Net::Socket::connect() {
        if (!_peer_addr.isValid()) {
            _err = SocketError::InvalidParameter;
            _sys_errno = 0;
            return false;
        }
        _state = SocketState::Connecting;
        _handle = Socket_Impl::create(_peer_addr, _type, _msg_type, _proto_no);
        if (_handle == INVALID_SOCKET_VAL) {
            copeFailed();
            _state = SocketState::Unused;
            return false;
        }
        if (!setAllOptions()) {
            copeFailed();
            return false;
        }

        auto ret = Socket_Impl::connect(_handle, _peer_addr);
        if (ret == SOCKET_ERROR) {
            copeFailed();
            if (_err == SocketError::ConnectionInProgress || _err == SocketError::ResourceUnavailable) {
                _state = SocketState::Connecting;
                fd_set e_sets, w_sets;
                FD_ZERO(&e_sets);
                FD_ZERO(&w_sets);
                FD_SET(_handle, &e_sets);
                FD_SET(_handle, &w_sets);
                timeval timeout = {5, 0};
#ifdef TINY_CPP_MY_OS_WINDOWS
                int ncnt = 0;
#else
                int ncnt = static_cast<int>(_handle) + 1;
#endif
                auto err = select(ncnt, nullptr, &w_sets, &e_sets, &timeout);
                if (err <= 0) {
                    Socket_Impl::close(_handle);
                    _state = SocketState::Unused;
                    _handle = INVALID_SOCKET_VAL;
                    if (err == 0) _err = SocketError::ConnectionTimeout;
                    return false;
                }
                OptionValue val;
                auto ok = Socket_Impl::getSocketLevelOption(_handle, static_cast<uint32_t>(SocketOption::NativeSocketError), val);
                if (!ok || val.type != OptionValue::Int || val.var.i != 0) {
                    _err = __SocketErrorsMap__.at(val.var.i);
                    Socket_Impl::close(_handle);
                    _state = SocketState::Unused;
                    _handle = INVALID_SOCKET_VAL;
                    return false;
                }
            } else {
                Socket_Impl::close(_handle);
                _state = SocketState::Unused;
                _handle = INVALID_SOCKET_VAL;
                return false;
            }
        }
        _state = SocketState::Connected;
        copeSuccess();
        return true;
    }

    bool Net::Socket::bind() {
        if (!_local_addr.isValid()) {
            _err = SocketError::InvalidParameter;
            _sys_errno = 0;
            return false;
        }
        _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
        if (_handle == INVALID_SOCKET_VAL) {
            copeFailed();
            return false;
        }
        if (!setAllOptions()) {
            copeFailed();
            return false;
        }
        auto ret = Socket_Impl::bind(_handle, _local_addr);
        if (ret == SOCKET_ERROR) {
            copeFailed();
            Socket_Impl::close(_handle);
            _handle = INVALID_SOCKET_VAL;
            return false;
        }
        _state = SocketState::Bound;
        copeSuccess();
        return true;
    }

    bool Net::Socket::bind(const char *address, uint16_t port) {
        _local_addr.setAddress(address, port);
        return bind();
    }

    bool Net::Socket::bind(const char *address, PortProtocol port) {
        _local_addr.setAddress(address, port);
        return bind();
    }

    bool Net::Socket::bind(Address &&address) {
        _local_addr = std::move(address);
        return bind();
    }

    bool Net::Socket::listen(uint16_t port, int max_connection_count) {
        _local_addr.setAddress("0.0.0.0", port);
        return listen(max_connection_count);
    }

    bool Net::Socket::listen(PortProtocol protocol_num, int max_connection_count) {
        auto port = static_cast<uint16_t>(protocol_num);
        _local_addr.setAddress("0.0.0.0", port);
        return listen(max_connection_count);
    }

    bool Net::Socket::listen(int max_connection_count) {
        if (!_local_addr.isValid()) {
            _err = SocketError::InvalidParameter;
            _sys_errno = 0;
            return false;
        }
        _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
        if (_handle == INVALID_SOCKET_VAL) {
            copeFailed();
            return false;
        }
        if (!setAllOptions()) {
            copeFailed();
            return false;
        }
        _state = SocketState::Listening;
        auto ret = Socket_Impl::bind(_handle, _local_addr);
        if (ret == SOCKET_ERROR) {
            goto ListenFailed;
        }
        ret = Socket_Impl::listen(_handle, max_connection_count);
        if (ret == SOCKET_ERROR) {
            goto ListenFailed;
        }
        copeSuccess();
        return true;
ListenFailed:
        copeFailed();
        Socket_Impl::close(_handle);
        _state = SocketState::Unused;
        _handle = INVALID_SOCKET_VAL;
        return false;
    }

    bool Net::Socket::accept(Socket &socket) {
        bool ok;
        socket = accept(&ok);
        return ok;
    }

    Net::Socket Net::Socket::accept(bool *ok) {
        if (!_local_addr.isValid() || _handle == INVALID_SOCKET_VAL) {
            _err = SocketError::InvalidParameter;
            _sys_errno = 0;
            if (ok) *ok = false;
            return {};
        }
        Address new_address;
        auto handle = Socket_Impl::accept(_handle, new_address, _local_addr.isIPv6());
        if (handle == INVALID_SOCKET_VAL) {
            copeFailed();
            if (ok) *ok = false;
            return {};
        }
        Socket cli_socket(_type);
        cli_socket._handle = handle;
        cli_socket._local_addr = std::move(new_address);
        cli_socket._state = SocketState::Connected;
        if (ok) *ok = true;
        copeSuccess();
        return cli_socket;
    }


    bool Net::Socket::close() {
        _state = SocketState::Closing;
        bool ok = Socket_Impl::close(_handle) == 0;
        _handle = INVALID_SOCKET_VAL;
        if (ok) copeSuccess(); else copeFailed();
        _state = SocketState::Unused;
        return ok;
    }

    bool Net::Socket::shutdown() {
        _state = SocketState::Closing;
        bool ok = Socket_Impl::shutdown(_handle) == 0;
        if (ok) copeSuccess(); else copeFailed();
        _state = SocketState::Shutdown;
        return ok;
    }

    bool Net::Socket::send(const std::string &message, int *sended_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _sys_errno = 0;
            _err = SocketError::SocketIsNotOpened;
            return false;
        }
        auto ok = Socket_Impl::send(_handle, message);
        if (ok < 0) {
            copeFailed();
            return false;
        }
        copeSuccess();
        if (sended_length) *sended_length = ok;
        return true;
    }

    bool Net::Socket::send(const NetDatas &data, int *sended_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _sys_errno = 0;
            _err = SocketError::SocketIsNotOpened;
            return false;
        }
        auto ok = Socket_Impl::send(_handle, data);
        if (ok < 0) {
            copeFailed();
            return false;
        }
        copeSuccess();
        if (sended_length) *sended_length = ok;
        return true;
    }

    bool Net::Socket::recv(NetDatas &data, size_t max_length, int* received_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _sys_errno = 0;
            _err = SocketError::SocketIsNotOpened;
            return false;
        }
        auto ok = Socket_Impl::recv(_handle, data, max_length);
        if (ok < 0) {
            copeFailed();
            return false;
        }
        copeSuccess();
        if (received_length) *received_length = ok;
        return true;
    }

    bool Net::Socket::recv(std::string &message, size_t max_length, int* received_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _sys_errno = 0;
            _err = SocketError::SocketIsNotOpened;
            return false;
        }
        auto ok = Socket_Impl::recv(_handle, message, max_length);
        if (ok < 0) {
            copeFailed();
            return false;
        }
        copeSuccess();
        if (received_length) *received_length = ok;
        return true;
    }

    bool Net::Socket::sendTo(const std::string &message, const Address &address, int *sended_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
            if (_handle == INVALID_SOCKET_VAL) {
                copeFailed();
                return false;
            }

            if (!setAllOptions()) {
                copeFailed();
                return false;
            }
        }
        auto len = Socket_Impl::sendto(_handle, message, address);
        if (len < 0) {
            copeFailed();
            return false;
        }
        if (sended_length) *sended_length = len;
        copeSuccess();
        return true;
    }

    bool Net::Socket::sendTo(const NetDatas &message, const Address &address, int *sended_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
            if (_handle == INVALID_SOCKET_VAL) {
                copeFailed();
                return false;
            }
            if (!setAllOptions()) {
                copeFailed();
                return false;
            }
        }
        auto len = Socket_Impl::sendto(_handle, message, address);
        if (len < 0) {
            copeFailed();
            return false;
        }
        if (sended_length) *sended_length = len;
        copeSuccess();
        return true;
    }

    bool Net::Socket::recvFrom(std::string &message, size_t max_length, const Address &address, int *received_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
            if (_handle == INVALID_SOCKET_VAL) {
                copeFailed();
                return false;
            }
            if (!setAllOptions()) {
                copeFailed();
                return false;
            }
        }
        if (max_length > 0) message.resize(max_length);
        auto len = Socket_Impl::recvfrom(_handle, message, address);
        if (len < 0) {
            copeFailed();
            return false;
        }
        if (received_length) *received_length = len;
        copeSuccess();
        return true;
    }

    bool Net::Socket::recvFrom(NetDatas &data, size_t max_length, const Address &address, int *received_length) {
        if (_handle == INVALID_SOCKET_VAL) {
            _handle = Socket_Impl::create(_local_addr, _type, _msg_type, _proto_no);
            if (_handle == INVALID_SOCKET_VAL) {
                copeFailed();
                return false;
            }
            if (!setAllOptions()) {
                copeFailed();
                return false;
            }
        }
        if (max_length > 0) data.resize(max_length);
        auto len = Socket_Impl::recvfrom(_handle, data, address);
        if (len < 0) {
            copeFailed();
            return false;
        }
        if (received_length) *received_length = len;
        copeSuccess();
        return true;
    }

    void Net::Socket::setOption(SocketOption option, OptionValue value, bool *ok) {
        setOption(static_cast<uint32_t>(option), value, ok);
    }

    void Net::Socket::setOption(uint32_t option_id, OptionValue value, bool *ok) {
        if (_options.find(option_id) != _options.end()) {
            _options.at(option_id) = value;
        } else {
            _options.emplace(option_id, value);
        }
        bool is_ok = true;
        if (_handle != INVALID_SOCKET_VAL) {
            if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
                auto& setting = __SocketSettingsMap__.at(option_id);
                is_ok = (setting.setter && setting.setter(_handle, option_id, value));
            } else {
                _err_opt_id = option_id;
                _err = SocketError::SetOptionError;
                is_ok = false;
            }
        }
        if (ok) *ok = is_ok;
    }

    const Net::Address & Net::Socket::localAddress() const {
        return _local_addr;
    }

    const Net::Address & Net::Socket::peerAddress() const {
        return _peer_addr;
    }

    Net::OptionValue Net::Socket::option(SocketOption option, bool *ok) const {
        return this->option(static_cast<uint32_t>(option), ok);
    }

    Net::OptionValue Net::Socket::option(uint32_t option_id, bool *ok) const {
        if (__SocketSettingsMap__.find(option_id) != __SocketSettingsMap__.end()) {
            auto& setting = __SocketSettingsMap__.at(option_id);
            OptionValue ret;
            if (setting.getter && !setting.getter(_handle, option_id, ret)) {
                if (ok) *ok = false;
                return {};
            }
            if (ok) *ok = true;
            return ret;
        }

        if (ok) *ok = false;
        return {};
    }

    Net::SocketError Net::Socket::lastError() const {
        return _err;
    }

    Net::SocketType Net::Socket::type() const {
        return _type;
    }

    Net::SocketState Net::Socket::state() const {
        return _state;
    }

    uint32_t Net::Socket::errorSocketOptionID() const {
        return _err_opt_id;
    }

    int Net::Socket::nativeErrorNo() const {
        return _sys_errno;
    }

    void Net::Socket::mapErrorNum(int error_code) {
        if (__SocketErrorsMap__.find(error_code) != __SocketErrorsMap__.end()) {
            _err = __SocketErrorsMap__.at(error_code);
        } else {
            _err = SocketError::UnknownError;
        }
    }

    void Net::Socket::copeFailed() {
        _sys_errno = getLastSystemError();
        mapErrorNum(_sys_errno);
        if (_sys_errno == 0) _err = SocketError::SetOptionError;
    }

    void Net::Socket::copeSuccess() {
        _sys_errno = 0;
        mapErrorNum(_sys_errno);
    }

    bool Net::Socket::setAllOptions() {
        for (auto& opt : _options) {
            try {
                auto& setting = __SocketSettingsMap__.at(opt.first);
                if (setting.setter && !setting.setter(_handle, opt.first, opt.second)) {
                    _err_opt_id = opt.first;
                    return false;
                }
            } catch (const std::exception&) {
                _err_opt_id = opt.first;
                return false;
            }
        }
        _err_opt_id = 0;
        return true;
    }

    int Net::getLastSystemError(std::string *info) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        int err = WSAGetLastError();
#else
        int err = errno;
#endif
        if (info) *info = getSystemErrorByErrno(err);
        return err;
    }

    std::string Net::getSystemErrorByErrno(int err) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (err == 0) {
            return "No errors found!";
        }
        void* msg_buf = nullptr;
        auto out_size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&msg_buf),
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
        std::ostringstream oss;
        oss << "Unknown error! code: 0x" << std::hex << err;
        return oss.str();
#else
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
