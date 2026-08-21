
#include <csignal>

#include "../src/Tiny.hpp"
#include <algorithm>
#ifdef TINY_CPP_MY_OS_WINDOWS
#include <process.h>
#include <winsock.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <climits>
#endif

using namespace Tiny;
using namespace Net;
using namespace TUI;

#pragma pack(1)
class ICMP_Packet {
public:
    ICMP_Packet() : _type_(), _code_(), _checksum_(), _data_() {}

    ICMP_Packet(uint8_t type, uint8_t code, const void* data = nullptr, size_t size_of_data = 0)
            : _type_(type), _code_(code), _checksum_() {
        if (size_of_data > sizeof(_data_)) size_of_data = sizeof(_data_);
        memcpy(_data_, data, size_of_data);
        calc_checksum(this, 4 + size_of_data);
    }

    void setPack(uint8_t type, uint8_t code, const void* data = nullptr, size_t size_of_data = 0) {
        _type_ = type;
        _code_ = code;
        if (size_of_data > sizeof(_data_)) size_of_data = sizeof(_data_);
        memcpy(_data_, data, size_of_data);
        calc_checksum(this, 4 + size_of_data);
    }

    uint8_t type() const {
        return _type_;
    }

    uint8_t code() const {
        return _code_;
    }

    uint16_t checksum() const {
        return _checksum_;
    }

    const void* data() const {
        return _data_;
    }
private:
    void calc_checksum(const void* data, size_t len) {
        auto buf = static_cast<const uint16_t*>(data);
        uint32_t sum = 0;

        while (len > 1) {
            sum += *buf++;
            len -= 2;
        }
        if (len == 1) {
            sum += *static_cast<const uint8_t *>(data);
        }
        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        _checksum_ = static_cast<uint16_t>(~sum);
    }

    uint8_t _type_;
    uint8_t _code_;
    uint16_t _checksum_;
    char    _data_[32]{};
};
#pragma pack()

