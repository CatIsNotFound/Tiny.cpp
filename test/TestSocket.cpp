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

#include <gtest/gtest.h>
#include "../src/Net/Socket.hpp"

#include <thread>
#include <chrono>
#include <atomic>
#include <string>

using namespace Tiny::Net;

// ============================================================================
// Test constants: dedicated loopback ports for this test binary only
// ============================================================================

static const uint16_t kTcpPort = 43190;       // TCP loopback test
static const uint16_t kUdpPort = 43191;       // UDP loopback test
static const uint16_t kBindInUsePort = 43192; // address-in-use test
static const uint16_t kRefusedPort = 43195;   // no listener -> connection refused

// ============================================================================
// PortProtocol Tests
// ============================================================================

TEST(PortProtocolTest, WebPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::HTTP), 80);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::HTTPS), 443);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SOCKS), 1080);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DoH), 443);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::RTMP), 1935);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::RTSP), 554);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SIP), 5060);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SIPS), 5061);
}

TEST(PortProtocolTest, RemoteAccessPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SSH), 22);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SFTP), 22);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::Telnet), 23);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::RDP), 3389);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::VNC), 5900);
}

TEST(PortProtocolTest, TransferPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::FTP_Con), 21);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::FTP_Data), 20);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::FTPS_Con), 990);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::FTPS_Data), 989);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::TFTP), 69);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::NFS), 2049);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SMB), 445);
}

TEST(PortProtocolTest, MailPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SMTP), 25);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SMTP_TLS), 587);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::POP3), 110);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::POP3S), 995);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::IMAP), 143);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::IMAPS), 993);
}

TEST(PortProtocolTest, InfraPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DHCP_Server), 67);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DHCP_Client), 68);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::NTP), 123);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SNMP_UDP), 161);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SNMP_Trap), 162);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::BGP), 179);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DNS), 53);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DoT), 853);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DoQ), 853);
}

TEST(PortProtocolTest, DatabaseAndMessageQueuePorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::MySQL), 3306);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::PostgreSQL), 5432);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::MongoDB), 27017);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::Redis), 6379);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::MS_SQL), 1433);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::Oracle), 1521);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::Elasticsearch), 9200);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::MQTT), 1883);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::MQTTS), 8883);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::AMQP), 5672);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::Kafka), 9092);
}

// ============================================================================
// SocketErrorName Tests
// ============================================================================

TEST(SocketErrorNameTest, KnownErrorNames) {
    EXPECT_STREQ(getSocketErrorName(SocketError::Success),
                 "Tiny::Net::SocketError::Success");
    EXPECT_STREQ(getSocketErrorName(SocketError::InvalidParameter),
                 "Tiny::Net::SocketError::InvalidParameter");
    EXPECT_STREQ(getSocketErrorName(SocketError::SocketOptionError),
                 "Tiny::Net::SocketError::SocketOptionError");
    EXPECT_STREQ(getSocketErrorName(SocketError::ProtoNotSupported),
                 "Tiny::Net::SocketError::ProtoNotSupported");
    EXPECT_STREQ(getSocketErrorName(SocketError::SocketIsNotOpened),
                 "Tiny::Net::SocketError::SocketIsNotOpened");
    EXPECT_STREQ(getSocketErrorName(SocketError::SocketClosed),
                 "Tiny::Net::SocketError::SocketClosed");
    EXPECT_STREQ(getSocketErrorName(SocketError::SocketInUse),
                 "Tiny::Net::SocketError::SocketInUse");
    EXPECT_STREQ(getSocketErrorName(SocketError::AddressInUse),
                 "Tiny::Net::SocketError::AddressInUse");
    EXPECT_STREQ(getSocketErrorName(SocketError::AddressNotAvailable),
                 "Tiny::Net::SocketError::AddressNotAvailable");
    EXPECT_STREQ(getSocketErrorName(SocketError::AddressNotSupport),
                 "Tiny::Net::SocketError::AddressNotSupport");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionRefused),
                 "Tiny::Net::SocketError::ConnectionRefused");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionInProgress),
                 "Tiny::Net::SocketError::ConnectionInProgress");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionInUsed),
                 "Tiny::Net::SocketError::ConnectionInUsed");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionReset),
                 "Tiny::Net::SocketError::ConnectionReset");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionAborted),
                 "Tiny::Net::SocketError::ConnectionAborted");
    EXPECT_STREQ(getSocketErrorName(SocketError::ConnectionTimeout),
                 "Tiny::Net::SocketError::ConnectionTimeout");
    EXPECT_STREQ(getSocketErrorName(SocketError::NetworkUnreachable),
                 "Tiny::Net::SocketError::NetworkUnreachable");
    EXPECT_STREQ(getSocketErrorName(SocketError::HostUnreachable),
                 "Tiny::Net::SocketError::HostUnreachable");
    EXPECT_STREQ(getSocketErrorName(SocketError::NetworkDown),
                 "Tiny::Net::SocketError::NetworkDown");
    EXPECT_STREQ(getSocketErrorName(SocketError::MessageTooLong),
                 "Tiny::Net::SocketError::MessageTooLong");
    EXPECT_STREQ(getSocketErrorName(SocketError::ResourceUnavailable),
                 "Tiny::Net::SocketError::ResourceUnavailable");
    EXPECT_STREQ(getSocketErrorName(SocketError::OperationDenied),
                 "Tiny::Net::SocketError::OperationDenied");
    EXPECT_STREQ(getSocketErrorName(SocketError::SystemResourceLimit),
                 "Tiny::Net::SocketError::SystemResourceLimit");
    EXPECT_STREQ(getSocketErrorName(SocketError::OperationInProgress),
                 "Tiny::Net::SocketError::OperationInProgress");
    EXPECT_STREQ(getSocketErrorName(SocketError::OperationCancelled),
                 "Tiny::Net::SocketError::OperationCancelled");
    EXPECT_STREQ(getSocketErrorName(SocketError::OperationNotSupported),
                 "Tiny::Net::SocketError::OperationNotSupported");
    EXPECT_STREQ(getSocketErrorName(SocketError::SegmentationFault),
                 "Tiny::Net::SocketError::SegmentationFault");
    EXPECT_STREQ(getSocketErrorName(SocketError::UnknownError),
                 "Tiny::Net::SocketError::UnknownError");
}

