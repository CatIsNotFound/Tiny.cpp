
#include "../src/Tiny.hpp"
#include <stack>
#include <csignal>
#ifdef TINY_CPP_MY_OS_UNIX
#include <unistd.h>
#endif

using namespace Tiny;

static std::unique_ptr<Net::Socket> socket;
static std::vector<Net::Socket> tcp_cons;
static std::vector<Net::Address> udp_cons;

static std::string getDateTime() {
    DT::DateTime now = DT::DateTime::now();
    return now.formatString("yyyy/MM/dd HH:mm:ss.SSS");
}

void exitSig(int sig) {
    TUI::Terminal::printFormat("[{}] Closing connection...\r\n", getDateTime());
    for (auto& con : tcp_cons) {
        con.close();
    }
    socket->close();
    TUI::Terminal::printFormat("[{}] Closed by signal {}.\r\n", getDateTime(), sig);
    exit(sig);
}

int client_mode(uint8_t type, Net::Address&& address) {
    TUI::Terminal::printFormat("[{}] Connecting to {}:{}.\r\n", getDateTime(), address.toString(), address.port());
    socket.reset(new Net::Socket(type == 1 ? Net::SocketType::TCP : Net::SocketType::UDP));
    auto svr_addr = address.toString();
    auto svr_port = address.port();
    socket->setPeerAddress(std::move(address));
    if (type == 1) {
        socket->setOption(Net::SocketOption::KeepAlive, true);
        socket->setOption(Net::SocketOption::NoDelay, true);
    }
#ifdef TINY_CPP_MY_OS_WINDOWS
    socket->setOption(Net::SocketOption::RecvBufTimeout, 10000);
    socket->setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
    struct timeval tv{10, 0};
    socket->setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
    socket->setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
#endif
    Net::Address cur_addr;
    bool ok{};
    if (type == 1) {
        ok = socket->connect();
        if (!ok) {
            TUI::Terminal::printError("netchat: Failed to connect to server! Exception: {}\r\n",
                                      Net::getSocketErrorName(socket->lastError()));
            return 16;
        }
        TUI::Terminal::printFormat("[{}] Connected to {}:{}.\r\n", getDateTime(),
                                    socket->peerAddress().toString(), socket->peerAddress().port());
    } else {
        cur_addr.setAddress("0.0.0.0", 0);
        auto dt = TUI::Terminal::formatString("$D_{}_$", DT::currentTimestamps());
        int recv_len{};
        socket->sendTo(dt, socket->peerAddress());
        ok = socket->recv(dt, 1024, &recv_len, &cur_addr, false);

        if (ok && recv_len > 0) {
            TUI::Terminal::printFormat("[{}] Connected to {}:{}.\r\n", getDateTime(), svr_addr, svr_port);
            socket->sendTo("$_OK_$", socket->peerAddress());
        } else {
            TUI::Terminal::printError("netchat: Failed to connect to server! Exception: {}\r\n",
                                      Net::getSocketErrorName(socket->lastError()));
            socket->close();
            return 16;
        }
    }
    if (type == 2) {
        socket->setOption(Net::SocketOption::NonBlocking, true, &ok);
        if (!ok) {
            TUI::Terminal::printError("[{}] WARNING: The socket option 'NonBlocking' is not enabled!\r\n");
        }
    }
    std::string recv_buf;
    auto start = DT::currentTimestamps();
    auto snd_start = DT::currentTimestamps();
    while (true) {
        auto end = DT::currentTimestamps();
        auto dt = TUI::Terminal::formatString("$D_{}_$", DT::currentTimestamps());
        if (end - start >= 5000) {
            ok = socket->recv(recv_buf, 2048);
            if (!ok) {
                TUI::Terminal::printFormat("[{}] Disconnected from server! Exception: {}\r\n",
                                       getDateTime(), Net::getSocketErrorName(socket->lastError()));
                break;
            }
            TUI::Terminal::printFormat("[{}] From server: {}\r\n", getDateTime(), recv_buf);
            start = end;
        }
        if (end - snd_start >= 5000) {
            socket->sendTo(dt, socket->peerAddress());
            snd_start = end;
        }
    }
    socket->close();
    return 0;
}

