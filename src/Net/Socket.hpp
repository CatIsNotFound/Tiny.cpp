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

#ifndef TINY_CPP_NET_SOCKET_HPP
#define TINY_CPP_NET_SOCKET_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <memory>

/*******************************************************************************************
 * P.S: - To enabled using Windows32 lib, please use the macro below:                      *
 *      #define TINY_CPP_MY_OS_WINDOWS                                                     *
 *      - To enabled using Unix lib (for Linux, MacOS, etc.), please use the macro below:  *
 *      #define TINY_CPP_MY_OS_UNIX                                                        *
 *******************************************************************************************/

#if !defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_MY_OS_UNIX)
    #if defined(_WIN32) || defined(_WIN64)
        #define TINY_CPP_MY_OS_WINDOWS
    #elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
        #define TINY_CPP_MY_OS_UNIX
    #endif
#endif

#if defined(__clang__) || defined(__GNUC__)
#   define API_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define API_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   define API_DEPRECATED(msg)
#endif

namespace Tiny {
    namespace Net {
#ifdef TINY_CPP_MY_OS_WINDOWS
        using Handle = uint64_t;
#elif defined(TINY_CPP_MY_OS_UNIX)
        using Handle = int;
#endif
        using NetDatas = std::vector<char>;

        /// List of common protocol ports
        enum class PortProtocol : uint16_t {
            HTTP  = 80,
            HTTPS = 443,
            SOCKS = 1080,

            DHCP_Server  = 67,
            DHCP_Client  = 68,
            NTP          = 123,
            SNMP_UDP     = 161,
            SNMP_Trap    = 162,
            BGP          = 179,

            SSH    = 22,
            Telnet = 23,
            RDP    = 3389,
            VNC    = 5900,

            FTP_Con   = 21,
            FTP_Data  = 20,
            FTPS_Con  = 990,
            FTPS_Data = 989,
            SFTP      = 22,
            TFTP      = 69,
            NFS       = 2049,
            SMB       = 445,

            SMTP      = 25,
            SMTP_TLS  = 587,
            POP3      = 110,
            POP3S     = 995,
            IMAP      = 143,
            IMAPS     = 993,

            DNS       = 53,
            DoT       = 853,    // DNS over TLS
            DoH       = 443,    // DNS over HTTPS
            DoQ       = 853,    // DNS over QUIC

            RTMP      = 1935,
            RTSP      = 554,
            SIP       = 5060,
            SIPS      = 5061,

            MySQL         = 3306,
            PostgreSQL    = 5432,
            MongoDB       = 27017,
            Redis         = 6379,
            MS_SQL        = 1433,  // Microsoft SQL Server
            Oracle        = 1521,
            Elasticsearch = 9200,

            MQTT    = 1883,
            MQTTS   = 8883,
            AMQP    = 5672,
            Kafka   = 9092
        };

        class Address {
        public:
            Address() = default;
            Address(const char* address, uint16_t port, bool use_ipv6 = false);
            Address(const char* address, PortProtocol protocol_num, bool use_ipv6 = false);
            ~Address();

            std::string toString(bool* ok = nullptr) const;

            void *address() const;
            uint16_t port() const;
            bool isValid() const;
            bool isIPv6() const;

            void setAddress(const char* address, uint16_t port, bool use_ipv6 = false);
            void setAddress(const char* address, PortProtocol port, bool use_ipv6 = false);
            void setPort(uint16_t port);
            void setPort(PortProtocol protocol_num);

            static Address localHost();
            static Address localHostIPv6();
            static std::vector<Address> parseFromHostname(const char *hostname, bool *ok = nullptr, int *err_cnt = nullptr);
            static Address parseFirstHostname(const char* hostname, bool *ok = nullptr);

            Address(const Address&) = delete;
            Address& operator=(const Address&) = delete;
            Address(Address&& addr) noexcept;
            Address& operator=(Address&& addr) noexcept;
            bool operator==(const Address& addr) const;
            bool operator!=(const Address& addr) const;
        private:
            void validate(const char* full_address, uint16_t port, bool use_ipv6 = false);
            struct Deleter {
                Address* _self;
                void operator()(void* ptr) const;
            } _deleter{this};
            std::unique_ptr<void, Deleter> _addr_ptr{nullptr, _deleter};
            uint16_t _port{UINT16_MAX};
            bool _valid{};
            bool _use_ipv6{};
        };