// ============================================================================
// Socket enums Tests
// ============================================================================

TEST(SocketTypeEnumTest, Values) {
    EXPECT_EQ(static_cast<int>(SocketType::TCP), 0);
    EXPECT_EQ(static_cast<int>(SocketType::UDP), 1);
    EXPECT_EQ(static_cast<int>(SocketType::SCTP), 2);
    EXPECT_EQ(static_cast<int>(SocketType::ICMP), 3);
    EXPECT_EQ(static_cast<int>(SocketType::ICMPV6), 4);
    EXPECT_EQ(static_cast<int>(SocketType::Custom), 5);
}

TEST(SocketStateEnumTest, Values) {
    EXPECT_EQ(static_cast<int>(SocketState::Unused), 0);
    EXPECT_EQ(static_cast<int>(SocketState::Closed), 1);
    EXPECT_EQ(static_cast<int>(SocketState::Bound), 2);
    EXPECT_EQ(static_cast<int>(SocketState::ParsingAddress), 3);
    EXPECT_EQ(static_cast<int>(SocketState::Connecting), 4);
    EXPECT_EQ(static_cast<int>(SocketState::Connected), 5);
    EXPECT_EQ(static_cast<int>(SocketState::Listening), 6);
    EXPECT_EQ(static_cast<int>(SocketState::Closing), 7);
    EXPECT_EQ(static_cast<int>(SocketState::Shutdown), 8);
}

TEST(SocketOptionEnumTest, Values) {
    EXPECT_EQ(static_cast<int>(SocketOption::AllowedBroadcast), 1);
    EXPECT_EQ(static_cast<int>(SocketOption::DontRoute), 2);
    EXPECT_EQ(static_cast<int>(SocketOption::KeepAlive), 3);
    EXPECT_EQ(static_cast<int>(SocketOption::NoDelay), 4);
    EXPECT_EQ(static_cast<int>(SocketOption::MaxSegmentSize), 5);
    EXPECT_EQ(static_cast<int>(SocketOption::KeepIdle), 6);
    EXPECT_EQ(static_cast<int>(SocketOption::KeepInterval), 7);
    EXPECT_EQ(static_cast<int>(SocketOption::KeepCount), 8);
    EXPECT_EQ(static_cast<int>(SocketOption::SendBufSize), 9);
    EXPECT_EQ(static_cast<int>(SocketOption::RecvBufSize), 10);
    EXPECT_EQ(static_cast<int>(SocketOption::SendBufTimeout), 11);
    EXPECT_EQ(static_cast<int>(SocketOption::RecvBufTimeout), 12);
    EXPECT_EQ(static_cast<int>(SocketOption::Linger), 13);
    EXPECT_EQ(static_cast<int>(SocketOption::ReuseAddr), 14);
    EXPECT_EQ(static_cast<int>(SocketOption::TTL), 15);
    EXPECT_EQ(static_cast<int>(SocketOption::NativeSocketError), 255);
}

