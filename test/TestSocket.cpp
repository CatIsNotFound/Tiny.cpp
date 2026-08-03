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

using namespace Tiny;
using namespace Tiny::Net;

// ============================================================================
// Address Tests
// ============================================================================

TEST(AddressTest, DefaultConstructor) {
    Address addr;
    EXPECT_FALSE(addr.isValid());
    EXPECT_EQ(addr.port(), UINT16_MAX);
    EXPECT_FALSE(addr.isIPv6());
}

TEST(AddressTest, ConstructorWithIPv4) {
    Address addr("127.0.0.1", 8080);
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 8080);
    EXPECT_FALSE(addr.isIPv6());
}

TEST(AddressTest, ConstructorWithIPv6) {
    Address addr("::1", 8080, true);
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 8080);
    EXPECT_TRUE(addr.isIPv6());
}

TEST(AddressTest, ConstructorWithPortProtocol) {
    Address addr("127.0.0.1", PortProtocol::HTTP);
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 80);
}

TEST(AddressTest, ToString) {
    Address addr("192.168.1.1", 9000);
    bool ok = false;
    std::string str = addr.toString(&ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(str, "192.168.1.1");
}

TEST(AddressTest, SetAddress) {
    Address addr;
    addr.setAddress("10.0.0.1", 3000);
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 3000);
}

TEST(AddressTest, SetPort) {
    Address addr("127.0.0.1", 80);
    addr.setPort(443);
    EXPECT_EQ(addr.port(), 443);
}

TEST(AddressTest, SetPortProtocol) {
    Address addr("127.0.0.1", 80);
    addr.setPort(PortProtocol::HTTPS);
    EXPECT_EQ(addr.port(), 443);
}

TEST(AddressTest, LocalHost) {
    Address addr = Address::localHost();
    EXPECT_TRUE(addr.isValid());
    EXPECT_FALSE(addr.isIPv6());
}

TEST(AddressTest, LocalHostIPv6) {
    Address addr = Address::localHostIPv6();
    EXPECT_TRUE(addr.isValid());
    EXPECT_TRUE(addr.isIPv6());
}

TEST(AddressTest, MoveConstructor) {
    Address addr1("127.0.0.1", 8080);
    Address addr2(std::move(addr1));
    EXPECT_TRUE(addr2.isValid());
    EXPECT_EQ(addr2.port(), 8080);
}

TEST(AddressTest, MoveAssignment) {
    Address addr1("127.0.0.1", 8080);
    Address addr2;
    addr2 = std::move(addr1);
    EXPECT_TRUE(addr2.isValid());
    EXPECT_EQ(addr2.port(), 8080);
}

TEST(AddressTest, EqualityOperator) {
    Address addr1("127.0.0.1", 8080);
    Address addr2("127.0.0.1", 8080);
    Address addr3("127.0.0.1", 9000);
    
    EXPECT_TRUE(addr1 == addr2);
    EXPECT_FALSE(addr1 == addr3);
}

TEST(AddressTest, InequalityOperator) {
    Address addr1("127.0.0.1", 8080);
    Address addr2("127.0.0.1", 9000);
    Address addr3("127.0.0.1", 8080);
    
    EXPECT_TRUE(addr1 != addr2);
    EXPECT_FALSE(addr1 != addr3);
}

// ============================================================================
// OptionValue Tests
// ============================================================================

TEST(OptionValueTest, DefaultConstructor) {
    OptionValue val;
    EXPECT_EQ(val.type, OptionValue::None);
    EXPECT_EQ(val.size, 0);
}

TEST(OptionValueTest, IntConstructor) {
    OptionValue val(42);
    EXPECT_EQ(val.type, OptionValue::Int);
    EXPECT_EQ(val.var.i, 42);
}

TEST(OptionValueTest, UIntConstructor) {
    OptionValue val(uint32_t(100));
    EXPECT_EQ(val.type, OptionValue::UInt);
    EXPECT_EQ(val.var.u, 100);
}

TEST(OptionValueTest, FloatConstructor) {
    OptionValue val(3.14f);
    EXPECT_EQ(val.type, OptionValue::Float);
    EXPECT_FLOAT_EQ(val.var.f, 3.14f);
}

TEST(OptionValueTest, StringConstructor) {
    OptionValue val("test");
    EXPECT_EQ(val.type, OptionValue::String);
    EXPECT_STREQ(val.var.s, "test");
}