        enum class SocketError : uint8_t {
            Success,
            /// Invalid IP address or protocol.
            InvalidParameter,
            SetOptionError,
            /// The specified protocol is not supported, or the socket type does not match the protocol.
            ProtoNotSupported,
            SocketIsNotOpened,
            SocketClosed,
            SocketInUse,
            /// Requested address or port is in used.
            AddressInUse,
            AddressNotAvailable,
            /// Requested address is not supported!
            /// (If host is not found or is not UP status.)
            AddressNotSupport,

            ConnectionRefused,
            ConnectionInProgress,
            ConnectionInUsed,
            ConnectionReset,
            ConnectionAborted,
            ConnectionTimeout,
            NetworkUnreachable,
            HostUnreachable,
            /// Network interface card is DOWN status.
            NetworkDown,

            /// While received message (or datas) is too long.
            MessageTooLong,
            /// Resource Temporarily unavailable
            ResourceUnavailable,

            /// Permission denied when trying to create socket or any other things.
            OperationDenied = 249,
            /// Resource limit error caused by the OS.
            SystemResourceLimit,
            OperationInProgress,
            /// Manually canceled by the user while the network is in use.
            OperationCancelled,
            /// e.g: Calling accept on a non-listening socket, or calling listen on a datagram socket.
            OperationNotSupported,
            SegmentationFault,
            UnknownError
        };

        const char* getSocketErrorName(SocketError err);

        enum class SocketType : uint8_t {
            TCP,
            UDP,
            SCTP,
            ICMP,
            ICMPV6,
            Custom
        };

        enum class SocketState : uint8_t {
            Unused,
            ParsingAddress,
            Connecting,
            Connected,
            Bound,
            Listening,
            Closing,
            Shutdown
        };

        enum class SocketOption : uint8_t {
            /// Allowed send/receive broadcast. (for IPv4 address)
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            AllowedBroadcast = 1,
            /// Bypass the outgoing routing table lookup and send directly to the network interface.
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            DontRoute,
            /// Enabled TCP keepalive detection.
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            KeepAlive,
            /// Try to optimize the socket for low latency. (for TCP)
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            NoDelay,
            /// TCP Maximum Segment Size (MSS)
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: Linux, MacOS, Windows 10+
            MaxSegmentSize,
            /// How long the connection is idle before sending a Keepalive probe (seconds).
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: Linux, MacOS, Windows 10+
            KeepIdle,
            /// Keepalive probe sending interval (seconds)
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: Linux, MacOS, Windows 10+
            KeepInterval,
            /// The maximum number of probes sent before deciding the connection is lost.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: Linux, MacOS, Windows 10+
            KeepCount,
            /// Send maximum of buffers size.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            SendBufSize,
            /// Receive maximum of buffers size.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            RecvBufSize,
            /// Set timeout of sending buffer.
            ///
            /// - ValueType:
            ///     - Windows: int (Stored as milliseconds)
            ///     - Others: The pointer of `timeval` (from <unistd.h> header file)
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            SendBufTimeout,
            /// Set timeout of receiving buffer.
            ///
            /// - ValueType:
            ///     - Windows: int (Stored as milliseconds)
            ///     - Others: The pointer of `timeval`
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            RecvBufTimeout,
            /// Decided how to close socket smoothly.
            ///
            /// - ValueType: The pointer of linger.
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            Linger,
            /// Allowed reuse local address.
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            ReuseAddr,
            /// Time To Live of an IP packet.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            TTL,
            /// IP type of service.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            TOS,
            /// TTL of the multicast packet.
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            MulticastTTL,
            /// Allow multicast data to loop back to the local machine.
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            MulticastLoopback,
            /// Add an IP group membership. (Only for IPv4 address)
            ///
            /// - ValueType: The pointer of `ip_mreq`
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            AddMembership,
            /// Remove an IP group membership. (Only for IPv4 address)
            ///
            /// - ValueType: The pointer of `ip_mreq`
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            RemoveMembership,
            /// Disabled map IPv4 address. (Only for mapping IPv6 address)
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            MapIPv6Only,
            /// Allow multicast data to loop back to the local machine. (Only for IPv6)
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            MulticastLoopbackIPv6,
            /// Join the IPv6 multicast group.
            ///
            /// - ValueType: The pointer of `ipv6_mreq`
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            JoinGroup,
            /// Leave the IPv6 multicast group.
            ///
            /// - ValueType: The pointer of `ipv6_mreq`
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            LeaveGroup,
            /// IP unicast hop limit. (Only for IPv6)
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            UnicastHops,
            /// IP multicast hop limit. (Only for IPv6)
            ///
            /// - ValueType: int
            ///
            /// - Supported: Set/Get
            ///
            /// - Supported OS: All
            MuticastHops,
            /// Allows the program to immediately return and continue execution
            /// if the conditions for a certain function are not met.
            ///
            /// - ValueType: int(bool) false/true
            ///
            /// - Supported: Set
            ///
            /// - Supported OS: All
            NonBlocking,
            /// Get current native socket error code
            ///
            /// - ValueType: int
            ///
            /// - Supported: Get
            ///
            /// - Supported OS: All
            NativeSocketError = 255
        };