// ============================================================================
// OptionValue Tests
// ============================================================================

TEST(OptionValueTest, DefaultIsNone) {
    OptionValue value;
    EXPECT_EQ(value.type, OptionValue::None);
    EXPECT_EQ(value.size, 0);
    EXPECT_EQ(value.var.v, nullptr);
}

TEST(OptionValueTest, Constructors) {
    OptionValue i(1024);
    EXPECT_EQ(i.type, OptionValue::Int);
    EXPECT_EQ(i.size, static_cast<int>(sizeof(int)));
    EXPECT_EQ(i.var.i, 1024);

    OptionValue u(static_cast<uint32_t>(4096));
    EXPECT_EQ(u.type, OptionValue::UInt);
    EXPECT_EQ(u.size, static_cast<int>(sizeof(uint32_t)));
    EXPECT_EQ(u.var.u, 4096u);

    OptionValue f(1.5f);
    EXPECT_EQ(f.type, OptionValue::Float);
    EXPECT_EQ(f.size, static_cast<int>(sizeof(float)));
    EXPECT_FLOAT_EQ(f.var.f, 1.5f);

    OptionValue s("hello");
    EXPECT_EQ(s.type, OptionValue::String);
    EXPECT_EQ(s.size, static_cast<int>(strlen("hello")));
    EXPECT_STREQ(s.var.s, "hello");

    int payload = 42;
    OptionValue c(&payload, sizeof(int));
    EXPECT_EQ(c.type, OptionValue::Custom);
    EXPECT_EQ(c.size, static_cast<int>(sizeof(int)));
    EXPECT_EQ(c.var.v, &payload);
}

TEST(OptionValueTest, Setters) {
    OptionValue value;

    value.set(11);
    EXPECT_EQ(value.type, OptionValue::Int);
    EXPECT_EQ(value.var.i, 11);

    value.set(0.5f);
    EXPECT_EQ(value.type, OptionValue::Float);
    EXPECT_FLOAT_EQ(value.var.f, 0.5f);

    value.set(static_cast<uint32_t>(7));
    EXPECT_EQ(value.type, OptionValue::UInt);
    EXPECT_EQ(value.var.u, 7u);

    value.set("text");
    EXPECT_EQ(value.type, OptionValue::String);
    EXPECT_EQ(value.size, static_cast<int>(strlen("text")));
    EXPECT_STREQ(value.var.s, "text");

    int data = 9;
    value.set(&data, sizeof(int));
    EXPECT_EQ(value.type, OptionValue::Custom);
    EXPECT_EQ(value.var.v, &data);

    value.unset();
    EXPECT_EQ(value.type, OptionValue::None);
    EXPECT_EQ(value.size, 0);
    EXPECT_EQ(value.var.v, nullptr);
}

TEST(OptionValueTest, AssignmentOperators) {
    OptionValue value;

    value = 33;
    EXPECT_EQ(value.type, OptionValue::Int);
    EXPECT_EQ(value.var.i, 33);

    value = "raw";
    EXPECT_EQ(value.type, OptionValue::String);
    EXPECT_STREQ(value.var.s, "raw");

    value = 2.25f;
    EXPECT_EQ(value.type, OptionValue::Float);
    EXPECT_FLOAT_EQ(value.var.f, 2.25f);

    value = nullptr;
    EXPECT_EQ(value.type, OptionValue::None);

    OptionValue assigned;
    assigned = OptionValue(77);
    EXPECT_EQ(assigned.type, OptionValue::Int);
    EXPECT_EQ(assigned.var.i, 77);
}

TEST(OptionValueTest, EqualityOperators) {
    OptionValue a(10);
    OptionValue b(10);
    OptionValue d(11);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != d);
    EXPECT_FALSE(a == d);

    // Different types are not equal even with the same raw value
    OptionValue u(static_cast<uint32_t>(10));
    EXPECT_TRUE(a != u);
    EXPECT_FALSE(a == u);

    OptionValue s1("abc");
    OptionValue s2("abc");
    OptionValue s3("abd");
    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 != s2);
    EXPECT_TRUE(s1 != s3);

    OptionValue n1;
    OptionValue n2;
    EXPECT_TRUE(n1 == n2);
}