class ICMP_Socket {
public:
#pragma pack(1)
    struct EchoRequest {
        uint16_t id;
        uint16_t seq;
        int64_t  times;
        char     data[20];
    };
#pragma pack()
    ICMP_Socket() : _socket(SocketType::Custom) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        const uint32_t TIMEOUT = 5000;
        _socket.setOption(SocketOption::RecvBufTimeout, TIMEOUT);
        _socket.setOption(SocketOption::SendBufTimeout, TIMEOUT);
#else
        timeval _timeval;
        _timeval.tv_sec = 5;
        _timeval.tv_usec = 0;
        _socket.setOption(SocketOption::RecvBufTimeout, OptionValue(&_timeval, sizeof(_timeval)));
        _socket.setOption(SocketOption::SendBufTimeout, OptionValue(&_timeval, sizeof(_timeval)));
#endif
    }

    bool setAddress(Address&& address) {
        if (address.isIPv6()) {
            _socket.setCustomSocketType(3, 58);
            _socket.setLocalAddress("::", 0, true);
        } else {
            _socket.setCustomSocketType(3, 1);
            _socket.setLocalAddress("0.0.0.0", 0);
        }
        if (!_socket.bind()) return false;
        _socket.setPeerAddress(std::move(address));
        return _socket.peerAddress().isValid();
    }

    bool setAddress(const char* address) {
        _socket.setCustomSocketType(3, 1);
        _socket.setLocalAddress("0.0.0.0", 0);
        if (!_socket.bind()) return false;
        _socket.setPeerAddress(address, 0);
        return _socket.peerAddress().isValid();
    }

    const Address& address() const {
        return _socket.peerAddress();
    }

    const char* error() {
        if (_socket.lastError() != SocketError::Success) {
            switch (_socket.lastError()) {
                case SocketError::AddressNotSupport:
                    return "Address is not supported!";
                case SocketError::ProtoNotSupported:
                    return "Protocol is not supported!";
                case SocketError::SocketIsNotOpened:
                    return "Socket is not opened!";
                case SocketError::SocketClosed:
                    return "Socket is already closed!";
                case SocketError::AddressInUse:
                    return "Address is already used!";
                case SocketError::AddressNotAvailable:
                    return "Address is not available!";
                case SocketError::ConnectionRefused:
                    return "Connection refused!";
                case SocketError::ConnectionReset:
                    return "Connection reset!";
                case SocketError::ConnectionAborted:
                    return "Connection aborted!";
                case SocketError::ConnectionTimeout:
                    return "Connection timeout!";
                case SocketError::NetworkUnreachable:
                    return "Network is unreachable!";
                case SocketError::HostUnreachable:
                    return "Host is unreachable!";
                case SocketError::NetworkDown:
                    return "Disconnected network!";
                case SocketError::OperationDenied:
                    return "Operation denied!";
                case SocketError::OperationCancelled:
                    return "Operation cancelled!";
                case SocketError::ResourceUnavailable:
                    return "Resource unavailable!";
                default:
                    return getSocketErrorName(_socket.lastError());
            }
        }
        if (_errco == 4) {
            return "Packet broken!";
        }
        return "Unknown!";
    }

    bool send() {
        EchoRequest echo{};
        echo.id = htons(getpid());
        echo.seq = htons(++_seq);
        echo.times = DT::currentTimestamps();
        char data[20] = {};
        srand(echo.times);
        for (size_t i = 0; i < 19; ++i) {
            data[i] = static_cast<char>(rand() % 26 + 65);
        }
        memcpy(echo.data, data, 20);
        void* ptr = &echo;
        ICMP_Packet packet(_socket.peerAddress().isIPv6() ? 128 : 8, 0, ptr, sizeof(EchoRequest));
        return _socket.sendTo(std::string(reinterpret_cast<char*>(&packet), sizeof(ICMP_Packet)), _socket.peerAddress());
    }

    bool recv(ICMP_Packet* packet = nullptr, EchoRequest* echo = nullptr, uint8_t* TTL = nullptr) {
        std::string data;
        while (true) {
            int recv_count = 0;
            auto ok = _socket.recvFrom(data, 128, _socket.peerAddress(), &recv_count);
            if (recv_count == 0) break;
            if (!ok) return false;
            size_t ihl{};
            if (_socket.peerAddress().isIPv6()) {
                ihl = data.find_first_of(static_cast<char>(0x81));
                if (ihl == std::string::npos) return false;
            } else {
                ihl = (data[0] & 0x0f) * 4;
            }
            auto PACK = reinterpret_cast<const ICMP_Packet*>(data.c_str() + ihl);
            if (TTL) {
                if (!_socket.peerAddress().isIPv6()) {
                    size_t idx = data.find_first_of(0x01);
                    if (idx == std::string::npos) {
                        _errco = 4;
                        continue;
                    }
                    *TTL = data.c_str()[idx - 1];
                }
            }
            if (_socket.peerAddress().isIPv6() && PACK->type() != 129) {
                _errco = 4;
                continue;
            }
            if (!_socket.peerAddress().isIPv6() && PACK->type() != 0) {
                _errco = 4;
                continue;
            }
            if (packet) {
                packet->setPack(PACK->type(), PACK->code(), PACK->data(), sizeof(EchoRequest));
            }
            if (echo) {
                auto ECHO = reinterpret_cast<const EchoRequest*>(PACK->data());
                echo->id = ECHO->id;
                echo->seq = ECHO->seq;
                echo->times = DT::currentTimestamps() - ECHO->times;

                memcpy(echo->data, ECHO->data, sizeof(echo->data));
            }
            return true;
        }
        return false;
    }

    uint32_t sendCount() const {
        return _seq;
    }

    void close() {
        _socket.close();
    }
private:
    Socket _socket;
    uint32_t _seq{};
    uint8_t _errco{};
};

int str2Int(const std::string& str) {
    int ret = 0;
    for (auto& c : str) {
        if (isdigit(c)) ret = ret * 10 + (c - '0');
        else break;
    }
    return ret;
}

std::vector<std::string> split(const std::string& str, size_t per_size) {
    std::vector<std::string> result;
    size_t sz = str.size() / per_size;
    if (str.size() % per_size != 0) sz += 1;
    result.reserve(sz);
    for (size_t i = 0; i < sz; ++i) {
        result.push_back(str.substr(per_size * i, per_size));
    }
    return result;
}

void print_help(CommandParser& cmd_parser) {
    auto scr = Terminal::screenSize();
    Terminal::printFormat("Usage: ping <HOST> [OPTIONS]\r\n\r\nOPTIONS:\r\n");
    for (auto& cmd : cmd_parser) {
        Terminal::printFormat("{:>8c}--{:<8s}, ", ' ', cmd.first);
        int idx = 0;
        std::string short_arg;
        for (auto& opt : cmd.second.short_options) {
            short_arg += Terminal::formatString("-{}", opt);
            if (++idx < cmd.second.short_options.size()) {
                short_arg += " | ";
            }
        }

        Terminal::printFormat("{:<16s} ", short_arg.c_str());
        auto pos = Terminal::cursorPosition();
        size_t dis_size = scr.width - pos.column;
        if (dis_size < cmd.second.description.size()) {
            auto texts = split(cmd.second.description, dis_size);
            for (size_t i = 0; i < texts.size(); ++i) {
                Terminal::printLine(texts[i]);
                if (i != texts.size() - 1) {
                    Terminal::moveCursor(pos.row + i, pos.column);
                }
            }
        } else {
            Terminal::printLine(cmd.second.description);
        }
    }
}