        struct OptionValue {
            enum ValueType : uint8_t {
                None,
                String,
                Int,
                UInt,
                Float,
                Custom
            } type;
#ifdef TINY_CPP_MY_OS_WINDOWS
            int size;           // Size of the data type. (By sizeof() function.)
#else
            unsigned int size;  // Size of the data type. (By sizeof() function.)
#endif
            union Value {
                int         i;  // Integer value
                uint32_t    u;  // Unsigned integer value
                float       f;  // Floating value
                char*       s;  // String value
                void*       v;  // Custom value (Stored as pointer)
            } var;              // Stored variable

            OptionValue() : type(None), size(0), var() {
                var.v = nullptr;
            }

            OptionValue(int v) : type(Int), size(sizeof(int)), var() {
                var.i = v;
            }

            OptionValue(uint32_t v) : type(UInt), size(sizeof(uint32_t)), var() {
                var.u = v;
            }

            OptionValue(float v) : type(Float), size(sizeof(float)), var() {
                var.f = v;
            }

            OptionValue(const char* v) : type(String), size(v ? strlen(v) : 0), var() {
                var.s = const_cast<char *>(v);
            }

            OptionValue(void* v, uint32_t size) : type(Custom), size(size), var() {
                var.v = v;
            }

            void set(int v) {
                type = Int;
                size = sizeof(int);
                var.i = v;
            }

            void set(uint32_t v) {
                type = UInt;
                size = sizeof(uint32_t);
                var.u = v;
            }

            void set(float v) {
                type = Float;
                size = sizeof(float);
                var.f = v;
            }

            void set(const char* v) {
                type = String;
                size = v ? strlen(v) : 0;
                var.s = const_cast<char *>(v);
            }

            void set(void* v, uint32_t len) {
                type = Custom;
                size = len;
                var.v = v;
            }

            void unset() {
                type = None;
                size = 0;
                var.v = nullptr;
            }

            OptionValue& operator=(int v) {
                set(v);
                return *this;
            }

            OptionValue& operator=(uint32_t v) {
                set(v);
                return *this;
            }

            OptionValue& operator=(float v) {
                set(v);
                return *this;
            }

            OptionValue& operator=(const char* v) {
                set(v);
                return *this;
            }

            OptionValue& operator=(std::nullptr_t) {
                unset();
                return *this;
            }

            OptionValue& operator=(const OptionValue& other) {
                this->type = other.type;
                this->size = other.size;
                switch (type) {
                    case None:
                        this->var.v = nullptr;
                        break;
                    case String:
                        this->var.s = const_cast<char *>(other.var.s);
                        break;
                    case Int:
                        this->var.i = other.var.i;
                        break;
                    case UInt:
                        this->var.u = other.var.u;
                        break;
                    case Float:
                        this->var.f = other.var.f;
                        break;
                    case Custom:
                        this->var.v = other.var.v;
                        break;
                }
                return *this;
            }