TEST(OptionValueTest, SetInt) {
    OptionValue val;
    val.set(100);
    EXPECT_EQ(val.type, OptionValue::Int);
    EXPECT_EQ(val.var.i, 100);
}

TEST(OptionValueTest, SetUInt) {
    OptionValue val;
    val.set(uint32_t(200));
    EXPECT_EQ(val.type, OptionValue::UInt);
    EXPECT_EQ(val.var.u, 200);
}

TEST(OptionValueTest, SetFloat) {
    OptionValue val;
    val.set(2.5f);
    EXPECT_EQ(val.type, OptionValue::Float);
    EXPECT_FLOAT_EQ(val.var.f, 2.5f);
}

TEST(OptionValueTest, SetString) {
    OptionValue val;
    val.set("hello");
    EXPECT_EQ(val.type, OptionValue::String);
    EXPECT_STREQ(val.var.s, "hello");
}

TEST(OptionValueTest, Unset) {
    OptionValue val(42);
    val.unset();
    EXPECT_EQ(val.type, OptionValue::None);
    EXPECT_EQ(val.size, 0);
}

TEST(OptionValueTest, AssignmentOperatorInt) {
    OptionValue val;
    val = 50;
    EXPECT_EQ(val.type, OptionValue::Int);
    EXPECT_EQ(val.var.i, 50);
}

TEST(OptionValueTest, AssignmentOperatorUInt) {
    OptionValue val;
    val = uint32_t(75);
    EXPECT_EQ(val.type, OptionValue::UInt);
    EXPECT_EQ(val.var.u, 75);
}

TEST(OptionValueTest, AssignmentOperatorFloat) {
    OptionValue val;
    val = 1.5f;
    EXPECT_EQ(val.type, OptionValue::Float);
    EXPECT_FLOAT_EQ(val.var.f, 1.5f);
}

TEST(OptionValueTest, AssignmentOperatorNullptr) {
    OptionValue val(42);
    val = nullptr;
    EXPECT_EQ(val.type, OptionValue::None);
}

TEST(OptionValueTest, CopyAssignment) {
    OptionValue val1(100);
    OptionValue val2;
    val2 = val1;
    EXPECT_EQ(val2.type, OptionValue::Int);
    EXPECT_EQ(val2.var.i, 100);
}

TEST(OptionValueTest, EqualityOperator) {
    OptionValue val1(42);
    OptionValue val2(42);
    OptionValue val3(100);
    
    EXPECT_TRUE(val1 == val2);
    EXPECT_FALSE(val1 == val3);
}

TEST(OptionValueTest, InequalityOperator) {
    OptionValue val1(42);
    OptionValue val2(100);
    OptionValue val3(42);
    
    EXPECT_TRUE(val1 != val2);
    EXPECT_FALSE(val1 != val3);
}

// ============================================================================
// Socket Basic Tests
// ============================================================================

TEST(SocketTest, DefaultConstructor) {
    Socket sock;
    EXPECT_EQ(sock.type(), SocketType::TCP);
    EXPECT_EQ(sock.state(), SocketState::Unused);
    EXPECT_EQ(sock.lastError(), SocketError::Success);
}

TEST(SocketTest, ConstructorWithType) {
    Socket tcp_sock(SocketType::TCP);
    EXPECT_EQ(tcp_sock.type(), SocketType::TCP);
    
    Socket udp_sock(SocketType::UDP);
    EXPECT_EQ(udp_sock.type(), SocketType::UDP);
}

TEST(SocketTest, MoveConstructor) {
    Socket sock1(SocketType::TCP);
    Socket sock2(std::move(sock1));
    EXPECT_EQ(sock2.type(), SocketType::TCP);
    EXPECT_EQ(sock2.state(), SocketState::Unused);
}

TEST(SocketTest, MoveAssignment) {
    Socket sock1(SocketType::UDP);
    Socket sock2;
    sock2 = std::move(sock1);
    EXPECT_EQ(sock2.type(), SocketType::UDP);
}

TEST(SocketTest, SetSocketType) {
    Socket sock;
    sock.setSocketType(SocketType::UDP);
    EXPECT_EQ(sock.type(), SocketType::UDP);
}