void print_result(const ICMP_Socket &socket, const std::vector<int64_t> &times_list) {
    Terminal::printLine("");
    Terminal::printFormat("=== PING {} Report ===\r\n", socket.address().toString());
    double lost = static_cast<double>(socket.sendCount() - times_list.size()) / static_cast<double>(socket.sendCount()) * 100.0;
    Terminal::printFormat("Total: {} pack(s), Received: {} pack(s) ({:.1f}% LOST)\r\n",
        socket.sendCount(), times_list.size(), lost);
    double avg = 0;
    double min = times_list.empty() ? 0 : static_cast<double>(INT64_MAX), max = 0;
    for (auto& times : times_list) {
        auto ts = static_cast<double>(times);
        avg += ts;
        min = min > ts ? ts : min;
        max = max < ts ? ts : max;
    }
    avg /= socket.sendCount();
    Terminal::printFormat("Times: Avg={:.2f}ms Min={}ms Max={}ms\r\n", avg, min, max);
}

static ICMP_Socket sock;
static std::vector<int64_t> times_seq_list;
static uint32_t seq_count;

void cancel_by_user(int) {
    print_result(sock, times_seq_list);
    sock.close();
    exit(0);
}

int main(int argc, char** argv) {
    CommandParser parser(argc, argv);
    parser.addCommand("count", "c", "The numbers of sending packages",
        true, "0");
    parser.addCommand("host", "i", "The remote host address to send to",
        true, {}, true, true);
    parser.addLastCommand("help", "h?", "Show the command help");
    int err;
    std::vector<std::string> missing;
    auto err_co = parser.exec(nullptr, &err, &missing);
    if (err_co != CommandParser::ParseError::NoError) {
        if (missing.empty()) {
            Terminal::printError("ping: {}! Expected '{}'.\r\n", CommandParser::getParseErrorName(err_co), argv[err]);
            return err;
        }
        Terminal::printError("ping: {}: '{}'.\r\n", CommandParser::getParseErrorName(err_co), missing[0]);
        return err;
    }
    auto exec_list = parser.execCommandList();

    std::string host;
    for (auto& cmd : exec_list) {
        if (cmd.option_name == "help") {
            print_help(parser);
            return 0;
        }
        if (cmd.option_name == "count") {
            seq_count = str2Int(cmd.value);
        } else if (cmd.option_name == "host") {
            host = cmd.value;
        }
    }
    if (!OS::isAdmin()) {
        Terminal::printError("ping: Permission denied! Please run as administrator/root.\r\n");
        return -1;
    }

    signal(SIGINT, cancel_by_user);

    bool ok;
    auto parse_host = parseFirstHostname(host.c_str(), &ok);

    if (ok) {
        if (!sock.setAddress(std::move(parse_host))) {
            Terminal::printError("ping: Binding address error!\r\n");
            return -1;
        }
    } else {
        Terminal::printError("ping: Invalid address: \"{}\"\r\n", host);
        return -1;
    }
    auto addr_str = sock.address().toString();
    Terminal::printFormat("Sending 32-byte packet(s) to {}:\r\n", addr_str);
    if (seq_count == 0) seq_count = INT_MAX;

    while (sock.sendCount() < seq_count) {
        if (sock.send()) {
            ICMP_Packet pck;
            ICMP_Socket::EchoRequest echo_req{};
            uint8_t ttl = 0;
            if (sock.recv(&pck, &echo_req, &ttl)) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                bool is_less_zero = echo_req.times == 0;
                times_seq_list.push_back(echo_req.times);
                Terminal::print() << "From " << addr_str << ": "
                                  << "id=" << static_cast<int>(ntohs(echo_req.id))
                                  << " seq=" << static_cast<int>(ntohs(echo_req.seq))
                                  << " times" << (is_less_zero ? "<1" : "=" + std::to_string(echo_req.times)) << "ms";
                if (!sock.address().isIPv6()) {
                    Terminal::print() << " TTL=" << static_cast<int>(ttl);
                }
                Terminal::print() << " data=\"" << echo_req.data << "\""
                                  << "(" << strlen(echo_req.data) << ")\r\n";
            } else {
                Terminal::perror() << "Error: " << sock.error() << "\r\n";
            }
            continue;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Terminal::printLine(sock.error());
    }
    print_result(sock, times_seq_list);
    sock.close();
    return 0;
}