// ============================================================================
// Address Tests
// ============================================================================

TEST(AddressTest, DefaultInvalid) {
    Address addr;
    EXPECT_FALSE(addr.isValid());
    EXPECT_TRUE(addr.toString().empty());
    EXPECT_EQ(addr.address(), nullptr);
}

TEST(AddressTest, IPv4Ctor) {
    Address addr("127.0.0.1", 8080);
    EXPECT_TRUE(addr.isValid());
    EXPECT_TRUE(addr.isSpecified());
    EXPECT_FALSE(addr.isIPv6());
    EXPECT_EQ(addr.port(), 8080);
    EXPECT_STREQ(addr.toString().c_str(), "127.0.0.1");
    EXPECT_NE(addr.address(), nullptr);
}

TEST(AddressTest, IPv6Ctor) {
    Address addr("::1", 9090, true);
    EXPECT_TRUE(addr.isValid());
    EXPECT_TRUE(addr.isSpecified());
    EXPECT_TRUE(addr.isIPv6());
    EXPECT_EQ(addr.port(), 9090);
    EXPECT_STREQ(addr.toString().c_str(), "::1");
}

TEST(AddressTest, InvalidAddress) {
    Address invalid("not-a-valid-ip", 1234);
    EXPECT_FALSE(invalid.isValid());
    EXPECT_TRUE(invalid.toString().empty());
    EXPECT_EQ(invalid.address(), nullptr);
}

TEST(AddressTest, UnspecifiedAddress) {
    Address addr("0.0.0.0", 0);
    EXPECT_TRUE(addr.isValid());
    EXPECT_FALSE(addr.isSpecified());

    Address made = Address::makeAddress();
    EXPECT_TRUE(made.isValid());
    EXPECT_FALSE(made.isSpecified());
    EXPECT_STREQ(made.toString().c_str(), "0.0.0.0");

    Address made_v6 = Address::makeAddress(true);
    EXPECT_TRUE(made_v6.isValid());
    EXPECT_TRUE(made_v6.isIPv6());
    EXPECT_STREQ(made_v6.toString().c_str(), "::");
}

TEST(AddressTest, LocalHostHelpers) {
    Address localhost = Address::localHost();
    EXPECT_TRUE(localhost.isValid());
    EXPECT_TRUE(localhost.isSpecified());
    EXPECT_FALSE(localhost.isIPv6());
    EXPECT_EQ(localhost.port(), 0);
    EXPECT_STREQ(localhost.toString().c_str(), "127.0.0.1");

    Address localhost_v6 = Address::localHostIPv6();
    EXPECT_TRUE(localhost_v6.isValid());
    EXPECT_TRUE(localhost_v6.isIPv6());
    EXPECT_STREQ(localhost_v6.toString().c_str(), "::1");

    EXPECT_TRUE(localhost == Address("127.0.0.1", 0));
    EXPECT_TRUE(localhost != Address("127.0.0.1", 1));
    EXPECT_TRUE(localhost != localhost_v6);
}

TEST(AddressTest, Setters) {
    Address addr;
    addr.setAddress("127.0.0.1", 1234);
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 1234);
    EXPECT_STREQ(addr.toString().c_str(), "127.0.0.1");

    addr.setPort(8081);
    EXPECT_EQ(addr.port(), 8081);

    addr.setPort(PortProtocol::HTTP);
    EXPECT_EQ(addr.port(), 80);

    EXPECT_TRUE(addr.validate());
    EXPECT_TRUE(addr.isSpecified());
}

TEST(AddressTest, MoveSemantics) {
    Address addr("127.0.0.1", 4321);
    Address moved(std::move(addr));
    EXPECT_TRUE(moved.isValid());
    EXPECT_EQ(moved.port(), 4321);
    EXPECT_STREQ(moved.toString().c_str(), "127.0.0.1");

    Address assigned;
    assigned = std::move(moved);
    EXPECT_TRUE(assigned.isValid());
    EXPECT_EQ(assigned.port(), 4321);
    EXPECT_STREQ(assigned.toString().c_str(), "127.0.0.1");
}

