
#include "../src/Tiny.hpp"
#include <csignal>
#include <cstdlib>
using namespace Tiny;

Net::Socket server, client;

void exitSig(int) {
    if (server.state() != Net::SocketState::Unused) {
        TUI::Terminal::printFormat("[{}] Closing server...\r\n", DT::currentTimestamps());
        server.close();
    }
    if (client.state() != Net::SocketState::Unused) {
        TUI::Terminal::printFormat("[{}] Disconnecting from server...\r\n", DT::currentTimestamps());
        client.close();
    }
    TUI::Terminal::printFormat("[{}] Closed connection.\r\n", DT::currentTimestamps());
    exit(0);
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

static bool is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static void strip(std::string& str) {
    size_t st = 0;
    while (st < str.size()) {
        if (!is_space(str[st])) break;
        st++;
    }
    size_t ed = str.size() - 1;
    while (ed > st) {
        if (!is_space(str[ed])) break;
        ed--;
    }
    str = str.substr(st, ed - st + 1);
}

static int str2Num(const std::string& str) {
    int num = 0;
    bool neg = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '-' && !neg) {
            neg = true;
        } else if (str[i] >= '0' && str[i] <= '9') {
            num = num * 10 + (str[i] - '0');
        } else {
            break;
        }
    }
    if (neg) num *= -1;
    return num;
}

void print_help(CommandParser& cmd_parser) {
    auto scr = TUI::Terminal::screenSize();
    TUI::Terminal::printFormat("Usage: TestNet <server/client> ... \r\n\r\nOPTIONS:\r\n");
    for (auto& cmd : cmd_parser) {
        TUI::Terminal::printFormat("{:>8c}--{:<6s}, ", ' ', cmd.first);
        int idx = 0;
        std::string short_arg;
        for (auto& opt : cmd.second.short_options) {
            short_arg += TUI::Terminal::formatString("-{}", opt);
            if (++idx < cmd.second.short_options.size()) {
                short_arg += " | ";
            }
        }

        TUI::Terminal::printFormat("{:<16s} ", short_arg.c_str());
        auto pos = TUI::Terminal::cursorPosition();
        size_t dis_size = scr.width - pos.column;
        if (dis_size < cmd.second.description.size()) {
            auto texts = split(cmd.second.description, dis_size);
            for (size_t i = 0; i < texts.size(); ++i) {
                TUI::Terminal::printLine(texts[i]);
                if (i != texts.size() - 1) {
                    TUI::Terminal::moveCursor(pos.row + i, pos.column);
                }
            }
        } else {
            TUI::Terminal::printLine(cmd.second.description);
        }
    }
}

