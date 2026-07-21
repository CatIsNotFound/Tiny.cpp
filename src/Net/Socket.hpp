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


namespace Tiny {
    namespace Net {
#ifdef TINY_CPP_MY_OS_WINDOWS
        using Handle = uint64_t;
#elif defined(TINY_CPP_MY_OS_UNIX)
        using Handle = int;
#endif
        using Datas = std::vector<char>;
        enum class Protocol : uint16_t {
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
            Address(const char* address, Protocol protocol_num, bool use_ipv6 = false);
            ~Address();

            std::string toString(bool* ok = nullptr) const;
            void* address() const;
            uint16_t port() const;
            bool isValid() const;
            bool isIPv6() const;

            void setAddress(const char* address, uint16_t port, bool use_ipv6 = false);
            void setAddress(const char* address, Protocol port, bool use_ipv6 = false);
            void setPort(uint16_t port);
            void setPort(Protocol protocol_num);

            static Address localHost();
            static Address localHostIPv6();

            Address(const Address&) = delete;
            Address& operator=(const Address&) = delete;
            Address(Address&& addr) noexcept;
            Address& operator=(Address&& addr) noexcept;
            bool operator==(const Address& addr) const;
            bool operator!=(const Address& addr) const;
        private:
            void validate(const char* full_address, uint16_t port, bool use_ipv6 = false);

            uint16_t _port{UINT16_MAX};
            bool _valid{};
            bool _use_ipv6{};
            void* _addr{};
        };

        class Socket {
        public:
            Socket();
            Socket(Address&& address);
            Socket(Socket&& other) noexcept;
            Socket& operator=(Socket&& other) noexcept;
            ~Socket();

            bool connect(const char* address, uint16_t port, bool use_udp = false);
            bool connect(const char* address, Protocol port, bool use_udp = false);
            bool connect(Address &&address, bool use_udp = false);
            bool connect(bool use_udp = false);
            bool bind(const char* address, uint16_t port, bool use_udp = false);
            bool bind(const char* address, Protocol port, bool use_udp = false);
            bool bind(Address &&address, bool use_udp = false);
            bool bind(bool use_udp = false);
            bool listen(uint16_t port, bool use_udp = false);
            bool listen(Protocol protocol_num, bool use_udp = false);

            bool accept(Socket& socket) const;
            Socket accept(bool* ok = nullptr) const;
            void close();

            bool send(const std::string &message);
            bool send(const Datas &data);
            bool recv(Datas& data, size_t max_length = 0, int* received_length = nullptr);
            bool recv(std::string& message, size_t max_length = 0, int* received_length = nullptr);

            bool setUnblockEnabled(bool enabled);

            bool isNull() const;

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;
        private:
            Address _address;
            Handle _handle;
        };

        std::string getLastError(int* err_code = nullptr);
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