TEST(AddressParseTest, ParseFromHostnameLocalhost) {
    bool ok = false;
    int err_cnt = 0;
    auto addresses = Address::parseFromHostname("localhost", &ok, &err_cnt);
    EXPECT_TRUE(ok);
    EXPECT_FALSE(addresses.empty());
    for (auto& addr : addresses) {
        EXPECT_TRUE(addr.isValid());
        EXPECT_STRNE(addr.toString().c_str(), "");
    }
}

TEST(AddressParseTest, ParseFirstHostname) {
    bool ok = false;
    Address addr = Address::parseFirstHostname("localhost", &ok);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(addr.isValid());
    EXPECT_STRNE(addr.toString().c_str(), "");
}

TEST(AddressParseTest, ParseInvalidHostname) {
    bool ok = false;
    int err_cnt = 0;
    auto addresses = Address::parseFromHostname("no-such-host-name-xyz", &ok, &err_cnt);
    EXPECT_FALSE(ok);
    EXPECT_TRUE(addresses.empty());
}

// ============================================================================
// Socket basic Tests
// ============================================================================

TEST(SocketBasicTest, DefaultSocket) {
    Socket socket;
    EXPECT_EQ(socket.type(), SocketType::TCP);
    EXPECT_EQ(socket.state(), SocketState::Unused);
    EXPECT_EQ(socket.lastError(), SocketError::Success);
    EXPECT_EQ(socket.errorSocketOptionID(), 0);
    EXPECT_EQ(socket.nativeErrorNo(), 0);

    // The local address defaults to an unspecified one
    const Address& local = socket.localAddress();
    EXPECT_TRUE(local.isValid());
    EXPECT_FALSE(local.isSpecified());
}

TEST(SocketBasicTest, SendRecvWithoutOpen) {
    Socket socket;
    std::string msg;

    EXPECT_FALSE(socket.send(std::string("data")));
    EXPECT_EQ(socket.lastError(), SocketError::SocketIsNotOpened);

    EXPECT_FALSE(socket.recv(msg, 1024));
    EXPECT_EQ(socket.lastError(), SocketError::SocketIsNotOpened);
}

TEST(SocketBasicTest, SendToWithoutOpen) {
    Socket socket;
    // Unlike send()/recv(), sendTo() auto-creates the native socket when the
    // handle is missing (UDP-style unconnected usage), so it never reports
    // SocketIsNotOpened. On an unconnected TCP socket the OS then rejects the
    // send; the exact error code (and the resulting state) is OS-dependent.
    EXPECT_FALSE(socket.sendTo(std::string("data"), Address::localHost()));
    EXPECT_NE(socket.lastError(), SocketError::Success);
    EXPECT_NE(socket.lastError(), SocketError::SocketIsNotOpened);
}

TEST(SocketBasicTest, ConnectWithoutPeer) {
    Socket socket;
    // The default peer address ("0.0.0.0:0") is valid but unspecified, so the
    // isValid() guard inside connect() passes and the OS rejects the attempt.
    // The exact error is OS-dependent (ConnectionRefused on Linux), but it is
    // never the InvalidParameter guard nor a success.
    EXPECT_FALSE(socket.connect());
    EXPECT_NE(socket.lastError(), SocketError::Success);
    EXPECT_NE(socket.lastError(), SocketError::InvalidParameter);
    EXPECT_EQ(socket.state(), SocketState::Closed);
}

TEST(SocketBasicTest, SettersBeforeOpen) {
    Socket socket;
    EXPECT_TRUE(socket.setLocalAddress("127.0.0.1", 8080));
    EXPECT_EQ(socket.localAddress().port(), 8080);

    EXPECT_TRUE(socket.setPeerAddress("127.0.0.1", 8081));
    EXPECT_EQ(socket.peerAddress().port(), 8081);

    EXPECT_TRUE(socket.setLocalPort(80));
    EXPECT_EQ(socket.localAddress().port(), 80);

    EXPECT_TRUE(socket.setPeerPort(81));
    EXPECT_EQ(socket.peerAddress().port(), 81);

    EXPECT_TRUE(socket.setSocketType(SocketType::UDP));
    EXPECT_EQ(socket.type(), SocketType::UDP);

    // Custom socket type only stores message/proto numbers
    // (SOCK_DGRAM = 2, IPPROTO_UDP = 17)
    socket.setCustomSocketType(2, 17);
    EXPECT_EQ(socket.type(), SocketType::UDP);
}