TEST(SocketTest, SetLocalAddress) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 8080);
    const Address& addr = sock.localAddress();
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 8080);
}

TEST(SocketTest, SetLocalAddressWithObject) {
    Socket sock;
    Address addr("127.0.0.1", 9000);
    sock.setLocalAddress(std::move(addr));
    const Address& local = sock.localAddress();
    EXPECT_TRUE(local.isValid());
    EXPECT_EQ(local.port(), 9000);
}

TEST(SocketTest, SetLocalPort) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 80);
    sock.setLocalPort(443);
    EXPECT_EQ(sock.localAddress().port(), 443);
}

TEST(SocketTest, SetPeerAddress) {
    Socket sock;
    sock.setPeerAddress("127.0.0.1", 8080);
    const Address& addr = sock.peerAddress();
    EXPECT_TRUE(addr.isValid());
    EXPECT_EQ(addr.port(), 8080);
}

TEST(SocketTest, SetPeerAddressWithObject) {
    Socket sock;
    Address addr("127.0.0.1", 9000);
    sock.setPeerAddress(std::move(addr));
    const Address& peer = sock.peerAddress();
    EXPECT_TRUE(peer.isValid());
    EXPECT_EQ(peer.port(), 9000);
}

TEST(SocketTest, SetPeerPort) {
    Socket sock;
    sock.setPeerAddress("127.0.0.1", 80);
    sock.setPeerPort(443);
    EXPECT_EQ(sock.peerAddress().port(), 443);
}

// ============================================================================
// Socket Option Tests
// ============================================================================

TEST(SocketOptionTest, SetAndGetKeepAlive) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18012);
    ASSERT_TRUE(sock.listen(1));

    bool ok = false;
    sock.setOption(SocketOption::KeepAlive, OptionValue(1), &ok);
    EXPECT_TRUE(ok);

    OptionValue val = sock.option(SocketOption::KeepAlive, &ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(val.var.i, 1);
    sock.close();
}

TEST(SocketOptionTest, SetAndGetReuseAddr) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18013);
    ASSERT_TRUE(sock.listen(1));

    bool ok = false;
    sock.setOption(SocketOption::ReuseAddr, OptionValue(1), &ok);
    EXPECT_TRUE(ok);

    OptionValue val = sock.option(SocketOption::ReuseAddr, &ok);
    EXPECT_TRUE(ok);
    EXPECT_EQ(val.var.i, 1);
    sock.close();
}

TEST(SocketOptionTest, SetAndGetSendBufSize) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18014);
    ASSERT_TRUE(sock.listen(1));

    bool ok = false;
    sock.setOption(SocketOption::SendBufSize, OptionValue(8192), &ok);
    EXPECT_TRUE(ok);

    OptionValue val = sock.option(SocketOption::SendBufSize, &ok);
    EXPECT_TRUE(ok);
    EXPECT_GE(val.var.i, 8192);
    sock.close();
}

TEST(SocketOptionTest, SetAndGetRecvBufSize) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18015);
    ASSERT_TRUE(sock.listen(1));

    bool ok = false;
    sock.setOption(SocketOption::RecvBufSize, OptionValue(8192), &ok);
    EXPECT_TRUE(ok);

    OptionValue val = sock.option(SocketOption::RecvBufSize, &ok);
    EXPECT_TRUE(ok);
    EXPECT_GE(val.var.i, 8192);
    sock.close();
}

// ============================================================================
// Socket Network Operations Tests
// ============================================================================

TEST(SocketNetworkTest, BindToLocalhost) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18001);
    bool result = sock.bind();
    EXPECT_TRUE(result);
    EXPECT_EQ(sock.state(), SocketState::Bound);
    sock.close();
}

TEST(SocketNetworkTest, ListenOnLocalhost) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18002);

    bool listen_result = sock.listen(10);
    EXPECT_TRUE(listen_result);
    EXPECT_EQ(sock.state(), SocketState::Listening);
    sock.close();
}

TEST(SocketNetworkTest, ConnectToServer) {
    // Start a server first
    Socket server_sock;
    server_sock.setLocalAddress("127.0.0.1", 18003);
    ASSERT_TRUE(server_sock.listen(1));
    server_sock.setOption(SocketOption::ReuseAddr, OptionValue(1));

    // Wait for server to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Connect client
    Socket client_sock;
    bool connect_result = client_sock.connect("127.0.0.1", 18003);
    EXPECT_TRUE(connect_result);
    EXPECT_EQ(client_sock.state(), SocketState::Connected);

    client_sock.close();
    server_sock.close();
}

