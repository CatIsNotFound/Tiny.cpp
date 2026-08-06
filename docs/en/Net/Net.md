# Net::Socket Module

Namespace: `Tiny::Net`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Enum Definitions](#4-enum-definitions)
5. [Address Class](#5-address-class)
6. [OptionValue Struct](#6-optionvalue-struct)
7. [Socket Class](#7-socket-class)
8. [Free Functions](#8-free-functions)
9. [Usage Examples](#9-usage-examples)
10. [Notes](#10-notes)

---

## 1. Module Overview

The `Net` module provides cross-platform network programming functionality, containing core classes for network communication:

- **Address Class**: IPv4/IPv6 address management, supports address parsing, validation, and port configuration
- **Socket Class**: TCP/UDP/SCTP socket operations, supports connection, binding, listening, data transmission, and socket option configuration
- **OptionValue Struct**: Flexible value container for socket options, supports multiple data types
- **PortProtocol Enum**: Common protocol port definitions (HTTP, HTTPS, SSH, FTP, etc.)

The module supports both Windows and Unix-like systems (Linux, macOS) through conditional compilation macros.

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/Net/Socket.hpp>
// Direct source copy method
#include "Net/Socket.hpp"
```

**Platform Macros**:
- `TINY_CPP_MY_OS_WINDOWS` - Enable Windows socket support
- `TINY_CPP_MY_OS_UNIX` - Enable Unix socket support (Linux, macOS, etc.)

If not explicitly defined, the module automatically detects the platform based on `_WIN32`, `_WIN64`, `__APPLE__`, `__linux__`, or `__unix__` macros.

---

## 3. Type Definitions

### 3.1 Handle Type

```cpp
#ifdef TINY_CPP_MY_OS_WINDOWS
    using Handle = uint64_t;
#elif defined(TINY_CPP_MY_OS_UNIX)
    using Handle = int;
#endif
```

**Description**: Platform-specific socket handle type. On Windows, it's a 64-bit unsigned integer (SOCKET type). On Unix systems, it's an integer file descriptor.

### 3.2 Datas Type

```cpp
using Datas = std::vector<char>;
```

**Description**: Byte array type for network data transmission, used for binary send/receive operations.

---

## 4. Enum Definitions

### 4.1 PortProtocol Enum

```cpp
enum class PortProtocol : uint16_t {
    HTTP  = 80,
    HTTPS = 443,
    SOCKS = 1080,
    // ... more protocols
};
```

**Description**: Enumeration of common network protocol port numbers.

| Category | Protocol | Port | Description |
|----------|----------|------|-------------|
| **Web** | `HTTP` | 80 | Hypertext Transfer Protocol |
| | `HTTPS` | 443 | HTTP Secure |
| | `SOCKS` | 1080 | SOCKS proxy protocol |
| **Network Services** | `DHCP_Server` | 67 | DHCP Server |
| | `DHCP_Client` | 68 | DHCP Client |
| | `NTP` | 123 | Network Time Protocol |
| | `SNMP_UDP` | 161 | SNMP over UDP |
| | `SNMP_Trap` | 162 | SNMP Trap |
| | `BGP` | 179 | Border Gateway Protocol |
| **Remote Access** | `SSH` | 22 | Secure Shell |
| | `Telnet` | 23 | Telnet protocol |
| | `RDP` | 3389 | Remote Desktop Protocol |
| | `VNC` | 5900 | Virtual Network Computing |
| **File Transfer** | `FTP_Con` | 21 | FTP Control connection |
| | `FTP_Data` | 20 | FTP Data connection |
| | `FTPS_Con` | 990 | FTP Secure Control |
| | `FTPS_Data` | 989 | FTP Secure Data |
| | `SFTP` | 22 | SSH File Transfer Protocol |
| | `TFTP` | 69 | Trivial File Transfer Protocol |
| | `NFS` | 2049 | Network File System |
| | `SMB` | 445 | Server Message Block |
| **Email** | `SMTP` | 25 | Simple Mail Transfer Protocol |
| | `SMTP_TLS` | 587 | SMTP with TLS |
| | `POP3` | 110 | Post Office Protocol v3 |
| | `POP3S` | 995 | POP3 Secure |
| | `IMAP` | 143 | Internet Message Access Protocol |
| | `IMAPS` | 993 | IMAP Secure |
| **DNS** | `DNS` | 53 | Domain Name System |
| | `DoT` | 853 | DNS over TLS |
| | `DoH` | 443 | DNS over HTTPS |
| | `DoQ` | 853 | DNS over QUIC |
| **Streaming** | `RTMP` | 1935 | Real-Time Messaging Protocol |
| | `RTSP` | 554 | Real Time Streaming Protocol |
| | `SIP` | 5060 | Session Initiation Protocol |
| | `SIPS` | 5061 | SIP Secure |
| **Database** | `MySQL` | 3306 | MySQL Database |
| | `PostgreSQL` | 5432 | PostgreSQL Database |
| | `MongoDB` | 27017 | MongoDB Database |
| | `Redis` | 6379 | Redis Cache |
| | `MS_SQL` | 1433 | Microsoft SQL Server |
| | `Oracle` | 1521 | Oracle Database |
| | `Elasticsearch` | 9200 | Elasticsearch |
| **Messaging** | `MQTT` | 1883 | Message Queuing Telemetry Transport |
| | `MQTTS` | 8883 | MQTT Secure |
| | `AMQP` | 5672 | Advanced Message Queuing Protocol |
| | `Kafka` | 9092 | Apache Kafka |

**Usage Example**:
```cpp
Tiny::Net::Address addr("127.0.0.1", Tiny::Net::PortProtocol::HTTP);
// Equivalent to port 80
```

### 4.2 SocketError Enum

```cpp
enum class SocketError : uint8_t {
    Success,
    InvalidParameter,
    SetOptionError,
    ProtoNotSupported,
    SocketIsNotOpened,
    SocketClosed,
    SocketInUse,
    AddressInUse,
    AddressNotAvailable,
    AddressNotSupport,
    ConnectionRefused,
    ConnectionInProgress,
    ConnectionInUsed,
    ConnectionReset,
    ConnectionAborted,
    ConnectionTimeout,
    NetworkUnreachable,
    HostUnreachable,
    NetworkDown,
    MessageTooLong,
    ResourceUnavailable,
    OperationDenied = 249,
    SystemResourceLimit,
    OperationInProgress,
    OperationCancelled,
    OperationNotSupported,
    SegmentationFault,
    UnknownError
};
```

**Description**: Socket operation error codes.

| Error Code | Description |
|------------|-------------|
| `Success` | Operation successful |
| `InvalidParameter` | Invalid IP address or protocol |
| `SetOptionError` | Failed to set socket option |
| `ProtoNotSupported` | Protocol not supported or socket type mismatch |
| `SocketIsNotOpened` | Socket has not been opened |
| `SocketClosed` | Socket has been closed |
| `SocketInUse` | Socket is already in use |
| `AddressInUse` | Requested address or port is in use |
| `AddressNotAvailable` | Address is not available |
| `AddressNotSupport` | Address not supported (host not found or not UP) |
| `ConnectionRefused` | Connection refused by remote host |
| `ConnectionInProgress` | Connection attempt in progress |
| `ConnectionInUsed` | Connection already in use |
| `ConnectionReset` | Connection reset by peer |
| `ConnectionAborted` | Connection aborted |
| `ConnectionTimeout` | Connection attempt timed out |
| `NetworkUnreachable` | Network is unreachable |
| `HostUnreachable` | Host is unreachable |
| `NetworkDown` | Network interface is down |
| `MessageTooLong` | Received message is too long |
| `ResourceUnavailable` | Resource temporarily unavailable |
| `OperationDenied` | Permission denied (error code 249) |
| `SystemResourceLimit` | OS resource limit exceeded |
| `OperationInProgress` | Operation in progress |
| `OperationCancelled` | Operation manually cancelled by user |
| `OperationNotSupported` | Operation not supported (e.g., accept on non-listening socket) |
| `SegmentationFault` | Segmentation fault occurred |
| `UnknownError` | Unknown error |

### 4.3 SocketType Enum

```cpp
enum class SocketType : uint8_t {
    TCP,
    UDP,
    SCTP,
    Custom
};
```

**Description**: Socket protocol type.

| Type | Description |
|------|-------------|
| `TCP` | Transmission Control Protocol (reliable, connection-oriented) |
| `UDP` | User Datagram Protocol (unreliable, connectionless) |
| `SCTP` | Stream Control Transmission Protocol |
| `Custom` | Custom protocol type |

### 4.4 SocketState Enum

```cpp
enum class SocketState : uint8_t {
    Unused,
    ParsingAddress,
    Connecting,
    Connected,
    Bound,
    Listening,
    Closing
};
```

**Description**: Socket lifecycle state.

| State | Description |
|-------|-------------|
| `Unused` | Socket not initialized or closed |
| `ParsingAddress` | Parsing address in progress |
| `Connecting` | Connection attempt in progress |
| `Connected` | Successfully connected to peer |
| `Bound` | Socket bound to local address |
| `Listening` | Socket listening for incoming connections |
| `Closing` | Socket closing in progress |

### 4.5 SocketOption Enum

```cpp
enum class SocketOption : uint8_t {
    AllowedBroadcast = 1,
    DontRoute,
    KeepAlive,
    NoDelay,
    SendBufSize,
    RecvBufSize,
    SendBufTimeout,
    RecvBufTimeout,
    Linger,
    ReuseAddr,
    MapIPv6Only,
    MulticastTTL,
    MulticastLoopback,
    NonBlocking,
    NativeSocketError
};
```

**Description**: Socket configuration options.

| Option | Value Type | Support | Description |
|--------|------------|---------|-------------|
| `AllowedBroadcast` | int (bool) | Set/Get | Enable send/receive broadcast (IPv4 only) |
| `DontRoute` | int (bool) | Set/Get | Bypass routing table, send directly to network interface |
| `KeepAlive` | int (bool) | Set/Get | Enable TCP keepalive detection |
| `NoDelay` | int (bool) | Set/Get | Disable Nagle algorithm for low latency (TCP only) |
| `SendBufSize` | int | Set/Get | Maximum send buffer size |
| `RecvBufSize` | int | Set/Get | Maximum receive buffer size |
| `SendBufTimeout` | Platform-specific | Set/Get | Send buffer timeout (Windows: uint32_t ms, Others: timeval*) |
| `RecvBufTimeout` | Platform-specific | Set/Get | Receive buffer timeout (Windows: uint32_t ms, Others: timeval*) |
| `Linger` | linger* | Set/Get | Control how socket closes gracefully |
| `ReuseAddr` | int (bool) | Set/Get | Allow reuse of local address |
| `MapIPv6Only` | int (bool) | Set/Get | Disable IPv4-mapped IPv6 addresses |
| `MulticastTTL` | int | Set/Get | Multicast time-to-live |
| `MulticastLoopback` | int (bool) | Set/Get | Enable multicast loopback |
| `NonBlocking` | int (bool) | Set | Enable non-blocking mode |
| `NativeSocketError` | int | Get | Get native socket error code |

---

## 5. Address Class

### 5.1 Class Overview

IPv4/IPv6 address management class. Supports address parsing, validation, port configuration, and address family conversion.

### 5.2 Constructors

```cpp
Address();                                                      // Default constructor
Address(const char* address, uint16_t port, bool use_ipv6 = false);
Address(const char* address, PortProtocol protocol_num, bool use_ipv6 = false);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `const char*` | IP address string (e.g., "127.0.0.1" or "::1") |
| `port` | `uint16_t` | Port number (0-65535) |
| `protocol_num` | `PortProtocol` | Protocol enum value (automatically converted to port) |
| `use_ipv6` | `bool` | Use IPv6 address family (default: false for IPv4) |

**Example**:
```cpp
Tiny::Net::Address addr1("192.168.1.1", 8080);
Tiny::Net::Address addr2("::1", Tiny::Net::PortProtocol::HTTP, true);
```

### 5.3 Destructor

```cpp
~Address();
```

### 5.4 Copy Control

```cpp
Address(const Address&) = delete;              // Copy prohibited
Address& operator=(const Address&) = delete;   // Copy assignment prohibited
Address(Address&& addr) noexcept;              // Move constructor
Address& operator=(Address&& addr) noexcept;   // Move assignment
```

### 5.5 Comparison Operators

```cpp
bool operator==(const Address& addr) const;
bool operator!=(const Address& addr) const;
```

### 5.6 Member Functions

#### toString

```cpp
std::string toString(bool* ok = nullptr) const;
```

- **Function**: Convert address to string representation
- **Parameter**: `ok` - Optional pointer to receive success status
- **Return Value**: Address string (e.g., "192.168.1.1:8080")
- **Example**:
```cpp
bool success;
std::string str = addr.toString(&success);
if (success) {
    std::cout << "Address: " << str << std::endl;
}
```

#### address

```cpp
void* address() const;
```

- **Function**: Get raw address structure pointer
- **Return Value**: Pointer to internal address structure (sockaddr_in or sockaddr_in6)

#### port

```cpp
uint16_t port() const;
```

- **Function**: Get port number
- **Return Value**: Port number (0-65535)

#### isValid

```cpp
bool isValid() const;
```

- **Function**: Check if address is valid
- **Return Value**: `true` if address was successfully parsed

#### isIPv6

```cpp
bool isIPv6() const;
```

- **Function**: Check if address is IPv6
- **Return Value**: `true` if IPv6 address family

#### setAddress

```cpp
void setAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setAddress(const char* address, PortProtocol port, bool use_ipv6 = false);
```

- **Function**: Set address and port
- **Parameters**: Same as constructor

#### setPort

```cpp
void setPort(uint16_t port);
void setPort(PortProtocol protocol_num);
```

- **Function**: Set port number
- **Parameter**: Port number or protocol enum

### 5.7 Static Member Functions

#### localHost

```cpp
static Address localHost();
```

- **Function**: Get localhost IPv4 address (127.0.0.1)
- **Return Value**: Address object with port UINT16_MAX

#### localHostIPv6

```cpp
static Address localHostIPv6();
```

- **Function**: Get localhost IPv6 address (::1)
- **Return Value**: Address object with port UINT16_MAX

---

## 6. OptionValue Struct

### 6.1 Struct Overview

Flexible value container for socket options. Supports multiple data types through a union and type-safe access.

### 6.2 ValueType Enum

```cpp
enum ValueType : uint8_t {
    None,
    String,
    Int,
    UInt,
    Float,
    Custom
};
```

### 6.3 Members

```cpp
ValueType type;              // Current value type
int size;                    // Size of data (sizeof)
union Value {
    int         i;           // Integer value
    uint32_t    u;           // Unsigned integer value
    float       f;           // Floating point value
    char*       s;           // String value
    void*       v;           // Custom pointer value
} var;                       // Stored variable
```

### 6.4 Constructors

```cpp
OptionValue();                           // None type
OptionValue(int v);                      // Int type
OptionValue(uint32_t v);                 // UInt type
OptionValue(float v);                    // Float type
OptionValue(const char* v);              // String type
OptionValue(void* v, uint32_t size);     // Custom type
```

### 6.5 Member Functions

#### set

```cpp
void set(int v);
void set(uint32_t v);
void set(float v);
void set(const char* v);
void set(void* v, uint32_t len);
```

- **Function**: Set value with specified type
- **Parameter**: Value to set

#### unset

```cpp
void unset();
```

- **Function**: Clear value (set to None type)

#### Assignment Operators

```cpp
OptionValue& operator=(int v);
OptionValue& operator=(uint32_t v);
OptionValue& operator=(float v);
OptionValue& operator=(char* v);
OptionValue& operator=(std::nullptr_t);
OptionValue& operator=(const OptionValue& other);
```

#### Comparison Operators

```cpp
bool operator==(const OptionValue& other) const;
bool operator!=(const OptionValue& other) const;
```

---

## 7. Socket Class

### 7.1 Class Overview

Cross-platform socket operation class. Supports TCP/UDP/SCTP protocols, connection management, data transmission, and socket option configuration.

### 7.2 Constructors

```cpp
Socket(SocketType type = SocketType::TCP);
Socket(Socket&& other) noexcept;
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `SocketType` | Socket protocol type (default: TCP) |

### 7.3 Destructor

```cpp
~Socket();
```

- Automatically closes socket handle

### 7.4 Copy Control

```cpp
Socket(const Socket&) = delete;              // Copy prohibited
Socket& operator=(const Socket&) = delete;   // Copy assignment prohibited
Socket& operator=(Socket&& other) noexcept;  // Move assignment allowed
```

### 7.5 Configuration Methods

#### setLocalAddress

```cpp
void setLocalAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setLocalAddress(Address&& address);
```

- **Function**: Set local bind address
- **Parameters**: Address and port information

#### setLocalPort

```cpp
void setLocalPort(uint16_t port);
```

- **Function**: Set local port only

#### setPeerAddress

```cpp
void setPeerAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setPeerAddress(Address&& address);
```

- **Function**: Set remote peer address

#### setPeerPort

```cpp
void setPeerPort(uint16_t port);
```

- **Function**: Set remote peer port only

#### setSocketType

```cpp
void setSocketType(SocketType type);
```

- **Function**: Set socket protocol type

### 7.6 Connection Methods

#### connect

```cpp
bool connect(const char* address, uint16_t port);
bool connect(const char* address, PortProtocol port);
bool connect(Address&& address);
bool connect();
```

- **Function**: Connect to remote peer
- **Parameters**: Remote address and port
- **Return Value**: `true` if connection successful
- **Notes**: Uses previously set peer address if called without parameters

#### bind

```cpp
bool bind(const char* address, uint16_t port);
bool bind(const char* address, PortProtocol port);
bool bind(Address&& address);
bool bind();
```

- **Function**: Bind socket to local address
- **Parameters**: Local address and port
- **Return Value**: `true` if binding successful
- **Notes**: Uses previously set local address if called without parameters

#### listen

```cpp
bool listen(uint16_t port, int max_connection_count);
bool listen(PortProtocol protocol_num, int max_connection_count);
bool listen(int max_connection_count);
```

- **Function**: Start listening for incoming connections
- **Parameters**:
  - `port` / `protocol_num` - Local port to listen on
  - `max_connection_count` - Maximum pending connection queue size
- **Return Value**: `true` if listening successful
- **Notes**: Uses previously set local port if called with only max_connection_count

#### accept

```cpp
bool accept(Socket& socket);
Socket accept(bool* ok = nullptr);
```

- **Function**: Accept incoming connection
- **Parameters**:
  - `socket` - Socket object to receive accepted connection
  - `ok` - Optional pointer to receive success status
- **Return Value**: New Socket object for accepted connection (overload 2)
- **Notes**: Socket must be in Listening state

#### close

```cpp
bool close();
```

- **Function**: Close socket connection
- **Return Value**: `true` if successful

#### shutdown

```cpp
bool shutdown();
```

- **Function**: Shutdown socket (graceful close)
- **Return Value**: `true` if successful

### 7.7 Data Transmission Methods

#### send

```cpp
bool send(const std::string& message, int* sended_length = nullptr);
bool send(const Datas& data, int* sended_length = nullptr);
```

- **Function**: Send data to connected peer
- **Parameters**:
  - `message` / `data` - Data to send
  - `sended_length` - Optional pointer to receive actual bytes sent
- **Return Value**: `true` if send successful
- **Notes**: Socket must be connected

#### recv

```cpp
bool recv(Datas& data, size_t max_length, int* received_length = nullptr);
bool recv(std::string& message, size_t max_length, int* received_length = nullptr);
```

- **Function**: Receive data from connected peer
- **Parameters**:
  - `data` / `message` - Buffer to receive data
  - `max_length` - Maximum bytes to receive
  - `received_length` - Optional pointer to receive actual bytes received
- **Return Value**: `true` if receive successful

#### sendTo

```cpp
bool sendTo(const std::string& message, const Address& address, int* sended_length = nullptr);
bool sendTo(const Datas& message, const Address& address, int* sended_length = nullptr);
```

- **Function**: Send data to specific address (for UDP)
- **Parameters**:
  - `message` - Data to send
  - `address` - Destination address
  - `sended_length` - Optional pointer to receive actual bytes sent
- **Return Value**: `true` if send successful

#### recvFrom

```cpp
bool recvFrom(std::string& message, size_t max_length, const Address& address, int* received_length = nullptr);
bool recvFrom(Datas& data, size_t max_length, const Address& address, int* received_length = nullptr);
```

- **Function**: Receive data from specific address (for UDP)
- **Parameters**:
  - `message` / `data` - Buffer to receive data
  - `max_length` - Maximum bytes to receive
  - `address` - Source address
  - `received_length` - Optional pointer to receive actual bytes received
- **Return Value**: `true` if receive successful

### 7.8 Socket Option Methods

#### setOption

```cpp
void setOption(SocketOption option, OptionValue value, bool* ok = nullptr);
void setOption(uint32_t option_id, OptionValue value, bool* ok = nullptr);
```

- **Function**: Set socket option
- **Parameters**:
  - `option` / `option_id` - Option enum or custom option ID
  - `value` - Option value
  - `ok` - Optional pointer to receive success status
- **Example**:
```cpp
Tiny::Net::Socket socket;
socket.setOption(Tiny::Net::SocketOption::ReuseAddr, 1);
socket.setOption(Tiny::Net::SocketOption::KeepAlive, 1);
```

#### option

```cpp
OptionValue option(SocketOption option, bool* ok = nullptr) const;
OptionValue option(uint32_t option_id, bool* ok = nullptr) const;
```

- **Function**: Get socket option value
- **Parameters**:
  - `option` / `option_id` - Option enum or custom option ID
  - `ok` - Optional pointer to receive success status
- **Return Value**: Option value

### 7.9 Query Methods

#### localAddress

```cpp
const Address& localAddress() const;
```

- **Function**: Get local bind address
- **Return Value**: Reference to local Address object

#### peerAddress

```cpp
const Address& peerAddress() const;
```

- **Function**: Get remote peer address
- **Return Value**: Reference to peer Address object

#### lastError

```cpp
SocketError lastError() const;
```

- **Function**: Get last error code
- **Return Value**: SocketError enum value

#### type

```cpp
SocketType type() const;
```

- **Function**: Get socket type
- **Return Value**: SocketType enum value

#### state

```cpp
SocketState state() const;
```

- **Function**: Get current socket state
- **Return Value**: SocketState enum value

#### errorSocketOptionID

```cpp
uint32_t errorSocketOptionID() const;
```

- **Function**: Get option ID that caused error
- **Return Value**: Option ID (0 if no option error)

#### nativeErrorNo

```cpp
int nativeErrorNo() const;
```

- **Function**: Get native system error code
- **Return Value**: System errno value

---

## 8. Free Functions

### 8.1 parseFromHostname

```cpp
std::vector<Address> parseFromHostname(const char* hostname, bool* ok = nullptr, int* err_cnt = nullptr);
```

- **Function**: Resolve hostname to multiple IP addresses
- **Parameters**:
  - `hostname` - Hostname to resolve (e.g., "www.example.com")
  - `ok` - Optional pointer to receive success status
  - `err_cnt` - Optional pointer to receive error count
- **Return Value**: Vector of Address objects (all resolved addresses)
- **Example**:
```cpp
bool success;
auto addresses = Tiny::Net::parseFromHostname("www.google.com", &success);
if (success) {
    for (const auto& addr : addresses) {
        std::cout << "Resolved: " << addr.toString() << std::endl;
    }
}
```

### 8.2 parseFirstHostname

```cpp
Address parseFirstHostname(const char* hostname, bool* ok = nullptr);
```

- **Function**: Resolve hostname to first IP address
- **Parameters**:
  - `hostname` - Hostname to resolve
  - `ok` - Optional pointer to receive success status
- **Return Value**: First resolved Address object
- **Example**:
```cpp
bool success;
auto addr = Tiny::Net::parseFirstHostname("www.google.com", &success);
if (success) {
    std::cout << "First IP: " << addr.toString() << std::endl;
}
```

### 8.3 getLastSystemError

```cpp
int getLastSystemError(std::string* info = nullptr);
```

- **Function**: Get last system error code
- **Parameter**: `info` - Optional pointer to receive error description
- **Return Value**: System error code (errno on Unix, WSAGetLastError on Windows)
- **Example**:
```cpp
std::string error_info;
int err = Tiny::Net::getLastSystemError(&error_info);
std::cout << "Error " << err << ": " << error_info << std::endl;
```

### 8.4 getSystemErrorByErrno

```cpp
std::string getSystemErrorByErrno(int err_no);
```

- **Function**: Get error description by error code
- **Parameter**: `err_no` - System error code
- **Return Value**: Error description string
- **Example**:
```cpp
std::string desc = Tiny::Net::getSystemErrorByErrno(10048);
// Returns: "Address already in use" (Windows)
```

### 8.5 getSocketErrorName

```cpp
const char* getSocketErrorName(SocketError err);
```

- **Function**: Get error name string
- **Parameter**: `err` - SocketError enum value
- **Return Value**: Error name string (e.g., "Success", "ConnectionRefused")
- **Example**:
```cpp
const char* name = Tiny::Net::getSocketErrorName(socket.lastError());
std::cout << "Error: " << name << std::endl;
```

---

## 9. Usage Examples

### 9.1 TCP Client Example

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // Create TCP socket
    Tiny::Net::Socket socket(Tiny::Net::SocketType::TCP);
    
    // Connect to server
    if (socket.connect("127.0.0.1", 8080)) {
        std::cout << "Connected to server" << std::endl;
        
        // Send message
        std::string message = "Hello, Server!";
        if (socket.send(message)) {
            std::cout << "Sent: " << message << std::endl;
        }
        
        // Receive response
        std::string response;
        if (socket.recv(response, 1024)) {
            std::cout << "Received: " << response << std::endl;
        }
        
        // Close connection
        socket.close();
    } else {
        std::cout << "Connection failed: " 
                  << Tiny::Net::getSocketErrorName(socket.lastError()) << std::endl;
    }
    
    return 0;
}
```

### 9.2 TCP Server Example

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // Create server socket
    Tiny::Net::Socket server(Tiny::Net::SocketType::TCP);
    
    // Enable address reuse
    server.setOption(Tiny::Net::SocketOption::ReuseAddr, 1);
    
    // Bind and listen
    if (server.bind("0.0.0.0", 8080) && server.listen(8080, 10)) {
        std::cout << "Server listening on port 8080" << std::endl;
        
        // Accept client connection
        Tiny::Net::Socket client = server.accept();
        if (client.state() == Tiny::Net::SocketState::Connected) {
            std::cout << "Client connected" << std::endl;
            
            // Receive message
            std::string message;
            if (client.recv(message, 1024)) {
                std::cout << "Received: " << message << std::endl;
                
                // Send response
                std::string response = "Hello, Client!";
                client.send(response);
            }
            
            client.close();
        }
        
        server.close();
    }
    
    return 0;
}
```

### 9.3 UDP Communication Example

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // Create UDP socket
    Tiny::Net::Socket socket(Tiny::Net::SocketType::UDP);
    
    // Bind to local port
    socket.bind("0.0.0.0", 9000);
    
    // Send data to specific address
    Tiny::Net::Address dest("127.0.0.1", 9001);
    std::string message = "UDP Message";
    socket.sendTo(message, dest);
    
    // Receive data
    std::string buffer;
    Tiny::Net::Address sender;
    if (socket.recvFrom(buffer, 1024, sender)) {
        std::cout << "Received from " << sender.toString() 
                  << ": " << buffer << std::endl;
    }
    
    socket.close();
    return 0;
}
```

### 9.4 Hostname Resolution Example

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // Resolve hostname
    bool success;
    auto addresses = Tiny::Net::parseFromHostname("www.example.com", &success);
    
    if (success && !addresses.empty()) {
        std::cout << "Resolved addresses:" << std::endl;
        for (size_t i = 0; i < addresses.size(); ++i) {
            std::cout << i + 1 << ". " << addresses[i].toString() << std::endl;
        }
    } else {
        std::cout << "Failed to resolve hostname" << std::endl;
        int err = Tiny::Net::getLastSystemError();
        std::cout << "System error: " << err << std::endl;
    }
    
    return 0;
}
```

### 9.5 Socket Options Configuration

```cpp
#include "Net/Socket.hpp"

int main() {
    Tiny::Net::Socket socket(Tiny::Net::SocketType::TCP);
    
    // Set multiple options before connection
    socket.setOption(Tiny::Net::SocketOption::ReuseAddr, 1);
    socket.setOption(Tiny::Net::SocketOption::KeepAlive, 1);
    socket.setOption(Tiny::Net::SocketOption::NoDelay, 1);
    socket.setOption(Tiny::Net::SocketOption::SendBufSize, 65536);
    socket.setOption(Tiny::Net::SocketOption::RecvBufSize, 65536);
    
    // Connect with options applied
    socket.connect("example.com", Tiny::Net::PortProtocol::HTTP);
    
    // Get option value
    bool ok;
    auto keepalive = socket.option(Tiny::Net::SocketOption::KeepAlive, &ok);
    if (ok && keepalive.var.i == 1) {
        // Keepalive is enabled
    }
    
    socket.close();
    return 0;
}
```

---

## 10. Notes

### 10.1 Platform Compatibility

- **Windows**: Requires Winsock2 library (automatically linked)
- **Unix/Linux/macOS**: Uses BSD socket API
- Socket handle type differs: `uint64_t` on Windows, `int` on Unix
- Timeout options use different types: `uint32_t` (milliseconds) on Windows, `timeval*` on Unix

### 10.2 Socket Lifecycle

1. **Creation**: Socket starts in `Unused` state
2. **Configuration**: Set local/peer addresses and options
3. **Connection**: Call `connect()` (client) or `bind()` + `listen()` + `accept()` (server)
4. **Communication**: Use `send()`/`recv()` for TCP, `sendTo()`/`recvFrom()` for UDP
5. **Closure**: Call `close()` or let destructor handle it

### 10.3 Error Handling

- All operations return `bool` indicating success/failure
- Use `lastError()` to get detailed error code
- Use `nativeErrorNo()` to get system-specific error code
- Use `getSocketErrorName()` to convert error to readable string

### 10.4 Address Family

- IPv4: Use `use_ipv6 = false` (default)
- IPv6: Use `use_ipv6 = true`
- Address class automatically handles address family conversion

### 10.5 Thread Safety

- Socket objects are **not thread-safe**
- Each socket should be used in a single thread
- For multi-threaded servers, create separate socket per connection

### 10.6 Resource Management

- Socket class prohibits copying, use move semantics for transfer
- Destructor automatically closes socket handle
- Explicitly call `close()` to release resources early
- Use RAII pattern for automatic resource management

### 10.7 Common Pitfalls

- **Port already in use**: Set `ReuseAddr` option before binding
- **Connection refused**: Ensure server is listening and firewall allows connection
- **Address already in use**: Check if another process is using the same port
- **Network unreachable**: Verify network interface is up and routing is correct
- **Blocking operations**: Use `NonBlocking` option for asynchronous operations