// ============================================================================
// Socket bind Tests
// ============================================================================

TEST(SocketBindTest, BindTCPLoopbackEphemeral) {
    Socket socket;
    EXPECT_TRUE(socket.bind("127.0.0.1", 0));
    EXPECT_EQ(socket.state(), SocketState::Bound);
    EXPECT_EQ(socket.lastError(), SocketError::Success);

    auto local = socket.localAddress().port();
    EXPECT_TRUE(socket.localAddress().isValid());
    EXPECT_TRUE(socket.localAddress().isSpecified());
    EXPECT_NE(local, 0);

    socket.close();
    EXPECT_EQ(socket.state(), SocketState::Closed);
}

TEST(SocketBindTest, BindUDPLoopbackEphemeral) {
    Socket socket;
    socket.setSocketType(SocketType::UDP);
    EXPECT_TRUE(socket.bind("127.0.0.1", 0));
    EXPECT_EQ(socket.state(), SocketState::Bound);
    EXPECT_NE(socket.localAddress().port(), 0);

    socket.close();
    EXPECT_EQ(socket.state(), SocketState::Closed);
}

TEST(SocketBindTest, BindInvalidAddress) {
    Socket socket;
    EXPECT_FALSE(socket.bind("999.999.999.999", 1234));
    EXPECT_EQ(socket.lastError(), SocketError::InvalidParameter);
    EXPECT_EQ(socket.state(), SocketState::Unused);
}

TEST(SocketBindTest, BindAddressInUse) {
    Socket first;
    ASSERT_TRUE(first.bind("127.0.0.1", kBindInUsePort));

    Socket second;
    EXPECT_FALSE(second.bind("127.0.0.1", kBindInUsePort));
    EXPECT_EQ(second.lastError(), SocketError::AddressInUse);

    first.close();
}

// ============================================================================
// Socket option Tests
// ============================================================================

TEST(SocketOptionTest, SetOptionBeforeOpenIsStored) {
    Socket socket;
    bool set_ok = false;
    socket.setOption(SocketOption::KeepAlive, OptionValue(1), &set_ok);
    EXPECT_TRUE(set_ok);

    // Not opened yet: the value is stored and can be read back
    bool get_ok = false;
    OptionValue value = socket.option(SocketOption::KeepAlive, &get_ok);
    EXPECT_TRUE(get_ok);
    EXPECT_EQ(value.type, OptionValue::Int);
    EXPECT_GE(value.var.i, 1);
}

TEST(SocketOptionTest, SetAndGetKeepAlive) {
    Socket socket;
    ASSERT_TRUE(socket.bind("127.0.0.1", 0));

    bool set_ok = false;
    socket.setOption(SocketOption::KeepAlive, OptionValue(1), &set_ok);
    EXPECT_TRUE(set_ok);

    bool get_ok = false;
    OptionValue value = socket.option(SocketOption::KeepAlive, &get_ok);
    EXPECT_TRUE(get_ok);
    EXPECT_EQ(value.type, OptionValue::Int);
    EXPECT_GE(value.var.i, 1);

    socket.close();
}

TEST(SocketOptionTest, SetAndGetBufferSizes) {
    Socket socket;
    ASSERT_TRUE(socket.bind("127.0.0.1", 0));

    bool set_ok = false;
    socket.setOption(SocketOption::SendBufSize, OptionValue(16384), &set_ok);
    EXPECT_TRUE(set_ok);
    socket.setOption(SocketOption::RecvBufSize, OptionValue(16384), &set_ok);
    EXPECT_TRUE(set_ok);

    // The system may adjust the requested buffer size (e.g. doubling it)
    bool get_ok = false;
    OptionValue send_buf = socket.option(SocketOption::SendBufSize, &get_ok);
    EXPECT_TRUE(get_ok);
    EXPECT_EQ(send_buf.type, OptionValue::Int);
    EXPECT_GE(send_buf.var.i, 16384);

    get_ok = false;
    OptionValue recv_buf = socket.option(SocketOption::RecvBufSize, &get_ok);
    EXPECT_TRUE(get_ok);
    EXPECT_EQ(recv_buf.type, OptionValue::Int);
    EXPECT_GE(recv_buf.var.i, 16384);

    socket.close();
}