int server_mode(uint8_t type, uint16_t port) {
    socket.reset(new Net::Socket(type == 1 ? Net::SocketType::TCP : Net::SocketType::UDP));
    socket->setLocalAddress("0.0.0.0", port);
    if (type == 1) {
        socket->setOption(Net::SocketOption::KeepAlive, true);
        socket->setOption(Net::SocketOption::NoDelay, true);
    } else {
        socket->setOption(Net::SocketOption::AllowedBroadcast, true);
    }
#ifdef TINY_CPP_MY_OS_WINDOWS
    socket->setOption(Net::SocketOption::RecvBufTimeout, 10000);
    socket->setOption(Net::SocketOption::SendBufTimeout, 10000);
#else
    struct timeval tv{10, 0};
    socket->setOption(Net::SocketOption::RecvBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
    socket->setOption(Net::SocketOption::SendBufTimeout, Net::OptionValue(&tv, sizeof(tv)));
#endif
    if (type == 1) {
        socket->setOption(Net::SocketOption::NonBlocking, true);
        bool is_ok = socket->listen(65535);
        if (!is_ok) {
            TUI::Terminal::printError("netchat: Failed to start serving! Exception: {}\r\n",
                                      Net::getSocketErrorName(socket->lastError()));
            return 16;
        }
    } else {
        bool is_ok = socket->bind();
        if (!is_ok) {
            TUI::Terminal::printError("netchat: Failed to bind address! Exception: {}\r\n",
                                      Net::getSocketErrorName(socket->lastError()));
            return 16;
        }
    }
    TUI::Terminal::printFormat("[{}] Start serving localhost:{}...\r\n", getDateTime(), port);
    auto start = DT::currentTimestamps();
    while (true) {
        auto end = DT::currentTimestamps();
        bool ok{};
        if (type == 1) {
            Net::Socket new_sock = socket->accept(&ok);
            if (ok) {
                auto& addr = new_sock.peerAddress();
                TUI::Terminal::printFormat("[{}] {}:{} is entered.\r\n", getDateTime(), addr.toString(), addr.port());
                tcp_cons.emplace_back(std::move(new_sock));
            }
        } else {
            std::string datas;
            int recv_len{};
            Net::Address new_addr = Net::Address::localHost();
            if (socket->recv(datas, 2048, &recv_len, &new_addr)) {
                if (new_addr.isSpecified()) {
                    auto iter = std::find_if(udp_cons.begin(), udp_cons.end(), [&new_addr](const Net::Address& s) {
                        return s == new_addr;
                    });
                    if (iter == udp_cons.end()) {
                        TUI::Terminal::printFormat("[{}] {}:{} is entered.\r\n",
                                getDateTime(), new_addr.toString(), new_addr.port());
                        socket->sendTo("$S_OK_$", new_addr);
                        udp_cons.emplace_back(std::move(new_addr));
                    } else if (recv_len > 0) {
                        TUI::Terminal::printFormat("[{}] From {}:{}: {}\r\n",
                                getDateTime(), new_addr.toString(), new_addr.port(),datas);
                    }
                }
            }
        }
        std::stack<size_t> rm_idxs{};
        size_t idx{};
        auto dt = TUI::Terminal::formatString("$D_{}_$", DT::currentTimestamps());
        std::string datas;
        auto ts = end - start;

        if (type == 1) {
            if (ts >= 5000) {
                start = end;
            }
            for (auto& sock : tcp_cons) {
                if (ts >= 5000) {
                    ok = sock.send(dt);
                    if (!ok) {
                        rm_idxs.push(idx);
                    }
                }
                int recv_len{};
                ok = sock.recv(datas, 2048, &recv_len);
                if (ok && recv_len > 0) {
                    auto& addr = sock.peerAddress();
                    TUI::Terminal::printFormat("[{}] From {}:{} message: {}\r\n", getDateTime(), addr.toString(), addr.port(),
                                               datas);
                }
                idx++;
            }
            while (!rm_idxs.empty()) {
                tcp_cons[rm_idxs.top()].close();
                auto& addr = tcp_cons[rm_idxs.top()].peerAddress();
                TUI::Terminal::printFormat("[{}] {}:{} is left.\r\n", getDateTime(), addr.toString(), addr.port());
                tcp_cons.erase(tcp_cons.begin() + rm_idxs.top());
                rm_idxs.pop();
            }
        } else {
            if (ts >= 5000) {
                start = end;
            }
            for (auto& con: udp_cons) {
                if (ts >= 5000) {
                    socket->sendTo(dt, con);
                    int recv_len{};
                    Net::Address recv_addr;
                    ok = socket->recv(datas, 2048, &recv_len, &recv_addr);
                    if (ok && recv_len > 0) {
                        TUI::Terminal::printFormat("[{}] From {}:{} message: {}\r\n", getDateTime(),
                                recv_addr.toString(), recv_addr.port(), datas);
                    } else {
                        rm_idxs.push(idx);
                    }
                }
                idx++;
            }
            while (!rm_idxs.empty()) {
                auto& addr = udp_cons[rm_idxs.top()];
                TUI::Terminal::printFormat("[{}] {}:{} is left.\r\n", getDateTime(), addr.toString(), addr.port());
                udp_cons.erase(udp_cons.begin() + rm_idxs.top());
                rm_idxs.pop();
            }
        }
    }
    socket->close();
    return 0;
}


int main(int argc, char **argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addCommand("port", "p", "Specified the port number.", true, "0", true);
    cmd_parser.addCommand("udp", "u", "Used as UDP.");
    cmd_parser.addCommand("tcp", "t", "Used as TCP.");
    cmd_parser.addFullCommand("listen", "Listening current local host as server.");
    cmd_parser.addFullCommand("connect", "Connect to a remote host.", true, "127.0.0.1");
    cmd_parser.addLastCommand("help", "h?", "Display the help information.");
    cmd_parser.addLastCommand("version", "v", "Display the version information.");

    int n;
    std::vector<std::string> missing;
    auto err = cmd_parser.exec(nullptr, &n, &missing);
    if (err != CommandParser::ParseError::NoError) {
        if (!missing.empty()) {
            TUI::Terminal::printError("netchat: Missing argument: '{}'. ", missing.front());
        } else {
            TUI::Terminal::printError("netchat: {}: '{}'. ", CommandParser::getParseErrorName(err), argv[n]);
        }
        TUI::Terminal::printError("Please type \"--help\" for more information.\r\n");
        return 1;
    }

    auto cmd_list = cmd_parser.execCommandList();
    uint8_t mode = 0;      /// Mode: 1 = Server, 2 = Client
    uint8_t sock_type = 0; /// Type: 1 = tcp, 2 = udp
    std::string host_addr{};
    uint16_t port_no{};
    for (auto& cmd : cmd_list) {
        if (cmd.option_name == "help") {
            TUI::Terminal::printLine(cmd_parser.generateHelpInfo(TUI::Terminal::screenSize().width, false, true));
            TUI::Terminal::printFormat("USAGE:\r\n"
                                      "    - To connect to remote host, execute: \r\n"
                                      "      {} connect <host> -p=<port> <-t | -u> \r\n"
                                      "    - To listen current local host, execute: \r\n"
                                      "      {} listen -p=<port> <-t | -u>\r\n", argv[0], argv[0]);
            return 0;
        }
        if (cmd.option_name == "version") {
            TUI::Terminal::printLine("netchat version 1.0");
            return 0;
        }
        if (cmd.option_name == "listen") {
            mode = 1;
        } else if (cmd.option_name == "connect") {
            mode = 2;
            if (cmd.value.empty()) {
                TUI::Terminal::printError("netcat: Which host address do you want to connect to? "
                                          "Please type \"--help\" for more information.\r\n");
                return 2;
            }
            host_addr = cmd.value;
        } else if (cmd.option_name == "tcp") {
            sock_type = 1;
        } else if (cmd.option_name == "udp") {
            sock_type = 2;
        } else if (cmd.option_name == "port") {
            port_no = std::stoi(cmd.value);
        }
    }
    if (mode == 0) {
        TUI::Terminal::printError("netchat: Unspecified command! Please type \"--help\" for more information.\r\n");
        return 4;
    }
    if (sock_type == 0) {
        TUI::Terminal::printError("netchat: Unspecified network protocol! Please type \"--help\" for more information.\r\n");
        return 4;
    }
    bool ok{};
    Net::Address address = (mode == 2) ? Net::Address::parseFirstHostname(host_addr.data(), &ok)
                                       : Net::Address::localHost();
    if (mode == 2 && !ok) {
        TUI::Terminal::printError("netchat: Invalid host address: \"{}\"!\r\n", host_addr);
        return 8;
    }
    address.setPort(port_no);
    signal(SIGINT, exitSig);

    return mode == 1 ? server_mode(sock_type, port_no) : client_mode(sock_type, std::move(address));
}