            bool operator==(const OptionValue& other) const {
                if (type != other.type) return false;
                if (size != other.size) return false;
                switch (type) {
                    case None:
                        return true;
                    case String:
                        return strcmp(var.s, other.var.s) == 0;
                    case Int:
                        return var.i == other.var.i;
                    case UInt:
                        return var.u == other.var.u;
                    case Float:
                        return var.f == other.var.f;
                    case Custom:
                        return var.v == other.var.v;
                }
                return false;
            }

            bool operator!=(const OptionValue& other) const {
                if (type != other.type) return true;
                if (size != other.size) return true;
                switch (type) {
                    case None:
                        return false;
                    case String:
                        return strcmp(var.s, other.var.s) != 0;
                    case Int:
                        return var.i != other.var.i;
                    case UInt:
                        return var.u != other.var.u;
                    case Float:
                        return var.f != other.var.f;
                    case Custom:
                        return var.v != other.var.v;
                }
                return true;
            }
        };

        class Socket {
        public:
            Socket(SocketType type = SocketType::TCP, uint8_t msg_type = 0, uint8_t proto_no = 0);
            Socket(Socket&& other) noexcept;
            Socket& operator=(Socket&& other) noexcept;
            ~Socket();

            void setLocalAddress(const char* address, uint16_t port, bool use_ipv6 = false);
            void setLocalAddress(Address&& address);
            void setLocalPort(uint16_t port);
            void setPeerAddress(const char* address, uint16_t port, bool use_ipv6 = false);
            void setPeerAddress(Address&& address);
            void setPeerPort(uint16_t port);
            void setSocketType(SocketType type);
            void setCustomSocketType(uint8_t type, uint8_t proto_no);

            bool connect(const char* address, uint16_t port);
            bool connect(const char* address, PortProtocol port);
            bool connect(Address &&address);
            bool connect();
            bool bind(const char* address, uint16_t port);
            bool bind(const char* address, PortProtocol port);
            bool bind(Address &&address);
            bool bind();
            bool listen(uint16_t port, int max_connection_count);
            bool listen(PortProtocol protocol_num, int max_connection_count);
            bool listen(int max_connection_count);

            bool accept(Socket& socket);
            Socket accept(bool* ok = nullptr);
            bool close();
            bool shutdown();

            bool send(const std::string &message, int *sended_length = nullptr);
            bool send(const NetDatas &data, int *sended_length = nullptr);
            bool recv(NetDatas& data, size_t max_length, int* received_length = nullptr);
            bool recv(std::string& message, size_t max_length, int* received_length = nullptr);

            bool sendTo(const std::string &message, const Address& address, int* sended_length = nullptr);
            bool sendTo(const NetDatas &message, const Address& address, int* sended_length = nullptr);
            bool recvFrom(std::string& message, size_t max_length, const Address& address, int* received_length = nullptr);
            bool recvFrom(NetDatas& data, size_t max_length, const Address& address, int* received_length = nullptr);

            void setOption(SocketOption option, OptionValue value, bool *ok = nullptr);
            void setOption(uint32_t option_id, OptionValue value, bool *ok = nullptr);

            const Address& localAddress() const;
            const Address& peerAddress() const;
            OptionValue option(SocketOption option, bool *ok = nullptr) const;
            OptionValue option(uint32_t option_id, bool *ok = nullptr) const;
            SocketError lastError() const;
            SocketType  type() const;
            SocketState state() const;

            uint32_t    errorSocketOptionID() const;
            int         nativeErrorNo() const;

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;
        protected:
            void mapErrorNum(int error_code);
        private:
            void copeFailed();
            void copeSuccess();
            bool setAllOptions();

            std::unordered_map<uint32_t, OptionValue> _options{};
            Address _local_addr, _peer_addr{};
            Handle _handle;
            int _sys_errno{};
            uint32_t _err_opt_id{};
            SocketType  _type;
            SocketError _err;
            SocketState _state;
            uint8_t     _msg_type;
            uint8_t     _proto_no;
        };

        int getLastSystemError(std::string* info = nullptr);
        std::string getSystemErrorByErrno(int err_no);
    }
}

#endif //TINY_CPP_NET_SOCKET_HPP

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