TEST(SocketOptionTest, SetAndGetTTL) {
    Socket socket;
    ASSERT_TRUE(socket.bind("127.0.0.1", 0));

    bool set_ok = false;
    socket.setOption(SocketOption::TTL, OptionValue(64), &set_ok);
    EXPECT_TRUE(set_ok);

    bool get_ok = false;
    OptionValue value = socket.option(SocketOption::TTL, &get_ok);
    EXPECT_TRUE(get_ok);
    EXPECT_EQ(value.type, OptionValue::Int);
    EXPECT_EQ(value.var.i, 64);

    socket.close();
}

TEST(SocketOptionTest, NoDelayOnTCPSocket) {
    Socket socket;
    ASSERT_TRUE(socket.bind("127.0.0.1", 0));

    bool set_ok = false;
    socket.setOption(SocketOption::NoDelay, OptionValue(1), &set_ok);
    EXPECT_TRUE(set_ok);

    socket.close();
}

TEST(SocketOptionTest, UnknownOption) {
    Socket socket;
    ASSERT_TRUE(socket.bind("127.0.0.1", 0));

    const uint32_t kUnknownID = 999;
    bool set_ok = true;
    socket.setOption(kUnknownID, OptionValue(1), &set_ok);
    EXPECT_FALSE(set_ok);
    EXPECT_EQ(socket.lastError(), SocketError::SocketOptionError);
    EXPECT_EQ(socket.errorSocketOptionID(), kUnknownID);

    bool get_ok = true;
    socket.option(kUnknownID, &get_ok);
    EXPECT_FALSE(get_ok);
    EXPECT_EQ(socket.errorSocketOptionID(), kUnknownID);

    socket.close();
}

// ============================================================================
// TCP loopback Tests
// ============================================================================

TEST(TCPLoopbackTest, ListenAcceptSendRecvClose) {
    Socket server;
    bool opt_ok = false;
    server.setOption(SocketOption::ReuseAddr, OptionValue(1), &opt_ok);
    EXPECT_TRUE(opt_ok);
    ASSERT_TRUE(server.listen(kTcpPort, 4));
    EXPECT_EQ(server.state(), SocketState::Listening);
    EXPECT_EQ(server.lastError(), SocketError::Success);

    Socket client;
    ASSERT_TRUE(client.connect("127.0.0.1", kTcpPort, 0));
    EXPECT_EQ(client.state(), SocketState::Connected);

    bool ok = false;
    Socket accepted = server.accept(&ok);
    ASSERT_TRUE(ok);
    EXPECT_EQ(accepted.state(), SocketState::Connected);
    EXPECT_TRUE(accepted.peerAddress().isValid());

    // Client -> Server
    int sended = -1;
    EXPECT_TRUE(client.send(std::string("Hello Tiny"), &sended));
    EXPECT_EQ(sended, 10);

    std::string message;
    int len = -1;
    EXPECT_TRUE(accepted.recv(message, 1024, &len));
    EXPECT_EQ(len, 10);
    EXPECT_EQ(message, std::string("Hello Tiny"));

    // Server -> Client
    sended = -1;
    EXPECT_TRUE(accepted.send(std::string("Bye Tiny"), &sended));
    EXPECT_EQ(sended, 8);

    len = -1;
    EXPECT_TRUE(client.recv(message, 1024, &len));
    EXPECT_EQ(len, 8);
    EXPECT_EQ(message, std::string("Bye Tiny"));

    // Closing the client makes the peer read EOF
    EXPECT_TRUE(client.close());
    EXPECT_EQ(client.state(), SocketState::Closed);

    len = -1;
    EXPECT_FALSE(accepted.recv(message, 1024, &len));
    EXPECT_EQ(accepted.lastError(), SocketError::SocketClosed);
    EXPECT_EQ(accepted.state(), SocketState::Closed);

    EXPECT_TRUE(accepted.close());
    EXPECT_TRUE(server.close());
    EXPECT_EQ(server.state(), SocketState::Closed);
}

TEST(TCPLoopbackTest, ListenWithPortProtocol) {
    Socket server;
    server.setOption(SocketOption::ReuseAddr, OptionValue(1));
    ASSERT_TRUE(server.listen(kTcpPort, 4));

    Socket client;
    ASSERT_TRUE(client.connect("127.0.0.1", PortProtocol(kTcpPort), 0));
    EXPECT_EQ(client.state(), SocketState::Connected);

    bool ok = false;
    Socket accepted = server.accept(&ok);
    ASSERT_TRUE(ok);

    int sended = -1;
    EXPECT_TRUE(client.send(std::string("port-proto"), &sended));
    EXPECT_EQ(sended, 10);

    std::string message;
    EXPECT_TRUE(accepted.recv(message, 1024));
    EXPECT_EQ(message, std::string("port-proto"));

    client.close();
    accepted.close();
    server.close();
}