TEST(SocketNetworkTest, SendAndReceive) {
    // Start server
    Socket server_sock;
    server_sock.setLocalAddress("127.0.0.1", 18004);
    ASSERT_TRUE(server_sock.listen(1));
    server_sock.setOption(SocketOption::ReuseAddr, OptionValue(1));

    // Wait for server to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Connect client
    Socket client_sock;
    ASSERT_TRUE(client_sock.connect("127.0.0.1", 18004));
    
    // Accept connection
    bool accept_ok = false;
    Socket accepted_sock = server_sock.accept(&accept_ok);
    ASSERT_TRUE(accept_ok);
    
    // Send from client
    std::string message = "Hello, Server!";
    bool send_ok = client_sock.send(message);
    EXPECT_TRUE(send_ok);
    
    // Give some time for data to arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Receive on server
    std::string received;
    bool recv_ok = accepted_sock.recv(received, 1024);
    EXPECT_TRUE(recv_ok);
    EXPECT_EQ(received, message);
    
    accepted_sock.close();
    client_sock.close();
    server_sock.close();
}

TEST(SocketNetworkTest, CloseSocket) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18005);
    ASSERT_TRUE(sock.bind());

    bool close_result = sock.close();
    EXPECT_TRUE(close_result);
    EXPECT_EQ(sock.state(), SocketState::Unused);
}

TEST(SocketNetworkTest, ShutdownSocket) {
    Socket server_sock;
    server_sock.setLocalAddress("127.0.0.1", 18006);
    ASSERT_TRUE(server_sock.listen(1));
    server_sock.setOption(SocketOption::ReuseAddr, OptionValue(1));

    // Wait for server to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    Socket client_sock;
    ASSERT_TRUE(client_sock.connect("127.0.0.1", 18006));
    
    bool shutdown_result = client_sock.shutdown();
    EXPECT_TRUE(shutdown_result);
    
    client_sock.close();
    server_sock.close();
}

// ============================================================================
// Socket Error Handling Tests
// ============================================================================

TEST(SocketErrorTest, LastErrorInitiallySuccess) {
    Socket sock;
    EXPECT_EQ(sock.lastError(), SocketError::Success);
}

TEST(SocketErrorTest, GetSocketErrorName) {
    const char* name = getSocketErrorName(SocketError::Success);
    EXPECT_STREQ(name, "Tiny::Net::SocketError::Success");
    
    name = getSocketErrorName(SocketError::InvalidParameter);
    EXPECT_STREQ(name, "Tiny::Net::SocketError::InvalidParameter");
    
    name = getSocketErrorName(SocketError::SocketClosed);
    EXPECT_STREQ(name, "Tiny::Net::SocketError::SocketClosed");
}

TEST(SocketErrorTest, NativeErrorNo) {
    Socket sock;
    int err = sock.nativeErrorNo();
    EXPECT_EQ(err, 0); // Initially no error
}

TEST(SocketErrorTest, ErrorSocketOptionID) {
    Socket sock;
    uint32_t opt_id = sock.errorSocketOptionID();
    EXPECT_EQ(opt_id, 0); // Initially no error
}

// ============================================================================
// Utility Function Tests
// ============================================================================

TEST(UtilityTest, ParseFromHostname) {
    bool ok = false;
    int err_cnt = 0;
    auto addresses = parseFromHostname("localhost", &ok, &err_cnt);
    
    EXPECT_TRUE(ok);
    EXPECT_GE(addresses.size(), 1);
}

TEST(UtilityTest, ParseFirstHostname) {
    bool ok = false;
    Address addr = parseFirstHostname("localhost", &ok);
    
    EXPECT_TRUE(ok);
    EXPECT_TRUE(addr.isValid());
}

TEST(UtilityTest, GetLastSystemError) {
    std::string info;
    int err = getLastSystemError(&info);
    // Just check it doesn't crash, error code depends on system state
    EXPECT_GE(err, 0);
}

TEST(UtilityTest, GetSystemErrorByErrno) {
    std::string msg = getSystemErrorByErrno(0);
    // Error 0 typically means "Success" or similar
    EXPECT_FALSE(msg.empty());
}