void test_server(Net::Address&& address) {
    std::vector<Net::Socket> client_list;
    client_list.reserve(256);
    server.setLocalAddress(std::move(address));
    server.setOption(Net::SocketOption::KeepAlive, true);
    server.setOption(Net::SocketOption::ReuseAddr, true);
    server.setOption(Net::SocketOption::NonBlocking, true);
    if (server.listen(256)) {
        auto start = DT::currentTimestamps();
        TUI::Terminal::printFormat("[{}] Started server.\r\n", start);
        while (true) {
            auto end = DT::currentTimestamps();
            if (end - start > 1000) {
                bool ok;
                auto new_svr = server.accept(&ok);
                if (ok) {
                    TUI::Terminal::printFormat("[{}] Connected: {}:{}\r\n", DT::currentTimestamps(),
                            new_svr.localAddress().toString(), new_svr.localAddress().port());
                    client_list.emplace_back(std::move(new_svr));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                size_t rm_idx{};
                std::vector<size_t> rm_list_idx;
                for (auto& client : client_list) {
                    std::string msg;
                    ok = client.recv(msg, 1024);
                    if (ok) {
                        TUI::Terminal::printFormat("[{}] From {}:{}: {}\r\n", DT::currentTimestamps(),
                                               client.localAddress().toString(), client.localAddress().port(), msg);
                    }
                    bool is_ok = client.send(TUI::Terminal::formatString("From Server at {}.", DT::currentTimestamps()));
                    if (!is_ok) {
                        TUI::Terminal::printFormat("[{}] Disconnect: {}:{}\r\n", DT::currentTimestamps(),
                            client.localAddress().toString(), client.localAddress().port());
                        client.close();
                        rm_list_idx.insert(rm_list_idx.begin(), rm_idx);
                    }
                    rm_idx++;
                }
                for (auto& id : rm_list_idx) {
                    client_list.erase(client_list.begin() + id);
                }
                start = end;
            }
        }
    } else {
        TUI::Terminal::perror() << "TestNet: Failed to starting server! Exception: "
                                << Net::getSocketErrorName(server.lastError()) << "\r\n";
        exit(8);
    }
}

void test_client(Net::Address&& address) {
    client.setPeerAddress(std::move(address));
    client.setOption(Net::SocketOption::KeepAlive, true);
    client.setOption(Net::SocketOption::NonBlocking, true);
    auto start = DT::currentTimestamps();
    if (client.connect()) {
        while (true) {
            auto end = DT::currentTimestamps();
            if (end - start > 1000) {
                bool ok = client.send(TUI::Terminal::formatString("From Client at {}.", DT::currentTimestamps()));
                if (!ok) {
                    TUI::Terminal::perror() << "TestNet: Connection error! Exception: "
                                << Net::getSocketErrorName(client.lastError()) << "\r\n";
                    client.close();
                    exit(10);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                std::string msg;
                ok = client.recv(msg, 1024);
                if (!ok) {
                    TUI::Terminal::perror() << "TestNet: Connection error! Exception: "
                                << Net::getSocketErrorName(client.lastError()) << "\r\n";
                    client.close();
                    exit(10);
                }
                TUI::Terminal::printFormat("[{}] From {}:{}: {}\r\n", DT::currentTimestamps(),
                    client.peerAddress().toString(), client.peerAddress().port(), msg);
                start = end;
            }
        }
    } else {
        TUI::Terminal::perror() << "TestNet: Failed to connect server! Exception: "
                                << Net::getSocketErrorName(client.lastError()) << "\r\n";
        exit(8);
    }
}

int main(int argc, char *argv[]) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addFullCommand("server", "Using server mode");
    cmd_parser.addFullCommand("client", "Using client mode");
    cmd_parser.addCommand("host", "H",
        "The IP address designated for connecting to the server", true, "127.0.0.1");
    cmd_parser.addCommand("port", "p",
        "Specify the port number to listen on or connect to", true, "8000");
    cmd_parser.addLastCommand("help", "h?", "Display the help information");
    int en;
    auto err = cmd_parser.exec(nullptr, &en);
    if (err != CommandParser::ParseError::NoError) {
        switch (err) {
            case CommandParser::ParseError::UnknownOption:
                TUI::Terminal::perror() << "TestNet: Unknown option: " << argv[en] << "\r\n";
                break;
            case CommandParser::ParseError::InvalidValue:
                TUI::Terminal::perror() << "TestNet: Invalid value: " << argv[en] << "\r\n";
                break;
            case CommandParser::ParseError::MissingArgument:
                TUI::Terminal::perror() << "TestNet: Missing argument: " << argv[en] << "\r\n";
                break;
            case CommandParser::ParseError::FormatError:
                TUI::Terminal::perror() << "TestNet: Format error at pos." << en << "\r\n";
                break;
            default:
                TUI::Terminal::perror() << "TestNet: Unknown error.\r\n";
                break;
        }
        TUI::Terminal::perror() << "Type '--help' to show details.\r\n";
        return 1;
    }
    auto exec_list = cmd_parser.execCommandList();


    if (exec_list.empty()) {
        TUI::Terminal::perror() << "TestNet: Please specified the mode. (server/client)\r\n";
        TUI::Terminal::perror() << "Type '--help' to show details.\r\n";
        return 2;
    }

    uint8_t mode = 0;
    Net::Address usr_addr;
    std::string usr_addr_str{"127.0.0.1"};
    uint16_t usr_port{};
    for (auto& command : exec_list) {
        if (command.option_name == "help") {
            print_help(cmd_parser);
            return 0;
        }
        if (command.option_name == "server") {
            mode = 1;
        }
        if (command.option_name == "client") {
            mode = 2;
        }
        if (command.option_name == "host") {
            usr_addr_str = command.value;
        }
        if (command.option_name == "port") {
            strip(command.value);
            usr_port = str2Num(command.value);
        }
    }
    if (mode == 0) {
        TUI::Terminal::perror() << "TestNet: Please specified the mode. (server/client)\r\n";
        TUI::Terminal::perror() << "Type '--help' to show details.\r\n";
        return 2;
    }

    usr_addr.setAddress(usr_addr_str.c_str(), usr_port);
    if (!usr_addr.isValid()) {
        TUI::Terminal::perror() << "TestNet: The specified IP address is not valid!\r\n";
        TUI::Terminal::perror() << "Type '--help' to show details.\r\n";
        return 3;
    }

    signal(SIGINT, exitSig);
    if (mode == 1) {
        test_server(std::move(usr_addr));
    } else if (mode == 2) {
        test_client(std::move(usr_addr));
    }
    return 0;
}