TEST(TCPLoopbackTest, ConnectRefused) {
    Socket client;
    EXPECT_FALSE(client.connect("127.0.0.1", kRefusedPort, 0));
    EXPECT_EQ(client.lastError(), SocketError::ConnectionRefused);
    EXPECT_NE(client.nativeErrorNo(), 0);
    EXPECT_EQ(client.state(), SocketState::Closed);
}

TEST(TCPLoopbackTest, AcceptWithoutListen) {
    Socket server;
    bool ok = true;
    Socket accepted = server.accept(&ok);
    EXPECT_FALSE(ok);
    EXPECT_EQ(server.lastError(), SocketError::InvalidParameter);
}

// ============================================================================
// UDP loopback Tests
// ============================================================================

TEST(UDPTest, SendToAndRecv) {
    Socket receiver;
    receiver.setSocketType(SocketType::UDP);
    ASSERT_TRUE(receiver.bind("127.0.0.1", kUdpPort));
    EXPECT_EQ(receiver.state(), SocketState::Bound);

    Socket sender;
    sender.setSocketType(SocketType::UDP);
    ASSERT_TRUE(sender.bind("127.0.0.1", 0));

    Address target("127.0.0.1", kUdpPort);
    ASSERT_TRUE(target.isValid());
    int sended = -1;
    EXPECT_TRUE(sender.sendTo(std::string("udp-payload"), target, &sended));
    EXPECT_EQ(sended, 11);

    std::string message;
    int len = -1;
    Address source;
    EXPECT_TRUE(receiver.recv(message, 1024, &len, &source));
    EXPECT_EQ(len, 11);
    EXPECT_EQ(message, std::string("udp-payload"));
    EXPECT_EQ(receiver.lastError(), SocketError::Success);

    receiver.close();
    sender.close();
}

TEST(UDPTest, SendToAndRecvNetDatas) {
    Socket receiver;
    receiver.setSocketType(SocketType::UDP);
    ASSERT_TRUE(receiver.bind("127.0.0.1", kUdpPort));

    Socket sender;
    sender.setSocketType(SocketType::UDP);
    ASSERT_TRUE(sender.bind("127.0.0.1", 0));

    NetDatas payload;
    payload.push_back('B');
    payload.push_back('Y');
    payload.push_back('T');
    payload.push_back('E');
    int sended = -1;
    EXPECT_TRUE(sender.sendTo(payload, Address("127.0.0.1", kUdpPort), &sended));
    EXPECT_EQ(sended, 4);

    NetDatas received;
    int len = -1;
    EXPECT_TRUE(receiver.recv(received, 1024, &len));
    ASSERT_EQ(len, 4);
    ASSERT_EQ(received.size(), 4u);
    EXPECT_EQ(received[0], 'B');
    EXPECT_EQ(received[3], 'E');

    receiver.close();
    sender.close();
}

// ============================================================================
// Socket move Tests
// ============================================================================

TEST(SocketMoveTest, MoveConstructor) {
    Socket src;
    ASSERT_TRUE(src.bind("127.0.0.1", 0));
    uint16_t port = src.localAddress().port();
    Handle src_handle = src.nativeHandle();

    Socket dst(std::move(src));
    EXPECT_EQ(dst.state(), SocketState::Bound);
    EXPECT_EQ(dst.localAddress().port(), port);
    EXPECT_EQ(dst.nativeHandle(), src_handle);
    EXPECT_NE(src.nativeHandle(), dst.nativeHandle());

    dst.close();
    EXPECT_EQ(dst.state(), SocketState::Closed);
}

TEST(SocketMoveTest, MoveAssignment) {
    Socket src;
    src.setSocketType(SocketType::UDP);
    ASSERT_TRUE(src.bind("127.0.0.1", 0));
    uint16_t port = src.localAddress().port();

    Socket dst;
    dst = std::move(src);
    EXPECT_EQ(dst.type(), SocketType::UDP);
    EXPECT_EQ(dst.state(), SocketState::Bound);
    EXPECT_EQ(dst.localAddress().port(), port);

    dst.close();
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
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
