# Net::Socket 模块

命名空间: `Tiny::Net`

---

## 目录

1. [模块概述](#1-模块概述)
2. [头文件](#2-头文件)
3. [类型定义](#3-类型定义)
4. [枚举定义](#4-枚举定义)
5. [Address 类](#5-address-类)
6. [OptionValue 结构体](#6-optionvalue-结构体)
7. [Socket 类](#7-socket-类)
8. [自由函数](#8-自由函数)
9. [使用示例](#9-使用示例)
10. [注意事项](#10-注意事项)

---

## 1. 模块概述

`Net` 模块提供跨平台的网络编程功能，包含网络通信的核心类：

- **Address 类**: IPv4/IPv6 地址管理，支持地址解析、验证和端口配置
- **Socket 类**: TCP/UDP/SCTP Socket 操作，支持连接、绑定、监听、数据传输和 Socket 选项配置
- **OptionValue 结构体**: 灵活的 Socket 选项值容器，支持多种数据类型
- **PortProtocol 枚举**: 常用协议端口定义（HTTP、HTTPS、SSH、FTP 等）

该模块通过条件编译宏同时支持 Windows 和类 Unix 系统（Linux、macOS）。

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/Net/Socket.hpp>
// 直接源码复制方式
#include "Net/Socket.hpp"
```

**平台宏定义**:
- `TINY_CPP_MY_OS_WINDOWS` - 启用 Windows Socket 支持
- `TINY_CPP_MY_OS_UNIX` - 启用 Unix Socket 支持（Linux、macOS 等）

如果未显式定义，模块会根据 `_WIN32`、`_WIN64`、`__APPLE__`、`__linux__` 或 `__unix__` 宏自动检测平台。

---

## 3. 类型定义

### 3.1 Handle 类型

```cpp
#ifdef TINY_CPP_MY_OS_WINDOWS
    using Handle = uint64_t;
#elif defined(TINY_CPP_MY_OS_UNIX)
    using Handle = int;
#endif
```

**描述**: 平台特定的 Socket 句柄类型。在 Windows 上是 64 位无符号整数（SOCKET 类型）。在 Unix 系统上是整数文件描述符。

### 3.2 Datas 类型

```cpp
using Datas = std::vector<char>;
```

**描述**: 用于网络数据传输的字节数组类型，用于二进制发送/接收操作。

---

## 4. 枚举定义

### 4.1 PortProtocol 枚举

```cpp
enum class PortProtocol : uint16_t {
    HTTP  = 80,
    HTTPS = 443,
    SOCKS = 1080,
    // ... 更多协议
};
```

**描述**: 常用网络协议端口号枚举。

| 分类 | 协议 | 端口 | 描述 |
|------|------|------|------|
| **Web** | `HTTP` | 80 | 超文本传输协议 |
| | `HTTPS` | 443 | 安全超文本传输协议 |
| | `SOCKS` | 1080 | SOCKS 代理协议 |
| **网络服务** | `DHCP_Server` | 67 | DHCP 服务器 |
| | `DHCP_Client` | 68 | DHCP 客户端 |
| | `NTP` | 123 | 网络时间协议 |
| | `SNMP_UDP` | 161 | SNMP over UDP |
| | `SNMP_Trap` | 162 | SNMP Trap |
| | `BGP` | 179 | 边界网关协议 |
| **远程访问** | `SSH` | 22 | 安全 Shell |
| | `Telnet` | 23 | Telnet 协议 |
| | `RDP` | 3389 | 远程桌面协议 |
| | `VNC` | 5900 | 虚拟网络计算 |
| **文件传输** | `FTP_Con` | 21 | FTP 控制连接 |
| | `FTP_Data` | 20 | FTP 数据连接 |
| | `FTPS_Con` | 990 | FTP 安全控制 |
| | `FTPS_Data` | 989 | FTP 安全数据 |
| | `SFTP` | 22 | SSH 文件传输协议 |
| | `TFTP` | 69 | 简单文件传输协议 |
| | `NFS` | 2049 | 网络文件系统 |
| | `SMB` | 445 | 服务器消息块 |
| **邮件** | `SMTP` | 25 | 简单邮件传输协议 |
| | `SMTP_TLS` | 587 | 带 TLS 的 SMTP |
| | `POP3` | 110 | 邮局协议 v3 |
| | `POP3S` | 995 | 安全 POP3 |
| | `IMAP` | 143 | 互联网消息访问协议 |
| | `IMAPS` | 993 | 安全 IMAP |
| **DNS** | `DNS` | 53 | 域名系统 |
| | `DoT` | 853 | DNS over TLS |
| | `DoH` | 443 | DNS over HTTPS |
| | `DoQ` | 853 | DNS over QUIC |
| **流媒体** | `RTMP` | 1935 | 实时消息协议 |
| | `RTSP` | 554 | 实时流协议 |
| | `SIP` | 5060 | 会话发起协议 |
| | `SIPS` | 5061 | 安全 SIP |
| **数据库** | `MySQL` | 3306 | MySQL 数据库 |
| | `PostgreSQL` | 5432 | PostgreSQL 数据库 |
| | `MongoDB` | 27017 | MongoDB 数据库 |
| | `Redis` | 6379 | Redis 缓存 |
| | `MS_SQL` | 1433 | Microsoft SQL Server |
| | `Oracle` | 1521 | Oracle 数据库 |
| | `Elasticsearch` | 9200 | Elasticsearch |
| **消息队列** | `MQTT` | 1883 | 消息队列遥测传输协议 |
| | `MQTTS` | 8883 | 安全 MQTT |
| | `AMQP` | 5672 | 高级消息队列协议 |
| | `Kafka` | 9092 | Apache Kafka |

**使用示例**:
```cpp
Tiny::Net::Address addr("127.0.0.1", Tiny::Net::PortProtocol::HTTP);
// 等同于端口 80
```

### 4.2 SocketError 枚举

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

**描述**: Socket 操作错误码。

| 错误码 | 描述 |
|--------|------|
| `Success` | 操作成功 |
| `InvalidParameter` | 无效的 IP 地址或协议 |
| `SetOptionError` | 设置 Socket 选项失败 |
| `ProtoNotSupported` | 不支持的协议或 Socket 类型不匹配 |
| `SocketIsNotOpened` | Socket 未打开 |
| `SocketClosed` | Socket 已关闭 |
| `SocketInUse` | Socket 已在使用中 |
| `AddressInUse` | 请求的地址或端口正在被使用 |
| `AddressNotAvailable` | 地址不可用 |
| `AddressNotSupport` | 不支持的地址（主机未找到或未启动） |
| `ConnectionRefused` | 连接被远程主机拒绝 |
| `ConnectionInProgress` | 连接尝试进行中 |
| `ConnectionInUsed` | 连接已在使用中 |
| `ConnectionReset` | 连接被对端重置 |
| `ConnectionAborted` | 连接中止 |
| `ConnectionTimeout` | 连接尝试超时 |
| `NetworkUnreachable` | 网络不可达 |
| `HostUnreachable` | 主机不可达 |
| `NetworkDown` | 网络接口已关闭 |
| `MessageTooLong` | 接收的消息过长 |
| `ResourceUnavailable` | 资源暂时不可用 |
| `OperationDenied` | 权限被拒绝（错误码 249） |
| `SystemResourceLimit` | 操作系统资源限制超出 |
| `OperationInProgress` | 操作进行中 |
| `OperationCancelled` | 用户手动取消操作 |
| `OperationNotSupported` | 不支持的操作（例如：在非监听 Socket 上调用 accept） |
| `SegmentationFault` | 发生段错误 |
| `UnknownError` | 未知错误 |

### 4.3 SocketType 枚举

```cpp
enum class SocketType : uint8_t {
    TCP,
    UDP,
    SCTP,
    Custom
};
```

**描述**: Socket 协议类型。

| 类型 | 描述 |
|------|------|
| `TCP` | 传输控制协议（可靠、面向连接） |
| `UDP` | 用户数据报协议（不可靠、无连接） |
| `SCTP` | 流控制传输协议 |
| `Custom` | 自定义协议类型 |

### 4.4 SocketState 枚举

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

**描述**: Socket 生命周期状态。

| 状态 | 描述 |
|------|------|
| `Unused` | Socket 未初始化或已关闭 |
| `ParsingAddress` | 地址解析中 |
| `Connecting` | 连接尝试进行中 |
| `Connected` | 已成功连接到对端 |
| `Bound` | Socket 已绑定到本地地址 |
| `Listening` | Socket 正在监听传入连接 |
| `Closing` | Socket 关闭中 |

### 4.5 SocketOption 枚举

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

**描述**: Socket 配置选项。

| 选项 | 值类型 | 支持 | 描述 |
|------|--------|------|------|
| `AllowedBroadcast` | int (bool) | 设置/获取 | 启用发送/接收广播（仅限 IPv4） |
| `DontRoute` | int (bool) | 设置/获取 | 绕过路由表，直接发送到网络接口 |
| `KeepAlive` | int (bool) | 设置/获取 | 启用 TCP 保活检测 |
| `NoDelay` | int (bool) | 设置/获取 | 禁用 Nagle 算法以降低延迟（仅限 TCP） |
| `SendBufSize` | int | 设置/获取 | 最大发送缓冲区大小 |
| `RecvBufSize` | int | 设置/获取 | 最大接收缓冲区大小 |
| `SendBufTimeout` | 平台相关 | 设置/获取 | 发送缓冲区超时（Windows: uint32_t 毫秒，其他: timeval*） |
| `RecvBufTimeout` | 平台相关 | 设置/获取 | 接收缓冲区超时（Windows: uint32_t 毫秒，其他: timeval*） |
| `Linger` | linger* | 设置/获取 | 控制 Socket 如何优雅关闭 |
| `ReuseAddr` | int (bool) | 设置/获取 | 允许重用本地地址 |
| `MapIPv6Only` | int (bool) | 设置/获取 | 禁用 IPv4 映射的 IPv6 地址 |
| `MulticastTTL` | int | 设置/获取 | 多播生存时间 |
| `MulticastLoopback` | int (bool) | 设置/获取 | 启用多播环回 |
| `NonBlocking` | int (bool) | 设置 | 启用非阻塞模式 |
| `NativeSocketError` | int | 获取 | 获取原生 Socket 错误码 |

---

## 5. Address 类

### 5.1 类概述

IPv4/IPv6 地址管理类。支持地址解析、验证、端口配置和地址族转换。

### 5.2 构造函数

```cpp
Address();                                                      // 默认构造函数
Address(const char* address, uint16_t port, bool use_ipv6 = false);
Address(const char* address, PortProtocol protocol_num, bool use_ipv6 = false);
```

| 参数 | 类型 | 描述 |
|------|------|------|
| `address` | `const char*` | IP 地址字符串（例如："127.0.0.1" 或 "::1"） |
| `port` | `uint16_t` | 端口号（0-65535） |
| `protocol_num` | `PortProtocol` | 协议枚举值（自动转换为端口） |
| `use_ipv6` | `bool` | 使用 IPv6 地址族（默认：false 表示 IPv4） |

**示例**:
```cpp
Tiny::Net::Address addr1("192.168.1.1", 8080);
Tiny::Net::Address addr2("::1", Tiny::Net::PortProtocol::HTTP, true);
```

### 5.3 析构函数

```cpp
~Address();
```

### 5.4 拷贝控制

```cpp
Address(const Address&) = delete;              // 禁止拷贝
Address& operator=(const Address&) = delete;   // 禁止拷贝赋值
Address(Address&& addr) noexcept;              // 移动构造函数
Address& operator=(Address&& addr) noexcept;   // 移动赋值
```

### 5.5 比较运算符

```cpp
bool operator==(const Address& addr) const;
bool operator!=(const Address& addr) const;
```

### 5.6 成员函数

#### toString

```cpp
std::string toString(bool* ok = nullptr) const;
```

- **功能**: 将地址转换为字符串表示
- **参数**: `ok` - 可选指针，用于接收成功状态
- **返回值**: 地址字符串（例如："192.168.1.1:8080"）
- **示例**:
```cpp
bool success;
std::string str = addr.toString(&success);
if (success) {
    std::cout << "地址: " << str << std::endl;
}
```

#### address

```cpp
void* address() const;
```

- **功能**: 获取原始地址结构指针
- **返回值**: 指向内部地址结构的指针（sockaddr_in 或 sockaddr_in6）

#### port

```cpp
uint16_t port() const;
```

- **功能**: 获取端口号
- **返回值**: 端口号（0-65535）

#### isValid

```cpp
bool isValid() const;
```

- **功能**: 检查地址是否有效
- **返回值**: 如果地址成功解析则返回 `true`

#### isIPv6

```cpp
bool isIPv6() const;
```

- **功能**: 检查地址是否为 IPv6
- **返回值**: 如果是 IPv6 地址族则返回 `true`

#### setAddress

```cpp
void setAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setAddress(const char* address, PortProtocol port, bool use_ipv6 = false);
```

- **功能**: 设置地址和端口
- **参数**: 与构造函数相同

#### setPort

```cpp
void setPort(uint16_t port);
void setPort(PortProtocol protocol_num);
```

- **功能**: 设置端口号
- **参数**: 端口号或协议枚举

### 5.7 静态成员函数

#### localHost

```cpp
static Address localHost();
```

- **功能**: 获取本地主机 IPv4 地址（127.0.0.1）
- **返回值**: 端口为 UINT16_MAX 的 Address 对象

#### localHostIPv6

```cpp
static Address localHostIPv6();
```

- **功能**: 获取本地主机 IPv6 地址（::1）
- **返回值**: 端口为 UINT16_MAX 的 Address 对象

---

## 6. OptionValue 结构体

### 6.1 结构体概述

Socket 选项的灵活值容器。通过联合体支持多种数据类型，并提供类型安全的访问。

### 6.2 ValueType 枚举

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

### 6.3 成员

```cpp
ValueType type;              // 当前值类型
int size;                    // 数据大小（sizeof）
union Value {
    int         i;           // 整数值
    uint32_t    u;           // 无符号整数值
    float       f;           // 浮点值
    char*       s;           // 字符串值
    void*       v;           // 自定义指针值
} var;                       // 存储的变量
```

### 6.4 构造函数

```cpp
OptionValue();                           // None 类型
OptionValue(int v);                      // Int 类型
OptionValue(uint32_t v);                 // UInt 类型
OptionValue(float v);                    // Float 类型
OptionValue(const char* v);              // String 类型
OptionValue(void* v, uint32_t size);     // Custom 类型
```

### 6.5 成员函数

#### set

```cpp
void set(int v);
void set(uint32_t v);
void set(float v);
void set(const char* v);
void set(void* v, uint32_t len);
```

- **功能**: 设置指定类型的值
- **参数**: 要设置的值

#### unset

```cpp
void unset();
```

- **功能**: 清除值（设置为 None 类型）

#### 赋值运算符

```cpp
OptionValue& operator=(int v);
OptionValue& operator=(uint32_t v);
OptionValue& operator=(float v);
OptionValue& operator=(char* v);
OptionValue& operator=(std::nullptr_t);
OptionValue& operator=(const OptionValue& other);
```

#### 比较运算符

```cpp
bool operator==(const OptionValue& other) const;
bool operator!=(const OptionValue& other) const;
```

---

## 7. Socket 类

### 7.1 类概述

跨平台 Socket 操作类。支持 TCP/UDP/SCTP 协议、连接管理、数据传输和 Socket 选项配置。

### 7.2 构造函数

```cpp
Socket(SocketType type = SocketType::TCP);
Socket(Socket&& other) noexcept;
```

| 参数 | 类型 | 描述 |
|------|------|------|
| `type` | `SocketType` | Socket 协议类型（默认：TCP） |

### 7.3 析构函数

```cpp
~Socket();
```

- 自动关闭 Socket 句柄

### 7.4 拷贝控制

```cpp
Socket(const Socket&) = delete;              // 禁止拷贝
Socket& operator=(const Socket&) = delete;   // 禁止拷贝赋值
Socket& operator=(Socket&& other) noexcept;  // 允许移动赋值
```

### 7.5 配置方法

#### setLocalAddress

```cpp
void setLocalAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setLocalAddress(Address&& address);
```

- **功能**: 设置本地绑定地址
- **参数**: 地址和端口信息

#### setLocalPort

```cpp
void setLocalPort(uint16_t port);
```

- **功能**: 仅设置本地端口

#### setPeerAddress

```cpp
void setPeerAddress(const char* address, uint16_t port, bool use_ipv6 = false);
void setPeerAddress(Address&& address);
```

- **功能**: 设置远程对端地址

#### setPeerPort

```cpp
void setPeerPort(uint16_t port);
```

- **功能**: 仅设置远程对端端口

#### setSocketType

```cpp
void setSocketType(SocketType type);
```

- **功能**: 设置 Socket 协议类型

### 7.6 连接方法

#### connect

```cpp
bool connect(const char* address, uint16_t port);
bool connect(const char* address, PortProtocol port);
bool connect(Address&& address);
bool connect();
```

- **功能**: 连接到远程对端
- **参数**: 远程地址和端口
- **返回值**: 连接成功返回 `true`
- **注意**: 如果无参数调用，则使用先前设置的对端地址

#### bind

```cpp
bool bind(const char* address, uint16_t port);
bool bind(const char* address, PortProtocol port);
bool bind(Address&& address);
bool bind();
```

- **功能**: 将 Socket 绑定到本地地址
- **参数**: 本地地址和端口
- **返回值**: 绑定成功返回 `true`
- **注意**: 如果无参数调用，则使用先前设置的本地地址

#### listen

```cpp
bool listen(uint16_t port, int max_connection_count);
bool listen(PortProtocol protocol_num, int max_connection_count);
bool listen(int max_connection_count);
```

- **功能**: 开始监听传入连接
- **参数**:
  - `port` / `protocol_num` - 监听的本地端口
  - `max_connection_count` - 最大待处理连接队列大小
- **返回值**: 监听成功返回 `true`
- **注意**: 如果仅使用 max_connection_count 调用，则使用先前设置的本地端口

#### accept

```cpp
bool accept(Socket& socket);
Socket accept(bool* ok = nullptr);
```

- **功能**: 接受传入连接
- **参数**:
  - `socket` - 接收已接受连接的 Socket 对象
  - `ok` - 可选指针，用于接收成功状态
- **返回值**: 已接受连接的新 Socket 对象（重载 2）
- **注意**: Socket 必须处于 Listening 状态

#### close

```cpp
bool close();
```

- **功能**: 关闭 Socket 连接
- **返回值**: 成功返回 `true`

#### shutdown

```cpp
bool shutdown();
```

- **功能**: 关闭 Socket（优雅关闭）
- **返回值**: 成功返回 `true`

### 7.7 数据传输方法

#### send

```cpp
bool send(const std::string& message, int* sended_length = nullptr);
bool send(const Datas& data, int* sended_length = nullptr);
```

- **功能**: 向已连接的对端发送数据
- **参数**:
  - `message` / `data` - 要发送的数据
  - `sended_length` - 可选指针，用于接收实际发送的字节数
- **返回值**: 发送成功返回 `true`
- **注意**: Socket 必须已连接

#### recv

```cpp
bool recv(Datas& data, size_t max_length, int* received_length = nullptr);
bool recv(std::string& message, size_t max_length, int* received_length = nullptr);
```

- **功能**: 从已连接的对端接收数据
- **参数**:
  - `data` / `message` - 接收数据的缓冲区
  - `max_length` - 最大接收字节数
  - `received_length` - 可选指针，用于接收实际接收的字节数
- **返回值**: 接收成功返回 `true`

#### sendTo

```cpp
bool sendTo(const std::string& message, const Address& address, int* sended_length = nullptr);
bool sendTo(const Datas& message, const Address& address, int* sended_length = nullptr);
```

- **功能**: 向特定地址发送数据（用于 UDP）
- **参数**:
  - `message` - 要发送的数据
  - `address` - 目标地址
  - `sended_length` - 可选指针，用于接收实际发送的字节数
- **返回值**: 发送成功返回 `true`

#### recvFrom

```cpp
bool recvFrom(std::string& message, size_t max_length, const Address& address, int* received_length = nullptr);
bool recvFrom(Datas& data, size_t max_length, const Address& address, int* received_length = nullptr);
```

- **功能**: 从特定地址接收数据（用于 UDP）
- **参数**:
  - `message` / `data` - 接收数据的缓冲区
  - `max_length` - 最大接收字节数
  - `address` - 源地址
  - `received_length` - 可选指针，用于接收实际接收的字节数
- **返回值**: 接收成功返回 `true`

### 7.8 Socket 选项方法

#### setOption

```cpp
void setOption(SocketOption option, OptionValue value, bool* ok = nullptr);
void setOption(uint32_t option_id, OptionValue value, bool* ok = nullptr);
```

- **功能**: 设置 Socket 选项
- **参数**:
  - `option` / `option_id` - 选项枚举或自定义选项 ID
  - `value` - 选项值
  - `ok` - 可选指针，用于接收成功状态
- **示例**:
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

- **功能**: 获取 Socket 选项值
- **参数**:
  - `option` / `option_id` - 选项枚举或自定义选项 ID
  - `ok` - 可选指针，用于接收成功状态
- **返回值**: 选项值

### 7.9 查询方法

#### localAddress

```cpp
const Address& localAddress() const;
```

- **功能**: 获取本地绑定地址
- **返回值**: 本地 Address 对象的引用

#### peerAddress

```cpp
const Address& peerAddress() const;
```

- **功能**: 获取远程对端地址
- **返回值**: 对端 Address 对象的引用

#### lastError

```cpp
SocketError lastError() const;
```

- **功能**: 获取最后错误码
- **返回值**: SocketError 枚举值

#### type

```cpp
SocketType type() const;
```

- **功能**: 获取 Socket 类型
- **返回值**: SocketType 枚举值

#### state

```cpp
SocketState state() const;
```

- **功能**: 获取当前 Socket 状态
- **返回值**: SocketState 枚举值

#### errorSocketOptionID

```cpp
uint32_t errorSocketOptionID() const;
```

- **功能**: 获取导致错误的选项 ID
- **返回值**: 选项 ID（如果没有选项错误则为 0）

#### nativeErrorNo

```cpp
int nativeErrorNo() const;
```

- **功能**: 获取原生系统错误码
- **返回值**: 系统 errno 值

---

## 8. 自由函数

### 8.1 parseFromHostname

```cpp
std::vector<Address> parseFromHostname(const char* hostname, bool* ok = nullptr, int* err_cnt = nullptr);
```

- **功能**: 将主机名解析为多个 IP 地址
- **参数**:
  - `hostname` - 要解析的主机名（例如："www.example.com"）
  - `ok` - 可选指针，用于接收成功状态
  - `err_cnt` - 可选指针，用于接收错误计数
- **返回值**: Address 对象向量（所有解析的地址）
- **示例**:
```cpp
bool success;
auto addresses = Tiny::Net::parseFromHostname("www.google.com", &success);
if (success) {
    for (const auto& addr : addresses) {
        std::cout << "解析结果: " << addr.toString() << std::endl;
    }
}
```

### 8.2 parseFirstHostname

```cpp
Address parseFirstHostname(const char* hostname, bool* ok = nullptr);
```

- **功能**: 将主机名解析为第一个 IP 地址
- **参数**:
  - `hostname` - 要解析的主机名
  - `ok` - 可选指针，用于接收成功状态
- **返回值**: 第一个解析的 Address 对象
- **示例**:
```cpp
bool success;
auto addr = Tiny::Net::parseFirstHostname("www.google.com", &success);
if (success) {
    std::cout << "第一个 IP: " << addr.toString() << std::endl;
}
```

### 8.3 getLastSystemError

```cpp
int getLastSystemError(std::string* info = nullptr);
```

- **功能**: 获取最后的系统错误码
- **参数**: `info` - 可选指针，用于接收错误描述
- **返回值**: 系统错误码（Unix 上的 errno，Windows 上的 WSAGetLastError）
- **示例**:
```cpp
std::string error_info;
int err = Tiny::Net::getLastSystemError(&error_info);
std::cout << "错误 " << err << ": " << error_info << std::endl;
```

### 8.4 getSystemErrorByErrno

```cpp
std::string getSystemErrorByErrno(int err_no);
```

- **功能**: 根据错误码获取错误描述
- **参数**: `err_no` - 系统错误码
- **返回值**: 错误描述字符串
- **示例**:
```cpp
std::string desc = Tiny::Net::getSystemErrorByErrno(10048);
// 返回: "Address already in use" (Windows)
```

### 8.5 getSocketErrorName

```cpp
const char* getSocketErrorName(SocketError err);
```

- **功能**: 获取错误名称字符串
- **参数**: `err` - SocketError 枚举值
- **返回值**: 错误名称字符串（例如："Success"、"ConnectionRefused"）
- **示例**:
```cpp
const char* name = Tiny::Net::getSocketErrorName(socket.lastError());
std::cout << "错误: " << name << std::endl;
```

---

## 9. 使用示例

### 9.1 TCP 客户端示例

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // 创建 TCP Socket
    Tiny::Net::Socket socket(Tiny::Net::SocketType::TCP);
    
    // 连接到服务器
    if (socket.connect("127.0.0.1", 8080)) {
        std::cout << "已连接到服务器" << std::endl;
        
        // 发送消息
        std::string message = "你好，服务器！";
        if (socket.send(message)) {
            std::cout << "已发送: " << message << std::endl;
        }
        
        // 接收响应
        std::string response;
        if (socket.recv(response, 1024)) {
            std::cout << "已接收: " << response << std::endl;
        }
        
        // 关闭连接
        socket.close();
    } else {
        std::cout << "连接失败: " 
                  << Tiny::Net::getSocketErrorName(socket.lastError()) << std::endl;
    }
    
    return 0;
}
```

### 9.2 TCP 服务器示例

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // 创建服务器 Socket
    Tiny::Net::Socket server(Tiny::Net::SocketType::TCP);
    
    // 启用地址重用
    server.setOption(Tiny::Net::SocketOption::ReuseAddr, 1);
    
    // 绑定并监听
    if (server.bind("0.0.0.0", 8080) && server.listen(8080, 10)) {
        std::cout << "服务器正在监听端口 8080" << std::endl;
        
        // 接受客户端连接
        Tiny::Net::Socket client = server.accept();
        if (client.state() == Tiny::Net::SocketState::Connected) {
            std::cout << "客户端已连接" << std::endl;
            
            // 接收消息
            std::string message;
            if (client.recv(message, 1024)) {
                std::cout << "已接收: " << message << std::endl;
                
                // 发送响应
                std::string response = "你好，客户端！";
                client.send(response);
            }
            
            client.close();
        }
        
        server.close();
    }
    
    return 0;
}
```

### 9.3 UDP 通信示例

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // 创建 UDP Socket
    Tiny::Net::Socket socket(Tiny::Net::SocketType::UDP);
    
    // 绑定到本地端口
    socket.bind("0.0.0.0", 9000);
    
    // 向特定地址发送数据
    Tiny::Net::Address dest("127.0.0.1", 9001);
    std::string message = "UDP 消息";
    socket.sendTo(message, dest);
    
    // 接收数据
    std::string buffer;
    Tiny::Net::Address sender;
    if (socket.recvFrom(buffer, 1024, sender)) {
        std::cout << "从 " << sender.toString() 
                  << " 接收: " << buffer << std::endl;
    }
    
    socket.close();
    return 0;
}
```

### 9.4 主机名解析示例

```cpp
#include "Net/Socket.hpp"
#include <iostream>

int main() {
    // 解析主机名
    bool success;
    auto addresses = Tiny::Net::parseFromHostname("www.example.com", &success);
    
    if (success && !addresses.empty()) {
        std::cout << "解析的地址:" << std::endl;
        for (size_t i = 0; i < addresses.size(); ++i) {
            std::cout << i + 1 << ". " << addresses[i].toString() << std::endl;
        }
    } else {
        std::cout << "解析主机名失败" << std::endl;
        int err = Tiny::Net::getLastSystemError();
        std::cout << "系统错误: " << err << std::endl;
    }
    
    return 0;
}
```

### 9.5 Socket 选项配置示例

```cpp
#include "Net/Socket.hpp"

int main() {
    Tiny::Net::Socket socket(Tiny::Net::SocketType::TCP);
    
    // 在连接前设置多个选项
    socket.setOption(Tiny::Net::SocketOption::ReuseAddr, 1);
    socket.setOption(Tiny::Net::SocketOption::KeepAlive, 1);
    socket.setOption(Tiny::Net::SocketOption::NoDelay, 1);
    socket.setOption(Tiny::Net::SocketOption::SendBufSize, 65536);
    socket.setOption(Tiny::Net::SocketOption::RecvBufSize, 65536);
    
    // 应用选项后连接
    socket.connect("example.com", Tiny::Net::PortProtocol::HTTP);
    
    // 获取选项值
    bool ok;
    auto keepalive = socket.option(Tiny::Net::SocketOption::KeepAlive, &ok);
    if (ok && keepalive.var.i == 1) {
        // 保活已启用
    }
    
    socket.close();
    return 0;
}
```

---

## 10. 注意事项

### 10.1 平台兼容性

- **Windows**: 需要 Winsock2 库（自动链接）
- **Unix/Linux/macOS**: 使用 BSD Socket API
- Socket 句柄类型不同：Windows 上为 `uint64_t`，Unix 上为 `int`
- 超时选项使用不同类型：Windows 上为 `uint32_t`（毫秒），Unix 上为 `timeval*`

### 10.2 Socket 生命周期

1. **创建**: Socket 初始处于 `Unused` 状态
2. **配置**: 设置本地/对端地址和选项
3. **连接**: 调用 `connect()`（客户端）或 `bind()` + `listen()` + `accept()`（服务器）
4. **通信**: TCP 使用 `send()`/`recv()`，UDP 使用 `sendTo()`/`recvFrom()`
5. **关闭**: 调用 `close()` 或让析构函数处理

### 10.3 错误处理

- 所有操作返回 `bool` 表示成功/失败
- 使用 `lastError()` 获取详细错误码
- 使用 `nativeErrorNo()` 获取系统特定错误码
- 使用 `getSocketErrorName()` 将错误转换为可读字符串

### 10.4 地址族

- IPv4: 使用 `use_ipv6 = false`（默认）
- IPv6: 使用 `use_ipv6 = true`
- Address 类自动处理地址族转换

### 10.5 线程安全

- Socket 对象**不是线程安全的**
- 每个 Socket 应该在单一线程中使用
- 对于多线程服务器，为每个连接创建独立的 Socket

### 10.6 资源管理

- Socket 类禁止拷贝，使用移动语义进行转移
- 析构函数自动关闭 Socket 句柄
- 显式调用 `close()` 提前释放资源
- 使用 RAII 模式进行自动资源管理

### 10.7 常见陷阱

- **端口已被占用**: 绑定前设置 `ReuseAddr` 选项
- **连接被拒绝**: 确保服务器正在监听且防火墙允许连接
- **地址已被使用**: 检查是否有其他进程使用相同端口
- **网络不可达**: 验证网络接口已启动且路由正确
- **阻塞操作**: 使用 `NonBlocking` 选项进行异步操作
