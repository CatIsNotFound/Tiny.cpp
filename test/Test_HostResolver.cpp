
#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace TUI;

int main(int argc, char** argv) {
    if (argc < 2) {
        Terminal::printError("Error: Please input the host name to resolve!\r\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        bool ok;
        int err_n;
        auto ret = Net::parseFromHostname(argv[i], &ok, &err_n);
        if (!ok) {
            Terminal::perror() << TStyle::fg(Color::Red, true) << "Failed to resolve host: " << argv[i]
                               << "! \r\n" << TStyle::reset();
            return 2;
        }
        Terminal::printFormat("From {}: \r\n", argv[i]);
        for (auto& addr : ret) {
            Terminal::printFormat("- {}:{}\r\n", addr.toString(), addr.port());
        }
        Terminal::printFormat("Found {} error(s) while resolving.\r\n", err_n);
    }
    return 0;
}