// ============================================================================
// Port Protocol Tests
// ============================================================================

TEST(PortProtocolTest, CommonPorts) {
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::HTTP), 80);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::HTTPS), 443);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::SSH), 22);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::DNS), 53);
    EXPECT_EQ(static_cast<uint16_t>(PortProtocol::FTP_Con), 21);
}

// ============================================================================
// Socket State Tests
// ============================================================================

TEST(SocketStateTest, InitialState) {
    Socket sock;
    EXPECT_EQ(sock.state(), SocketState::Unused);
}

TEST(SocketStateTest, StateAfterBind) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18007);
    sock.bind();
    EXPECT_EQ(sock.state(), SocketState::Bound);
    sock.close();
}

TEST(SocketStateTest, StateAfterListen) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18008);
    sock.listen(10);
    EXPECT_EQ(sock.state(), SocketState::Listening);
    sock.close();
}

TEST(SocketStateTest, StateAfterClose) {
    Socket sock;
    sock.setLocalAddress("127.0.0.1", 18009);
    sock.bind();
    sock.close();
    EXPECT_EQ(sock.state(), SocketState::Unused);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(IntegrationTest, FullClientServerCommunication) {
    // Setup server
    Socket server;
    server.setLocalAddress("127.0.0.1", 18010);
    ASSERT_TRUE(server.listen(5));
    server.setOption(SocketOption::ReuseAddr, OptionValue(1));
    server.setOption(SocketOption::KeepAlive, OptionValue(1));

    // Wait for server to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Setup client
    Socket client;
    ASSERT_TRUE(client.connect("127.0.0.1", 18010));
    client.setOption(SocketOption::KeepAlive, OptionValue(1));
    
    // Accept connection
    bool accept_ok = false;
    Socket conn = server.accept(&accept_ok);
    ASSERT_TRUE(accept_ok);
    
    // Client sends message
    std::string msg1 = "Request from client";
    ASSERT_TRUE(client.send(msg1));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Server receives message
    std::string recv1;
    ASSERT_TRUE(conn.recv(recv1, 1024));
    EXPECT_EQ(recv1, msg1);
    
    // Server sends response
    std::string msg2 = "Response from server";
    ASSERT_TRUE(conn.send(msg2));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Client receives response
    std::string recv2;
    ASSERT_TRUE(client.recv(recv2, 1024));
    EXPECT_EQ(recv2, msg2);
    
    // Cleanup
    conn.close();
    client.close();
    server.close();
}

TEST(IntegrationTest, MultipleConnections) {
    // Setup server
    Socket server;
    server.setLocalAddress("127.0.0.1", 18011);
    ASSERT_TRUE(server.listen(3));
    server.setOption(SocketOption::ReuseAddr, OptionValue(1));

    // Wait for server to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Connect multiple clients
    Socket client1, client2, client3;
    ASSERT_TRUE(client1.connect("127.0.0.1", 18011));
    ASSERT_TRUE(client2.connect("127.0.0.1", 18011));
    ASSERT_TRUE(client3.connect("127.0.0.1", 18011));
    
    // Accept all connections
    bool ok1 = false, ok2 = false, ok3 = false;
    Socket conn1 = server.accept(&ok1);
    Socket conn2 = server.accept(&ok2);
    Socket conn3 = server.accept(&ok3);
    
    EXPECT_TRUE(ok1);
    EXPECT_TRUE(ok2);
    EXPECT_TRUE(ok3);
    
    // Send from each client
    ASSERT_TRUE(client1.send("Client 1"));
    ASSERT_TRUE(client2.send("Client 2"));
    ASSERT_TRUE(client3.send("Client 3"));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Receive on server
    std::string msg1, msg2, msg3;
    ASSERT_TRUE(conn1.recv(msg1, 1024));
    ASSERT_TRUE(conn2.recv(msg2, 1024));
    ASSERT_TRUE(conn3.recv(msg3, 1024));
    
    EXPECT_EQ(msg1, "Client 1");
    EXPECT_EQ(msg2, "Client 2");
    EXPECT_EQ(msg3, "Client 3");
    
    // Cleanup
    conn1.close();
    conn2.close();
    conn3.close();
    client1.close();
    client2.close();
    client3.close();
    server.close();
}
