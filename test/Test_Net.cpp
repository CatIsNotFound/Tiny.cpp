
#include "../src/TUI/Terminal.hpp"
#include "../src/Parser/CommandParser.hpp"
#include "../src/DateTime/DateTime.hpp"
#include "../src/Net/Socket.hpp"

using namespace Tiny;
using namespace Net;

static uint64_t ID = 0;

void test_server() {
    Socket server;
    DT::Duration start = DT::currentTimestamps();
    if (server.listen(8080)) {
        Socket client;
        while (true) {
            DT::Duration now = DT::currentTimestamps();
            if (client.state() != SocketState::Connected) client = server.accept();
            if (now - start > 1000) {
                if (client.state() != SocketState::Connected) {
                    TUI::Terminal::printFormat("[{:>16s}] Waiting...\r", std::to_string(DT::currentTimestamps()));
                } else {
                    std::string msg;
                    auto ok = client.send(std::to_string(DT::currentTimestamps()), nullptr);
                    if (ok) {
                        bool is_ok = client.recv(msg, 255);
                        if (is_ok) TUI::Terminal::printFormat("\r\nRecv: {}\r\n", msg);
                    } else {
                        client.close();
                    }
                    TUI::Terminal::printLine(ok ? "\r\nOk!" : "\r\nDisconnected!");
                }
                start = now;
            }
            auto input = TUI::Terminal::getInput();
            if (input.type == 1) {
                if (input.input.keyboard.key == TUI::KEY_CTRL_D) break;
            }
        }
        server.close();
    } else {
        TUI::Terminal::printFormat("Failed to start! Exception: {}\r\n", Net::getSocketErrorName(server.lastError()));
    }
}

void test_client() {
    Socket client;
    DT::Duration start = DT::currentTimestamps();
    client.setOption(SocketOption::KeepAlive, true);
    client.setOption(SocketOption::NonBlocking, true);
    if (client.connect("127.0.0.1", 8080)) {
        while (true) {
            DT::Duration now = DT::currentTimestamps();
            if (now - start > 1000) {
                std::string msg;
                auto recv_sz = client.recv(msg, 255);
                bool is_sent = client.send("Hello!\r\n", nullptr);
                if (recv_sz) {
                    TUI::Terminal::printFormat("Recv: {}\r\n", msg);
                } else {
                    break;
                }
                TUI::Terminal::printLine(is_sent ? "Sent" : "Failed!");
                start = now;
            }
            auto input = TUI::Terminal::getInput();
            if (input.type == 1) {
                if (input.input.keyboard.key == TUI::KEY_CTRL_D) break;
            }
        }
        client.close();
    } else {
        TUI::Terminal::printFormat("Failed to connect! Exception: {}\r\n", Net::getSocketErrorName(client.lastError()));
    }
}

int main(int argc, char **argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addFullLastCommand("server");
    cmd_parser.addFullLastCommand("client");
    auto ok = cmd_parser.exec();
    if (ok != CommandParser::ParseError::NoError) {
        TUI::Terminal::printError("E: {}\r\n", CommandParser::getParseErrorName(ok));
        return 1;
    }
    auto cmd_list = cmd_parser.execCommandList();
    for (auto& cmd : cmd_list) {
        if (cmd.option_name == "server") {
            TUI::Terminal::printLine("Using Server mode...");
            test_server();
            return 0;
        }
        if (cmd.option_name == "client") {
            TUI::Terminal::printLine("Using Client mode...");
            test_client();
            return 0;
        }
    }
    return 0;
}
